/**
 * @file msg.cpp
 *
 * Implementation of function for sending and reciving network messages.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"

namespace dvl {

static DWORD sgdwOwnerWait;
static DWORD sgdwRecvOffset;
static int sgnCurrMegaPlayer;
static DLevel sgLevels[NUMLEVELS]; // Basically contains a view of all levels as understood from multiplayer sync
static Cmd sbLastCmd;
static TMegaPkt *sgpCurrPkt;
static uint8_t sgRecvBuf[sizeof(DLevel) + 1];
static Cmd sgbRecvCmd;
static LocalLevel sgLocals[NUMLEVELS];
static DJunk sgJunk;
static TMegaPkt *sgpMegaPkt;
static bool sgbDeltaChanged;
static uint8_t sgbDeltaChunks;
bool deltaload;
uint8_t gbBufferMsgs;
int dwRecCount;

void msg_send_drop_pkt(int pnum, int reason)
{
	TFakeDropPlr cmd;

	cmd.dwReason = reason;
	cmd.bCmd = Cmd::FAKE_DROPID;
	cmd.bPlr = pnum;
	msg_send_packet(pnum, &cmd, sizeof(cmd));
}

void msg_send_packet(int pnum, const void *packet, DWORD dwSize)
{
	TMegaPkt *packeta;
	TFakeCmdPlr cmd;

	if (pnum != sgnCurrMegaPlayer) {
		sgnCurrMegaPlayer = pnum;
		cmd.bCmd = Cmd::FAKE_SETID;
		cmd.bPlr = pnum;
		msg_send_packet(pnum, &cmd, sizeof(cmd));
	}
	packeta = sgpCurrPkt;
	if (sgpCurrPkt->dwSpaceLeft < dwSize) {
		msg_get_next_packet();
		packeta = sgpCurrPkt;
	}
	memcpy((char *)&packeta[1] - packeta->dwSpaceLeft, packet, dwSize);
	sgpCurrPkt->dwSpaceLeft -= dwSize;
}

void msg_get_next_packet()
{
	TMegaPkt *result;

	sgpCurrPkt = (TMegaPkt *)DiabloAllocPtr(sizeof(TMegaPkt));
	sgpCurrPkt->pNext = NULL;
	sgpCurrPkt->dwSpaceLeft = 32000;

	result = (TMegaPkt *)&sgpMegaPkt;
	while (result->pNext) {
		result = result->pNext;
	}
	result->pNext = sgpCurrPkt;
}

bool msg_wait_resync()
{
	bool success;

	msg_get_next_packet();
	sgbDeltaChunks = 0;
	sgnCurrMegaPlayer = -1;
	sgbRecvCmd = Cmd::DLEVEL_END;
	gbBufferMsgs = 1;
	sgdwOwnerWait = SDL_GetTicks();
	success = UiProgressDialog("Waiting for game data...", 1, msg_wait_for_turns, 20);
	gbBufferMsgs = 0;
	if (!success) {
		msg_free_packets();
		return false;
	}

	if (gbGameDestroyed) {
		DrawDlg("The game ended");
		msg_free_packets();
		return false;
	}

	if (sgbDeltaChunks != 21) {
		DrawDlg("Unable to get level data");
		msg_free_packets();
		return false;
	}

	return true;
}

void msg_free_packets()
{
	while (sgpMegaPkt) {
		sgpCurrPkt = sgpMegaPkt->pNext;
		MemFreeDbg(sgpMegaPkt);
		sgpMegaPkt = sgpCurrPkt;
	}
}

int msg_wait_for_turns()
{
	bool received;
	DWORD turns;
	if (!sgbDeltaChunks) {
		nthread_send_and_recv_turn(0, 0);
		if (!SNetGetOwnerTurnsWaiting(&turns) && SErrGetLastError() == STORM_ERROR_NOT_IN_GAME)
			return 100;
		if (SDL_GetTicks() - sgdwOwnerWait <= 2000 && turns < gdwTurnsInTransit)
			return 0;
		sgbDeltaChunks++;
	}
	multi_process_network_packets();
	nthread_send_and_recv_turn(0, 0);
	if (nthread_has_500ms_passed(false))
		nthread_recv_turns(&received);

	if (gbGameDestroyed)
		return 100;
	if (gbDeltaSender >= MAX_PLRS) {
		sgbDeltaChunks = 0;
		sgbRecvCmd = Cmd::DLEVEL_END;
		gbDeltaSender = uint8_t(myplr().id());
		nthread_set_turn_upper_bit();
	}
	if (sgbDeltaChunks == 20) {
		sgbDeltaChunks = 21;
		return 99;
	}
	return 100 * sgbDeltaChunks / 21;
}

void run_delta_info()
{
	if (game.isMultiplayer()) {
		gbBufferMsgs = 2;
		msg_pre_packet();
		gbBufferMsgs = 0;
		msg_free_packets();
	}
}

void msg_pre_packet()
{
	int i;
	int spaceLeft, pktSize;
	TMegaPkt *pkt;
	TFakeCmdPlr *cmd, *tmpCmd;
	TFakeDropPlr *dropCmd;

	TMegaPkt * pkt = sgpMegaPkt;
	for (i = -1; pkt; pkt = pkt->pNext) {
		spaceLeft = 32000;
		cmd = (TFakeCmdPlr *)pkt->data;
		while (spaceLeft != pkt->dwSpaceLeft) {
			if (cmd->bCmd == Cmd::FAKE_SETID) {
				tmpCmd = cmd;
				cmd++;
				i = tmpCmd->bPlr;
				spaceLeft -= sizeof(*cmd);
			} else if (cmd->bCmd == Cmd::FAKE_DROPID) {
				dropCmd = (TFakeDropPlr *)cmd;
				cmd += 3;
				spaceLeft -= sizeof(*dropCmd);
				multi_player_left(dropCmd->bPlr, dropCmd->dwReason);
			} else {
				pktSize = ParseCmd(i, (TCmd *)cmd);
				cmd = (TFakeCmdPlr *)((char *)cmd + pktSize);
				spaceLeft -= pktSize;
			}
		}
	}
}

void DeltaExportData(int pnum)
{
	uint8_t *dst, *dstEnd;
	int size, i;
	char src;

	if (sgbDeltaChanged) {
		dst = (uint8_t *)DiabloAllocPtr(4722);
		for (i = 0; i < NUMLEVELS; i++) {
			dstEnd = dst + 1;
			dstEnd = DeltaExportItem(dstEnd, sgLevels[i].item);
			dstEnd = DeltaExportObject(dstEnd, sgLevels[i].object);
			dstEnd = DeltaExportMonster(dstEnd, sgLevels[i].monster);
			size = msg_comp_level(dst, dstEnd);
			dthread_send_delta(pnum, i + int(Cmd::DLEVEL_0), dst, size);
		}
		dstEnd = dst + 1;
		dstEnd = DeltaExportJunk(dstEnd);
		size = msg_comp_level(dst, dstEnd);
		dthread_send_delta(pnum, int(Cmd::DLEVEL_JUNK), dst, size);
		mem_free_dbg(dst);
	}
	src = 0;
	dthread_send_delta(pnum, int(Cmd::DLEVEL_END), &src, 1);
}

uint8_t *DeltaExportItem(uint8_t *dst, TCmdPItem *src)
{
	int i;
	for (i = 0; i < MAXITEMS; i++) {
		if (src->bCmd == Cmd::INVALID) {
			*dst = 0xFF;
			dst++;
		} else {
			memcpy(dst, src, sizeof(TCmdPItem));
			dst += sizeof(TCmdPItem);
		}
		src++;
	}

	return dst;
}

uint8_t *DeltaExportObject(uint8_t *dst, DObjectStr *src)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return dst + sizeof(DObjectStr) * MAXOBJECTS;
}

uint8_t *DeltaExportMonster(uint8_t *dst, DMonsterStr *src)
{
	int i;

	for (i = 0; i < MAXMONSTERS; i++) {
		if (*(uint8_t *)src == 0xFF) {
			*dst = 0xFF;
			dst++;
		} else {
			memcpy(dst, src, sizeof(DMonsterStr));
			dst += sizeof(DMonsterStr);
		}
		src++;
	}

	return dst;
}

uint8_t *DeltaExportJunk(uint8_t *dst)
{
	int i;
	MultiQuests *mq;
	DPortal *pD;

	for (i = 0; i < MAXPORTAL; i++) {
		pD = &sgJunk.portal[i];
		if (pD->x == 0xFF) {
			*dst = 0xFF;
			dst++;
		} else {
			memcpy(dst, pD, sizeof(*pD));
			dst += sizeof(*pD);
		}
	}

	mq = sgJunk.quests;
	for (i = 0; i < MAXMULTIQUESTS; i++) {
		if (questlist[i]._qflags & 1) {
			mq->qlog = quests[i]._qlog;
			mq->qstate = quests[i]._qactive;
			mq->qvar1 = quests[i]._qvar1;
			memcpy(dst, mq, sizeof(*mq));
			dst += sizeof(*mq);
			mq++;
		}
	}

	return dst;
}

int msg_comp_level(uint8_t *buffer, uint8_t *end)
{
	int size = end - buffer - 1;
	int pkSize = PkwareCompress(buffer + 1, size);
	*buffer = size != pkSize;
	return pkSize + 1;
}

void delta_init()
{
	sgbDeltaChanged = false;
	memset(&sgJunk, 0xFF, sizeof(sgJunk));
	memset(sgLevels, 0xFF, sizeof(sgLevels));
	memset(sgLocals, 0, sizeof(sgLocals));
	deltaload = false;
}

void delta_kill_monster(ActorId mi, V2Di pos, uint8_t bLevel)
{
	if (game.isMultiplayer()) {
		sgbDeltaChanged = true;
		DMonsterStr* pD = &sgLevels[bLevel].monster[mi];
		pD->_mx = pos.x;
		pD->_my = pos.y;
		assert(actors.getMonster(mi))
		pD->_mdir = int(monsters[mi].data._mdir);
		pD->_mhitpoints = 0;
	}
}

void delta_monster_hp(int mi, int hp, uint8_t bLevel)
{
	DMonsterStr *pD;

	if (game.isMultiplayer()) {
		sgbDeltaChanged = true;
		pD = &sgLevels[bLevel].monster[mi];
		if (pD->_mhitpoints > hp)
			pD->_mhitpoints = hp;
	}
}

void delta_sync_monster(const TSyncMonster *pSync, uint8_t bLevel)
{
	DMonsterStr *pD;

	if (game.isSingleplayer()) {
		return;
	}

	/// ASSERT: assert(pSync != NULL);
	/// ASSERT: assert(bLevel < NUMLEVELS);
	sgbDeltaChanged = true;

	pD = &sgLevels[bLevel].monster[pSync->_mndx];
	if (pD->_mhitpoints != 0) {
		pD->_mx = pSync->_mx;
		pD->_my = pSync->_my;
		pD->_mactive = UINT8_MAX;
		pD->_menemy = pSync->_menemy;
	}
}

void delta_sync_golem(TCmdGolem *pG, int pnum, uint8_t bLevel)
{
	DMonsterStr *pD;

	if (game.isMultiplayer()) {
		sgbDeltaChanged = true;
		pD = &sgLevels[bLevel].monster[pnum];
		pD->_mx = pG->_mx;
		pD->_my = pG->_my;
		pD->_mactive = UINT8_MAX;
		pD->_menemy = pG->_menemy;
		pD->_mdir = pG->_mdir;
		pD->_mhitpoints = pG->_mhitpoints;
	}
}

void delta_leave_sync(uint8_t bLevel)
{
	int i, ma;
	DMonsterStr *pD;

	if (game.isMultiplayer()) {
		if (lvl.currlevel == 0) {
			glSeedTbl[0] = GetRndSeed();
		}
		if (lvl.currlevel > 0) {
			for (i = 0; i < nummonsters; ++i) {
				ma = monstactive[i];
				if (monsters[ma].data._mhitpoints) {
					sgbDeltaChanged = true;
					pD = &sgLevels[bLevel].monster[ma];
					pD->_mx = monsters[ma].data._m.x;
					pD->_my = monsters[ma].data._m.y;
					pD->_mdir = int(monsters[ma].data._mdir);
					pD->_menemy = encode_enemy(ma);
					pD->_mhitpoints = monsters[ma].data._mhitpoints;
					pD->_mactive = monsters[ma].data._msquelch;
				}
			}
			memcpy(&sgLocals[bLevel], automap.getView(), sizeof(automap.getView()));
		}
	}
}

bool delta_portal_inited(int i)
{
	return sgJunk.portal[i].x == 0xFF;
}

bool delta_quest_inited(int i)
{
	return sgJunk.quests[i].qstate != 0xFF;
}

void DeltaAddItem(Item & item)
{
	if (game.isSingleplayer()) return;
	TCmdPItem* pD = sgLevels[lvl.currlevel].item;
	for (int i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != 0xFF
		    && pD->wIndx == item.IDidx
		    && pD->wCI == item._iCreateInfo
		    && pD->dwSeed == item._iSeed
		    && (pD->bCmd == int(Cmd::WALKXY) || pD->bCmd == int(Cmd::STAND))) {
			return;
		}
	}

	pD = sgLevels[lvl.currlevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == 0xFF) {
			pD->bCmd = Cmd::STAND;
			sgbDeltaChanged = true;
			pD->x = item._i.x;
			pD->y = item._i.y;
			pD->wIndx = item.IDidx;
			pD->wCI = item._iCreateInfo;
			pD->dwSeed = item._iSeed;
			pD->bId = item._iIdentified;
			pD->bDur = item._iDurability;
			pD->bMDur = item._iMaxDur;
			pD->bCh = item._iCharges;
			pD->bMCh = item._iMaxCharges;
			pD->wValue = item._ivalue;
			return;
		}
	}
}

void DeltaSaveLevel()
{
	if (game.isMultiplayer()) {


		for (int i = 0; i < MAX_PLRS; i++) {
			if (i != myplr())
				plr[i].data._pGFXLoad = 0;
		}
		myplr()._pLvlVisited[lvl.currlevel] = true;
		delta_leave_sync(lvl.currlevel);
	}
}

void DeltaLoadLevel()
{
	int ii, ot;
	int i, j, k, l;
	V2Di pos, posb;
	bool done;

	if (game.isSingleplayer()) {
		return;
	}

	deltaload = true;
	if (lvl.currlevel != 0) {
		for (i = 0; i < nummonsters; i++) {
			if (sgLevels[lvl.currlevel].monster[i]._mx != 0xFF) {
				monsters[i].M_ClearSquares();
				pos.x = sgLevels[lvl.currlevel].monster[i]._mx;
				pos.y = sgLevels[lvl.currlevel].monster[i]._my;
				monsters[i].data._m = pos;
				monsters[i].data._mold = pos;
				monsters[i].data._mfut = pos;
				if (sgLevels[lvl.currlevel].monster[i]._mhitpoints != -1)
					monsters[i].data._mhitpoints = sgLevels[lvl.currlevel].monster[i]._mhitpoints;
				if (!sgLevels[lvl.currlevel].monster[i]._mhitpoints) {
					monsters[i].data._mold = pos;
					monsters[i].M_ClearSquares();
					if (monsters[i].data._mAi != MonstAi::DIABLO) {
						if (!monsters[i].data._uniqtype)
							/// ASSERT: assert(monsters[i].data.MType != NULL);
							AddDead(monsters[i].data._m, monsters[i].data.MType->mdeadval, monsters[i].data._mdir);
						else
							AddDead(monsters[i].data._m, monsters[i].data._udeadval, monsters[i].data._mdir);
					}
					monsters[i].data._mDelFlag = true;
					monsters[i].M_UpdateLeader();
				} else {
					decode_enemy(i, sgLevels[lvl.currlevel].monster[i]._menemy);
					if (monsters[i].data._m.x && monsters[i].data._m.x != 1 || monsters[i].data._m.y)
						grid[monsters[i].data._m.x][monsters[i].data._m.y].setActor(i);
					if (i < MAX_PLRS) {
						monsters[i].MMonstAi::Golum();
						monsters[i].data._mFlags |= (MonsterFlag::targets_monster | MonsterFlag::golem);
					} else {
						monsters[i].M_StartStand(monsters[i].data._mdir);
					}
					monsters[i].data._msquelch = sgLevels[lvl.currlevel].monster[i]._mactive;
				}
			}
		}
		memcpy(automap.getView(), &sgLocals[lvl.currlevel], sizeof(automap.getView()));
	}

	for (i = 0; i < MAXITEMS; i++) {
		if (sgLevels[lvl.currlevel].item[i].bCmd != 0xFF) {
			if (sgLevels[lvl.currlevel].item[i].bCmd == Cmd::WALKXY) {
				ii = FindGetItem(
				    sgLevels[lvl.currlevel].item[i].wIndx,
				    sgLevels[lvl.currlevel].item[i].wCI,
				    sgLevels[lvl.currlevel].item[i].dwSeed);
				if (ii != -1) {
					if (grid.at(item[ii]._i).getItem() == ii)
						grid.at(item[ii]._i).clearItem();
					DeleteItem(ii, i);
				}
			}
			if (sgLevels[lvl.currlevel].item[i].bCmd == Cmd::ACK_PLRINFO) {
				ii = itemavail[0];
				itemavail[0] = itemavail[MAXITEMS - numitems - 1];
				itemactive[numitems] = ii;
				if (sgLevels[lvl.currlevel].item[i].wIndx == ItemIndex::EAR) {
					RecreateEar(
					    ii,
					    sgLevels[lvl.currlevel].item[i].wCI,
					    sgLevels[lvl.currlevel].item[i].dwSeed,
					    sgLevels[lvl.currlevel].item[i].bId,
					    sgLevels[lvl.currlevel].item[i].bDur,
					    sgLevels[lvl.currlevel].item[i].bMDur,
					    sgLevels[lvl.currlevel].item[i].bCh,
					    sgLevels[lvl.currlevel].item[i].bMCh,
					    sgLevels[lvl.currlevel].item[i].wValue,
					    sgLevels[lvl.currlevel].item[i].dwBuff);
				} else {
					RecreateItem(
					    ii,
					    sgLevels[lvl.currlevel].item[i].wIndx,
					    sgLevels[lvl.currlevel].item[i].wCI,
					    sgLevels[lvl.currlevel].item[i].dwSeed,
					    sgLevels[lvl.currlevel].item[i].wValue);
					if (sgLevels[lvl.currlevel].item[i].bId)
						item[ii]._iIdentified = true;
					item[ii]._iDurability = sgLevels[lvl.currlevel].item[i].bDur;
					item[ii]._iMaxDur = sgLevels[lvl.currlevel].item[i].bMDur;
					item[ii]._iCharges = sgLevels[lvl.currlevel].item[i].bCh;
					item[ii]._iMaxCharges = sgLevels[lvl.currlevel].item[i].bMCh;
				}
				pos.x = sgLevels[lvl.currlevel].item[i].x;
				pos.y = sgLevels[lvl.currlevel].item[i].y;
				if (!CanPut(pos)) {
					done = false;
					for (k = 1; k < 50 && !done; k++) {
						for (j = -k; j <= k && !done; j++) {
							posb.y = pos.y + j;
							for (l = -k; l <= k && !done; l++) {
								posb.x = pos.x + l;
								if (CanPut(posb)) {
									done = true;
									pos = posb;
								}
							}
						}
					}
				}
				item[ii]._i = pos;
				grid.at(pos).setItem(ii);
				RespawnItem(ii, false);
				numitems++;
			}
		}
	}

	if (lvl.currlevel != 0) {
		for (i = 0; i < MAXOBJECTS; i++) {
			switch (sgLevels[lvl.currlevel].object[i].bCmd) {
			case Cmd::OPENDOOR:
			case Cmd::CLOSEDOOR:
			case Cmd::OPERATEOBJ:
			case Cmd::PLROPOBJ:
				SyncOpObject(-1, sgLevels[lvl.currlevel].object[i].bCmd, i);
				break;
			case Cmd::BREAKOBJ:
				SyncBreakObj(-1, i);
				break;
			}
		}

		for (i = 0; i < nobjects; i++) {
			ot = object[objectactive[i]]._otype;
			if (ot == ObjectType::TRAPL || ot == ObjectType::TRAPR)
				Obj_Trap(objectactive[i]);
		}
	}
	deltaload = false;
}

void NetSendCmd(bool bHiPri, Cmd bCmd)
{
	TCmd cmd;

	cmd.bCmd = int(bCmd);
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdGolem(V2Di pos, uint8_t dir, uint8_t menemy, int hp, uint8_t cl)
{
	TCmdGolem cmd;

	cmd.bCmd = int(Cmd::AWAKEGOLEM);
	cmd._mx = pos.x;
	cmd._my = pos.y;
	cmd._mdir = dir;
	cmd._menemy = menemy;
	cmd._mhitpoints = hp;
	cmd.currlevel = cl;
	NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdLoc(bool bHiPri, Cmd bCmd, V2Di pos)
{
	ALIGN_BY_1 TCmdLoc cmd;

	cmd.bCmd = int(bCmd);
	cmd.x = pos.x;
	cmd.y = pos.y;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam1(bool bHiPri, Cmd bCmd, V2Di pos, WORD wParam1)
{
	TCmdLocParam1 cmd;

	cmd.bCmd = int(bCmd);
	cmd.x = pos.x;
	cmd.y = pos.y;
	cmd.wParam1 = wParam1;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam2(bool bHiPri, Cmd bCmd, V2Di pos, WORD wParam1, WORD wParam2)
{
	TCmdLocParam2 cmd;

	cmd.bCmd = int(bCmd);
	cmd.x = pos.x;
	cmd.y = pos.y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam3(bool bHiPri, Cmd bCmd, V2Di pos, WORD wParam1, WORD wParam2, WORD wParam3)
{
	TCmdLocParam3 cmd;

	cmd.bCmd = int(bCmd);
	cmd.x = pos.x;
	cmd.y = pos.y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

// wParam 1 : attribute BASE value
void NetSendCmdParam1(bool bHiPri, Cmd bCmd, WORD wParam1)
{
	ALIGN_BY_1 TCmdParam1 cmd;

	cmd.bCmd = int(bCmd);
	cmd.wParam1 = wParam1;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdParam2(bool bHiPri, Cmd bCmd, WORD wParam1, WORD wParam2)
{
	TCmdParam2 cmd;

	cmd.bCmd = int(bCmd);
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdParam3(bool bHiPri, Cmd bCmd, WORD wParam1, WORD wParam2, WORD wParam3)
{
	TCmdParam3 cmd;

	cmd.bCmd = int(bCmd);
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdQuest(bool bHiPri, QuestId q)
{
	TCmdQuest cmd;

	cmd.q = int(q);
	cmd.bCmd = int(Cmd::SYNCQUEST);
	cmd.qstate = quests[int(q)]._qactive;
	cmd.qlog = quests[int(q)]._qlog;
	cmd.qvar1 = quests[int(q)]._qvar1;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdGItem(bool bHiPri, Cmd bCmd, uint8_t mast, uint8_t pnum, Item &item)
{
	TCmdGItem cmd;

	cmd.bCmd = int(bCmd);
	cmd.bPnum = pnum;
	cmd.bMaster = mast;
	cmd.bLevel = lvl.currlevel;
	cmd.bCursitem = ii;
	cmd.dwTime = 0;
	cmd.x = item._i.x;
	cmd.y = item._i.y;
	cmd.wIndx = item.IDidx;

	if (item.IDidx == ItemIndex::EAR) {
		cmd.wCI = item._iName[8] | (item._iName[7] << 8);
		cmd.dwSeed = item._iName[12] | ((item._iName[11] | ((item._iName[10] | (item._iName[9] << 8)) << 8)) << 8);
		cmd.bId = item._iName[13];
		cmd.bDur = item._iName[14];
		cmd.bMDur = item._iName[15];
		cmd.bCh = item._iName[16];
		cmd.bMCh = item._iName[17];
		cmd.wValue = item._ivalue | (item._iName[18] << 8) | ((item._iCurs - 19) << 6);
		cmd.dwBuff = item._iName[22] | ((item._iName[21] | ((item._iName[20] | (item._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = item._iCreateInfo;
		cmd.dwSeed = item._iSeed;
		cmd.bId = item._iIdentified;
		cmd.bDur = item._iDurability;
		cmd.bMDur = item._iMaxDur;
		cmd.bCh = item._iCharges;
		cmd.bMCh = item._iMaxCharges;
		cmd.wValue = item._ivalue;
	}

	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdGItem2(bool usonly, Cmd bCmd, uint8_t mast, uint8_t pnum, TCmdGItem *p)
{
	int ticks;
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.bPnum = pnum;
	cmd.bCmd = int(bCmd);
	cmd.bMaster = mast;

	if (!usonly) {
		cmd.dwTime = 0;
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
		return;
	}

	ticks = SDL_GetTicks();
	if (!cmd.dwTime) {
		cmd.dwTime = ticks;
	} else if (ticks - cmd.dwTime > 5000) {
		return;
	}

	multi_msg_add((uint8_t *)&cmd.bCmd, sizeof(cmd));
}

bool NetSendCmdReq2(Cmd bCmd, uint8_t mast, uint8_t pnum, TCmdGItem *p)
{
	int ticks;
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.bCmd = int(bCmd);
	cmd.bPnum = pnum;
	cmd.bMaster = mast;

	ticks = SDL_GetTicks();
	if (!cmd.dwTime) {
		cmd.dwTime = ticks;
	} else if (ticks - cmd.dwTime > 5000) {
		return false;
	}

	multi_msg_add((uint8_t *)&cmd.bCmd, sizeof(cmd));

	return true;
}

void NetSendCmdExtra(TCmdGItem *p)
{
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.dwTime = 0;
	cmd.bCmd = int(Cmd::ITEMEXTRA);
	NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdPItem(bool bHiPri, Cmd bCmd, uint8_t x, uint8_t y)
{
	TCmdPItem cmd;

	cmd.bCmd = int(bCmd);
	cmd.x = x;
	cmd.y = y;
	cmd.wIndx = int(myplr().heldItem()->IDidx);

	if (myplr().heldItem()->IDidx == ItemIndex::EAR) {
		cmd.wCI = myplr().heldItem()->_iName[8] | (myplr().heldItem()->_iName[7] << 8);
		cmd.dwSeed = myplr().heldItem()->_iName[12] | ((myplr().heldItem()->_iName[11] | ((myplr().heldItem()->_iName[10] | (myplr().heldItem()->_iName[9] << 8)) << 8)) << 8);
		cmd.bId = myplr().heldItem()->_iName[13];
		cmd.bDur = myplr().heldItem()->_iName[14];
		cmd.bMDur = myplr().heldItem()->_iName[15];
		cmd.bCh = myplr().heldItem()->_iName[16];
		cmd.bMCh = myplr().heldItem()->_iName[17];
		cmd.wValue = myplr().heldItem()->_ivalue | (myplr().heldItem()->_iName[18] << 8) | ((myplr().heldItem()->_iCurs - 19) << 6);
		cmd.dwBuff = myplr().heldItem()->_iName[22] | ((myplr().heldItem()->_iName[21] | ((myplr().heldItem()->_iName[20] | (myplr().heldItem()->_iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = myplr().heldItem()->_iCreateInfo;
		cmd.dwSeed = myplr().heldItem()->_iSeed;
		cmd.bId = myplr().heldItem()->_iIdentified;
		cmd.bDur = myplr().heldItem()->_iDurability;
		cmd.bMDur = myplr().heldItem()->_iMaxDur;
		cmd.bCh = myplr().heldItem()->_iCharges;
		cmd.bMCh = myplr().heldItem()->_iMaxCharges;
		cmd.wValue = myplr().heldItem()->_ivalue;
	}

	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdChItem(bool bHiPri, BodyLoc bLoc)
{
	TCmdChItem cmd;

	cmd.bCmd = int(Cmd::CHANGEPLRITEMS);
	cmd.bLoc = int(bLoc);
	cmd.wIndx = int(myplr().heldItem()->IDidx);
	cmd.wCI = myplr().heldItem()->_iCreateInfo;
	cmd.dwSeed = myplr().heldItem()->_iSeed;
	cmd.bId = myplr().heldItem()->_iIdentified;

	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdDelItem(bool bHiPri, BodyLoc bLoc)
{
	TCmdDelItem cmd;

	cmd.bLoc = int(bLoc);
	cmd.bCmd = int(Cmd::DELPLRITEMS);
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdDItem(bool bHiPri, Item &item)
{
	TCmdPItem cmd;

	cmd.bCmd = int(Cmd::DROPITEM);
	cmd.x = item._i.x;
	cmd.y = item._i.y;
	cmd.wIndx = int(item.IDidx);

	if (item.IDidx == ItemIndex::EAR) {
		cmd.wCI = item._iName[8] | (item._iName[7] << 8);
		cmd.dwSeed = item._iName[12] | ((item._iName[11] | ((item._iName[10] | (item._iName[9] << 8)) << 8)) << 8);
		cmd.bId = item._iName[13];
		cmd.bDur = item._iName[14];
		cmd.bMDur = item._iName[15];
		cmd.bCh = item._iName[16];
		cmd.bMCh = item._iName[17];
		cmd.wValue = item._ivalue | (item._iName[18] << 8) | ((item._iCurs - 19) << 6);
		cmd.dwBuff = item._iName[22] | ((item._iName[21] | ((item._iName[20] | (item._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = item._iCreateInfo;
		cmd.dwSeed = item._iSeed;
		cmd.bId = item._iIdentified;
		cmd.bDur = item._iDurability;
		cmd.bMDur = item._iMaxDur;
		cmd.bCh = item._iCharges;
		cmd.bMCh = item._iMaxCharges;
		cmd.wValue = item._ivalue;
	}

	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdDamage(bool bHiPri, uint8_t bPlr, DWORD dwDam)
{
	TCmdDamage cmd;

	cmd.bCmd = int(Cmd::PLRDAMAGE);
	cmd.bPlr = bPlr;
	cmd.dwDam = dwDam;
	if (bHiPri)
		NetSendHiPri((uint8_t *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((uint8_t *)&cmd, sizeof(cmd));
}

void NetSendCmdString(int pmask, const char *pszStr)
{
	int dwStrLen;
	TCmdString cmd;

	dwStrLen = strlen(pszStr);
	cmd.bCmd = int(Cmd::STRING);
	strcpy(cmd.str, pszStr);
	multi_send_msg_packet(pmask, (uint8_t *)&cmd.bCmd, dwStrLen + 2);
}

void delta_close_portal(int pnum)
{
	memset(&sgJunk.portal[pnum], 0xFF, sizeof(sgJunk.portal[pnum]));
	sgbDeltaChanged = true;
}

DWORD ParseCmd(int pnum, TCmd *pCmd)
{
	sbLastCmd = Cmd(pCmd->bCmd);
	if (sgwPackPlrOffsetTbl[pnum] != 0 && sbLastCmd != Cmd::ACK_PLRINFO && sbLastCmd != Cmd::SEND_PLRINFO)
		return 0;

	switch (Cmd(pCmd->bCmd)) {
	case Cmd::SYNCDATA:
		return On_SYNCDATA(pCmd, pnum);
	case Cmd::WALKXY:
		return On_WALKXY(pCmd, pnum);
	case Cmd::ADDSTR:
		return On_ADDSTR(pCmd, pnum);
	case Cmd::ADDDEX:
		return On_ADDDEX(pCmd, pnum);
	case Cmd::ADDMAG:
		return On_ADDMAG(pCmd, pnum);
	case Cmd::ADDVIT:
		return On_ADDVIT(pCmd, pnum);
	case Cmd::SBSPELL:
		return On_SBSPELL(pCmd, pnum);
	case Cmd::GOTOGETITEM:
		return On_GOTOGETITEM(pCmd, pnum);
	case Cmd::REQUESTGITEM:
		return On_REQUESTGITEM(pCmd, pnum);
	case Cmd::GETITEM:
		return On_GETITEM(pCmd, pnum);
	case Cmd::GOTOAGETITEM:
		return On_GOTOAGETITEM(pCmd, pnum);
	case Cmd::REQUESTAGITEM:
		return On_REQUESTAGITEM(pCmd, pnum);
	case Cmd::AGETITEM:
		return On_AGETITEM(pCmd, pnum);
	case Cmd::ITEMEXTRA:
		return On_ITEMEXTRA(pCmd, pnum);
	case Cmd::PUTITEM:
		return On_PUTITEM(pCmd, pnum);
	case Cmd::SYNCPUTITEM:
		return On_SYNCPUTITEM(pCmd, pnum);
	case Cmd::RESPAWNITEM:
		return On_RESPAWNITEM(pCmd, pnum);
	case Cmd::ATTACKXY:
		return On_ATTACKXY(pCmd, pnum);
	case Cmd::SATTACKXY:
		return On_SATTACKXY(pCmd, pnum);
	case Cmd::RATTACKXY:
		return On_RATTACKXY(pCmd, pnum);
	case Cmd::SPELLXYD:
		return On_SPELLXYD(pCmd, pnum);
	case Cmd::SPELLXY:
		return On_SPELLXY(pCmd, pnum);
	case Cmd::TSPELLXY:
		return On_TSPELLXY(pCmd, pnum);
	case Cmd::OPOBJXY:
		return On_OPOBJXY(pCmd, pnum);
	case Cmd::DISARMXY:
		return On_DISARMXY(pCmd, pnum);
	case Cmd::OPOBJT:
		return On_OPOBJT(pCmd, pnum);
	case Cmd::ATTACKID:
		return On_ATTACKID(pCmd, pnum);
	case Cmd::ATTACKPID:
		return On_ATTACKPID(pCmd, pnum);
	case Cmd::RATTACKID:
		return On_RATTACKID(pCmd, pnum);
	case Cmd::RATTACKPID:
		return On_RATTACKPID(pCmd, pnum);
	case Cmd::SPELLID:
		return On_SPELLID(pCmd, pnum);
	case Cmd::SPELLPID:
		return On_SPELLPID(pCmd, pnum);
	case Cmd::TSPELLID:
		return On_TSPELLID(pCmd, pnum);
	case Cmd::TSPELLPID:
		return On_TSPELLPID(pCmd, pnum);
	case Cmd::KNOCKBACK:
		return On_KNOCKBACK(pCmd, pnum);
	case Cmd::RESURRECT:
		return On_RESURRECT(pCmd, pnum);
	case Cmd::HEALOTHER:
		return On_HEALOTHER(pCmd, pnum);
	case Cmd::TALKXY:
		return On_TALKXY(pCmd, pnum);
	case Cmd::DEBUG:
		return On_DEBUG(pCmd, pnum);
	case Cmd::NEWLVL:
		return On_NEWLVL(pCmd, pnum);
	case Cmd::WARP:
		return On_WARP(pCmd, pnum);
	case Cmd::MONSTDEATH:
		return On_MONSTDEATH(pCmd, pnum);
	case Cmd::KILLGOLEM:
		return On_KILLGOLEM(pCmd, pnum);
	case Cmd::AWAKEGOLEM:
		return On_AWAKEGOLEM(pCmd, pnum);
	case Cmd::MONSTDAMAGE:
		return On_MONSTDAMAGE(pCmd, pnum);
	case Cmd::PLRDEAD:
		return On_PLRDEAD(pCmd, pnum);
	case Cmd::PLRDAMAGE:
		return On_PLRDAMAGE(pCmd, pnum);
	case Cmd::OPENDOOR:
		return On_OPENDOOR(pCmd, pnum);
	case Cmd::CLOSEDOOR:
		return On_CLOSEDOOR(pCmd, pnum);
	case Cmd::OPERATEOBJ:
		return On_OPERATEOBJ(pCmd, pnum);
	case Cmd::PLROPOBJ:
		return On_PLROPOBJ(pCmd, pnum);
	case Cmd::BREAKOBJ:
		return On_BREAKOBJ(pCmd, pnum);
	case Cmd::CHANGEPLRITEMS:
		return On_CHANGEPLRITEMS(pCmd, pnum);
	case Cmd::DELPLRITEMS:
		return On_DELPLRITEMS(pCmd, pnum);
	case Cmd::PLRLEVEL:
		return On_PLRLEVEL(pCmd, pnum);
	case Cmd::DROPITEM:
		return On_DROPITEM(pCmd, pnum);
	case Cmd::ACK_PLRINFO:
		return On_ACK_PLRINFO(pCmd, pnum);
	case Cmd::SEND_PLRINFO:
		return On_SEND_PLRINFO(pCmd, pnum);
	case Cmd::PLAYER_JOINLEVEL:
		return On_PLAYER_JOINLEVEL(pCmd, pnum);
	case Cmd::ACTIVATEPORTAL:
		return On_ACTIVATEPORTAL(pCmd, pnum);
	case Cmd::DEACTIVATEPORTAL:
		return On_DEACTIVATEPORTAL(pCmd, pnum);
	case Cmd::RETOWN:
		return On_RETOWN(pCmd, pnum);
	case Cmd::SETSTR:
		return On_SETSTR(pCmd, pnum);
	case Cmd::SETMAG:
		return On_SETMAG(pCmd, pnum);
	case Cmd::SETDEX:
		return On_SETDEX(pCmd, pnum);
	case Cmd::SETVIT:
		return On_SETVIT(pCmd, pnum);
	case Cmd::STRING:
		return On_STRING(pCmd, pnum);
	case Cmd::SYNCQUEST:
		return On_SYNCQUEST(pCmd, pnum);
	case Cmd::ENDSHIELD:
		return On_ENDSHIELD(pCmd, pnum);
	case Cmd::CHEAT_EXPERIENCE:
		return On_CHEAT_EXPERIENCE(pCmd, pnum);
	case Cmd::CHEAT_SPELL_LEVEL:
		return On_CHEAT_SPELL_LEVEL(pCmd, pnum);
	case Cmd::NOVA:
		return On_NOVA(pCmd, pnum);
	case Cmd::SETSHIELD:
		return On_SETSHIELD(pCmd, pnum);
	case Cmd::REMSHIELD:
		return On_REMSHIELD(pCmd, pnum);
	}

	if (pCmd->bCmd < Cmd::DLEVEL_0 || pCmd->bCmd > Cmd::DLEVEL_END) {
		SNetDropPlayer(pnum, 0x40000006);
		return 0;
	}

	return On_DLEVEL(pnum, pCmd);
}

DWORD On_DLEVEL(int pnum, TCmd *pCmd)
{
	TCmdPlrInfoHdr *p = (TCmdPlrInfoHdr *)pCmd;

	if (gbDeltaSender != pnum) {
		if (p->bCmd == Cmd::DLEVEL_END) {
			gbDeltaSender = pnum;
			sgbRecvCmd = Cmd::DLEVEL_END;
		} else if (p->bCmd == Cmd::DLEVEL_0 && p->wOffset == 0) {
			gbDeltaSender = pnum;
			sgbRecvCmd = Cmd::DLEVEL_END;
		} else {
			return p->wBytes + sizeof(*p);
		}
	}
	if (sgbRecvCmd == Cmd::DLEVEL_END) {
		if (p->bCmd == Cmd::DLEVEL_END) {
			sgbDeltaChunks = 20;
			return p->wBytes + sizeof(*p);
		} else if (p->bCmd == Cmd::DLEVEL_0 && p->wOffset == 0) {
			sgdwRecvOffset = 0;
			sgbRecvCmd = p->bCmd;
		} else {
			return p->wBytes + sizeof(*p);
		}
	} else if (sgbRecvCmd != p->bCmd) {
		DeltaImportData(sgbRecvCmd, sgdwRecvOffset);
		if (p->bCmd == Cmd::DLEVEL_END) {
			sgbDeltaChunks = 20;
			sgbRecvCmd = Cmd::DLEVEL_END;
			return p->wBytes + sizeof(*p);
		} else {
			sgdwRecvOffset = 0;
			sgbRecvCmd = p->bCmd;
		}
	}

	/// ASSERT: assert(p->wOffset == sgdwRecvOffset);
	memcpy(&sgRecvBuf[p->wOffset], &p[1], p->wBytes);
	sgdwRecvOffset += p->wBytes;
	return p->wBytes + sizeof(*p);
}

void DeltaImportData(uint8_t cmd, DWORD recv_offset)
{
	uint8_t i;
	uint8_t *src;

	if (sgRecvBuf[0])
		PkwareDecompress(&sgRecvBuf[1], recv_offset, 4721);

	src = &sgRecvBuf[1];
	if (cmd == Cmd::DLEVEL_JUNK) {
		DeltaImportJunk(src);
	} else if (cmd >= Cmd::DLEVEL_0 && cmd <= Cmd::DLEVEL_16) {
		i = cmd - Cmd::DLEVEL_0;
		src = DeltaImportItem(src, sgLevels[i].item);
		src = DeltaImportObject(src, sgLevels[i].object);
		DeltaImportMonster(src, sgLevels[i].monster);
	} else {
		app_fatal("msg:1");
	}

	sgbDeltaChunks++;
	sgbDeltaChanged = true;
}

uint8_t *DeltaImportItem(uint8_t *src, TCmdPItem *dst)
{
	int i;

	for (i = 0; i < MAXITEMS; i++) {
		if (*src == 0xFF) {
			memset(dst, 0xFF, sizeof(TCmdPItem));
			src++;
		} else {
			memcpy(dst, src, sizeof(TCmdPItem));
			src += sizeof(TCmdPItem);
		}
		dst++;
	}

	return src;
}

uint8_t *DeltaImportObject(uint8_t *src, DObjectStr *dst)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return src + sizeof(DObjectStr) * MAXOBJECTS;
}

uint8_t *DeltaImportMonster(uint8_t *src, DMonsterStr *dst)
{
	int i;

	for (i = 0; i < MAXMONSTERS; i++) {
		if (*src == 0xFF) {
			memset(dst, 0xFF, sizeof(DMonsterStr));
			src++;
		} else {
			memcpy(dst, src, sizeof(DMonsterStr));
			src += sizeof(DMonsterStr);
		}
		dst++;
	}

	return src;
}

void DeltaImportJunk(uint8_t *src)
{
	int i;
	MultiQuests *mq;

	for (i = 0; i < MAXPORTAL; i++) {
		if (*src == 0xFF) {
			memset(&sgJunk.portal[i], 0xFF, sizeof(DPortal));
			src++;
			SetPortalStats(i, false, { 0, 0 }, 0, DunType::town);
		} else {
			memcpy(&sgJunk.portal[i], src, sizeof(DPortal));
			src += sizeof(DPortal);
			SetPortalStats(
			    i,
			    true,
			    { sgJunk.portal[i].x, sgJunk.portal[i].y },
			    sgJunk.portal[i].level,
			    sgJunk.portal[i].ltype);
		}
	}

	mq = sgJunk.quests;
	for (i = 0; i < MAXMULTIQUESTS; i++) {
		if (questlist[i]._qflags & 1) {
			memcpy(mq, src, sizeof(MultiQuests));
			src += sizeof(MultiQuests);
			quests[i]._qlog = mq->qlog;
			quests[i]._qactive = mq->qstate;
			quests[i]._qvar1 = mq->qvar1;
			mq++;
		}
	}
}

DWORD On_SYNCDATA(TCmd *pCmd, int pnum)
{
	return sync_update(pnum, (const uint8_t *)pCmd);
}

DWORD On_WALKXY(TCmd *pCmd, int pnum)
{
	TCmdLoc *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].MakePlrPath({ p->x, p->y }, true);
		plr[pnum].data.destAction = DestinationAction::NONE;
	}

	return sizeof(*p);
}

DWORD On_ADDSTR(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		plr[pnum].ModifyPlrStr(p->wParam1);

	return sizeof(*p);
}

DWORD On_ADDMAG(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		plr[pnum].ModifyPlrMag(p->wParam1);

	return sizeof(*p);
}

DWORD On_ADDDEX(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		plr[pnum].ModifyPlrDex(p->wParam1);

	return sizeof(*p);
}

DWORD On_ADDVIT(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		plr[pnum].ModifyPlrVit(p->wParam1);

	return sizeof(*p);
}

DWORD On_SBSPELL(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1) {
		if (lvl.currlevel != 0 || spelldata[p->wParam1].sTownSpell) {
			plr[pnum].data._pSpell = p->wParam1;
			plr[pnum].data._pSplType = plr[pnum].data._pSBkSplType;
			plr[pnum].data._pSplFrom = 1;
			plr[pnum].data.destAction = DestinationAction::SPELL;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

void msg_errorf(const char *pszFmt, ...)
{
	static DWORD msg_err_timer;
	DWORD ticks;
	char msg[256];
	va_list va;

	va_start(va, pszFmt);
	ticks = SDL_GetTicks();
	if (ticks - msg_err_timer >= 5000) {
		msg_err_timer = ticks;
		vsprintf(msg, pszFmt, va);
		ErrorPlrMsg(msg);
	}
	va_end(va);
}

DWORD On_GOTOGETITEM(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].MakePlrPath({ p->x, p->y }, false);
		plr[pnum].data.destAction = DestinationAction::PICKUPITEM;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_REQUESTGITEM(TCmd *pCmd, int pnum)
{
	TCmdGItem *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs != 1 && i_own_level(plr[pnum].data.plrlevel)) {
		if (GetItemRecord(p->dwSeed, p->wCI, p->wIndx)) {
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				NetSendCmdGItem2(false, Cmd::GETITEM, myplr(), p->bPnum, p);
				if (p->bPnum != myplr())
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					myplr().inventory.PickupItem(ii);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(Cmd::REQUESTGITEM, myplr(), p->bPnum, p))
				NetSendCmdExtra(p);
		}
	}

	return sizeof(*p);
}

bool i_own_level(int nReqLevel)
{
	int i;

	for (i = 0; i < MAX_PLRS; i++) {
		if (plr[i].data.plractive
		    && !plr[i].data._pLvlChanging
		    && plr[i].data.plrlevel == nReqLevel
		    && (i != myplr() || !gbBufferMsgs))
			break;
	}

	return i == myplr();
}

DWORD On_GETITEM(TCmd *pCmd, int pnum)
{
	TCmdGItem *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
		if (delta_get_item(p, p->bLevel)) {
			if ((lvl.currlevel == p->bLevel || p->bPnum == myplr()) && p->bMaster != myplr()) {
				if (p->bPnum == myplr()) {
					if (lvl.currlevel != p->bLevel) {			
						ii = myplr().inventory.DropItemSync(myplr().pos(), p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
						if (ii != -1)
							myplr().inventory.PickupItem(ii);
					} else
						myplr().inventory.PickupItem(ii);
				} else
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
			}
		} else
			NetSendCmdGItem2(true, Cmd::GETITEM, p->bMaster, p->bPnum, p);
	}

	return sizeof(*p);
}

bool delta_get_item(TCmdGItem *pI, uint8_t bLevel)
{
	bool result;
	TCmdPItem *pD;
	int i;
	bool found;

	result = true;
	found = false;
	if (game.isMultiplayer()) {
		pD = sgLevels[bLevel].item;
		for (i = 0; i < MAXITEMS; i++, pD++) {
			if (pD->bCmd != 0xFF && pD->wIndx == pI->wIndx && pD->wCI == pI->wCI && pD->dwSeed == pI->dwSeed) {
				found = true;
				break;
			}
		}
		if (found) {
			if (pD->bCmd == Cmd::WALKXY) {
				return result;
			}
			if (pD->bCmd == Cmd::STAND) {
				sgbDeltaChanged = 1;
				pD->bCmd = Cmd::WALKXY;
				return result;
			}
			if (pD->bCmd == Cmd::ACK_PLRINFO) {
				pD->bCmd = 0xFF;
				sgbDeltaChanged = 1;
				return result;
			}
			app_fatal("delta:1");
		}
		if (((pI->wCI >> 8) & 0x80) == 0)
			return false;
		pD = sgLevels[bLevel].item;
		for (i = 0; i < MAXITEMS; i++, pD++) {
			if (pD->bCmd == 0xFF) {
				sgbDeltaChanged = 1;
				pD->bCmd = Cmd::WALKXY;
				pD->x = pI->x;
				pD->y = pI->y;
				pD->wIndx = pI->wIndx;
				pD->wCI = pI->wCI;
				pD->dwSeed = pI->dwSeed;
				pD->bId = pI->bId;
				pD->bDur = pI->bDur;
				pD->bMDur = pI->bMDur;
				pD->bCh = pI->bCh;
				pD->bMCh = pI->bMCh;
				pD->wValue = pI->wValue;
				pD->dwBuff = pI->dwBuff;
				result = true;
				break;
			}
		}
	}
	return result;
}

DWORD On_GOTOAGETITEM(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].MakePlrPath({ p->x, p->y }, false);
		plr[pnum].data.destAction = DestinationAction::PICKUPAITEM;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_REQUESTAGITEM(TCmd *pCmd, int pnum)
{
	TCmdGItem *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs != 1 && i_own_level(plr[pnum].data.plrlevel)) {
		if (GetItemRecord(p->dwSeed, p->wCI, p->wIndx)) {
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				NetSendCmdGItem2(false, Cmd::AGETITEM, myplr(), p->bPnum, p);
				if (p->bPnum != myplr())
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					myplr().inventory.AutoHoldItem(p->bCursitem);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(Cmd::REQUESTAGITEM, myplr(), p->bPnum, p))
				NetSendCmdExtra(p);
		}
	}

	return sizeof(*p);
}

DWORD On_AGETITEM(TCmd *pCmd, int pnum)
{
	TCmdGItem *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		FindGetItem(p->wIndx, p->wCI, p->dwSeed);
		if (delta_get_item(p, p->bLevel)) {
			if ((lvl.currlevel == p->bLevel || p->bPnum == myplr()) && p->bMaster != myplr()) {
				if (p->bPnum == myplr()) {
					if (lvl.currlevel != p->bLevel) {
						int ii = myplr().inventory.DropItemSync(myplr().pos(), p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
						if (ii != -1)
							myplr().inventory.AutoHoldItem(ii);
					} else
						myplr().inventory.AutoHoldItem(p->bCursitem);
				} else
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
			}
		} else
			NetSendCmdGItem2(true, Cmd::AGETITEM, p->bMaster, p->bPnum, p);
	}

	return sizeof(*p);
}

DWORD On_ITEMEXTRA(TCmd *pCmd, int pnum)
{
	TCmdGItem *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		delta_get_item(p, p->bLevel);
		if (lvl.currlevel == plr[pnum].data.plrlevel)
			SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
	}

	return sizeof(*p);
}

DWORD On_PUTITEM(TCmd *pCmd, int pnum)
{
	TCmdPItem *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (lvl.currlevel == plr[pnum].data.plrlevel) {
		int ii;
		if (pnum == myplr())
			ii = myplr().inventory.DropItem({ p->x, p->y });
		else
			ii = myplr().inventory.DropItemSync({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
		if (ii != -1) {
			PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
			delta_put_item(p, item[ii]._i, plr[pnum].data.plrlevel);
			check_update_plr(pnum);
		}
		return sizeof(*p);
	} else {
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		delta_put_item(p, { p->x, p->y }, plr[pnum].data.plrlevel);
		check_update_plr(pnum);
	}

	return sizeof(*p);
}

void delta_put_item(TCmdPItem *pI, V2Di pos, uint8_t bLevel)
{
	int i;
	TCmdPItem *pD;

	if (game.isSingleplayer()) {
		return;
	}
	pD = sgLevels[bLevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != Cmd::WALKXY
		    && pD->bCmd != 0xFF
		    && pD->wIndx == pI->wIndx
		    && pD->wCI == pI->wCI
		    && pD->dwSeed == pI->dwSeed) {
			if (pD->bCmd == Cmd::ACK_PLRINFO)
				return;
			app_fatal("Trying to drop a floor item?");
		}
	}

	pD = sgLevels[bLevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == 0xFF) {
			sgbDeltaChanged = true;
			memcpy(pD, pI, sizeof(TCmdPItem));
			pD->bCmd = Cmd::ACK_PLRINFO;
			pD->x = pos.x;
			pD->y = pos.y;
			return;
		}
	}
}

void check_update_plr(int pnum)
{
	if (game.isMultiplayer() && pnum == myplr())
		pfile_update(true);
}

DWORD On_SYNCPUTITEM(TCmd *pCmd, int pnum)
{
	TCmdPItem *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (lvl.currlevel == plr[pnum].data.plrlevel) {
		int ii = myplr().inventory.DropItemSync({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
		if (ii != -1) {
			PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
			delta_put_item(p, item[ii]._i, plr[pnum].data.plrlevel);
			check_update_plr(pnum);
		}
		return sizeof(*p);
	} else {
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		delta_put_item(p, { p->x, p->y }, plr[pnum].data.plrlevel);
		check_update_plr(pnum);
	}

	return sizeof(*p);
}

DWORD On_RESPAWNITEM(TCmd *pCmd, int pnum)
{
	TCmdPItem *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (lvl.currlevel == plr[pnum].data.plrlevel && pnum != myplr()) {
			myplr().inventory.DropItemSync({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
		}
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		delta_put_item(p, { p->x, p->y }, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

DWORD On_ATTACKXY(TCmd *pCmd, int pnum)
{
	TCmdLoc *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].MakePlrPath({ p->x, p->y }, false);
		plr[pnum].data.destAction = DestinationAction::ATTACK;
		plr[pnum].data.destParam1 = p->x;
		plr[pnum].data.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD On_SATTACKXY(TCmd *pCmd, int pnum)
{
	TCmdLoc *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].data.destAction = DestinationAction::ATTACK;
		plr[pnum].data.destParam1 = p->x;
		plr[pnum].data.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD On_RATTACKXY(TCmd *pCmd, int pnum)
{
	TCmdLoc *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].data.destAction = DestinationAction::RATTACK;
		plr[pnum].data.destParam1 = p->x;
		plr[pnum].data.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD On_SPELLXYD(TCmd *pCmd, int pnum)
{
	TCmdLocParam3 *p = (TCmdLocParam3 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (lvl.currlevel != 0 || spelldata[p->wParam1].sTownSpell) {
			plr[pnum].ClrPlrPath();
			plr[pnum].data.destAction = DestinationAction::SPELLWALL;
			plr[pnum].data.destParam1 = p->x;
			plr[pnum].data.destParam2 = p->y;
			plr[pnum].data.destParam3 = p->wParam2;
			plr[pnum].data.destParam4 = p->wParam3;
			plr[pnum].data._pSpell = p->wParam1;
			plr[pnum].data._pSplType = plr[pnum].data._pRSplType;
			plr[pnum].data._pSplFrom = 0;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

DWORD On_SPELLXY(TCmd *pCmd, int pnum)
{
	TCmdLocParam2 *p = (TCmdLocParam2 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (lvl.currlevel != 0 || spelldata[p->wParam1].sTownSpell) {
			plr[pnum].ClrPlrPath();
			plr[pnum].data.destAction = DestinationAction::SPELL;
			plr[pnum].data.destParam1 = p->x;
			plr[pnum].data.destParam2 = p->y;
			plr[pnum].data.destParam3 = p->wParam2;
			plr[pnum].data._pSpell = p->wParam1;
			plr[pnum].data._pSplType = plr[pnum].data._pRSplType;
			plr[pnum].data._pSplFrom = 0;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

DWORD On_TSPELLXY(TCmd *pCmd, int pnum)
{
	TCmdLocParam2 *p = (TCmdLocParam2 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (lvl.currlevel != 0 || spelldata[p->wParam1].sTownSpell) {
			plr[pnum].ClrPlrPath();
			plr[pnum].data.destAction = DestinationAction::SPELL;
			plr[pnum].data.destParam1 = p->x;
			plr[pnum].data.destParam2 = p->y;
			plr[pnum].data.destParam3 = p->wParam2;
			plr[pnum].data._pSpell = p->wParam1;
			plr[pnum].data._pSplType = plr[pnum].data._pTSplType;
			plr[pnum].data._pSplFrom = 2;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

DWORD On_OPOBJXY(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (object[p->wParam1]._oSolidFlag || object[p->wParam1]._oDoorFlag)
			plr[pnum].MakePlrPath({ p->x, p->y }, false);
		else
			plr[pnum].MakePlrPath({ p->x, p->y }, true);
		plr[pnum].data.destAction = DestinationAction::OPERATE;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_DISARMXY(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (object[p->wParam1]._oSolidFlag || object[p->wParam1]._oDoorFlag)
			plr[pnum].MakePlrPath({ p->x, p->y }, false);
		else
			plr[pnum].MakePlrPath({ p->x, p->y }, true);
		plr[pnum].data.destAction = DestinationAction::DISARM;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_OPOBJT(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].data.destAction = DestinationAction::OPERATETK;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_ATTACKID(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		int distx = abs(plr[pnum].pos().x - monsters[p->wParam1].data._mfut.x);
		int disty = abs(plr[pnum].pos().y - monsters[p->wParam1].data._mfut.y);
		if (distx > 1 || disty > 1)
			plr[pnum].MakePlrPath({ monsters[p->wParam1].data._mfut.x, monsters[p->wParam1].data._mfut.y }, false);
		plr[pnum].data.destAction = DestinationAction::ATTACKMON;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_ATTACKPID(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].MakePlrPath(plr[p->wParam1].futpos(), false);
		plr[pnum].data.destAction = DestinationAction::ATTACKPLR;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_RATTACKID(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].data.destAction = DestinationAction::RATTACKMON;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_RATTACKPID(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].data.destAction = DestinationAction::RATTACKPLR;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_SPELLID(TCmd *pCmd, int pnum)
{
	TCmdParam3 *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (lvl.currlevel != 0 || spelldata[p->wParam2].sTownSpell) {
			plr[pnum].ClrPlrPath();
			plr[pnum].data.destAction = DestinationAction::SPELLMON;
			plr[pnum].data.destParam1 = p->wParam1;
			plr[pnum].data.destParam2 = p->wParam3;
			plr[pnum].data._pSpell = p->wParam2;
			plr[pnum].data._pSplType = plr[pnum].data._pRSplType;
			plr[pnum].data._pSplFrom = 0;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

DWORD On_SPELLPID(TCmd *pCmd, int pnum)
{
	TCmdParam3 *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (lvl.currlevel != 0 || spelldata[p->wParam2].sTownSpell) {
			plr[pnum].ClrPlrPath();
			plr[pnum].data.destAction = DestinationAction::SPELLPLR;
			plr[pnum].data.destParam1 = p->wParam1;
			plr[pnum].data.destParam2 = p->wParam3;
			plr[pnum].data._pSpell = p->wParam2;
			plr[pnum].data._pSplType = plr[pnum].data._pRSplType;
			plr[pnum].data._pSplFrom = 0;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

DWORD On_TSPELLID(TCmd *pCmd, int pnum)
{
	TCmdParam3 *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (lvl.currlevel != 0 || spelldata[p->wParam2].sTownSpell) {
			plr[pnum].ClrPlrPath();
			plr[pnum].data.destAction = DestinationAction::SPELLMON;
			plr[pnum].data.destParam1 = p->wParam1;
			plr[pnum].data.destParam2 = p->wParam3;
			plr[pnum].data._pSpell = p->wParam2;
			plr[pnum].data._pSplType = plr[pnum].data._pTSplType;
			plr[pnum].data._pSplFrom = 2;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

DWORD On_TSPELLPID(TCmd *pCmd, int pnum)
{
	TCmdParam3 *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (lvl.currlevel != 0 || spelldata[p->wParam2].sTownSpell) {
			plr[pnum].ClrPlrPath();
			plr[pnum].data.destAction = DestinationAction::SPELLPLR;
			plr[pnum].data.destParam1 = p->wParam1;
			plr[pnum].data.destParam2 = p->wParam3;
			plr[pnum].data._pSpell = p->wParam2;
			plr[pnum].data._pSplType = plr[pnum].data._pTSplType;
			plr[pnum].data._pSplFrom = 2;
		} else
			msg_errorf("%s has cast an illegal spell.", plr[pnum].data._pName);
	}

	return sizeof(*p);
}

DWORD On_KNOCKBACK(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		monsters[p->wParam1].M_GetKnockback();
		monsters[p->wParam1].startHitByPlayer(pnum, 0);
	}

	return sizeof(*p);
}

DWORD On_RESURRECT(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		DoResurrect(pnum, p->wParam1);
		check_update_plr(pnum);
	}

	return sizeof(*p);
}

DWORD On_HEALOTHER(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel)
		DoHealOther(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD On_TALKXY(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].MakePlrPath({ p->x, p->y }, false);
		plr[pnum].data.destAction = DestinationAction::TALK;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_NEWLVL(TCmd *pCmd, int pnum)
{
	TCmdParam2 *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr())
		plr[pnum].StartNewLvl(p->wParam1, p->wParam2);

	return sizeof(*p);
}

DWORD On_WARP(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		plr[pnum].StartWarpLvl(p->wParam1);
		if (pnum == myplr() && pcurs >= Cursor::FIRSTITEM) {
			item[MAXITEMS] = myplr().data.HoldItem;
			myplr().inventory.AutoHoldItem(MAXITEMS);
		}
	}

	return sizeof(*p);
}

DWORD On_MONSTDEATH(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr()) {
		if (lvl.currlevel == plr[pnum].data.plrlevel)
			monsters[p->wParam1].M_SyncStartKill({ p->x, p->y }, pnum);
		delta_kill_monster(p->wParam1, { p->x, p->y }, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

DWORD On_KILLGOLEM(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr()) {
		if (lvl.currlevel == p->wParam1)
			monsters[pnum].M_SyncStartKill({ p->x, p->y }, pnum); // Incorrect???? monsters[pnum] ?
		delta_kill_monster(pnum, { p->x, p->y }, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

DWORD On_AWAKEGOLEM(TCmd *pCmd, int pnum)
{
	TCmdGolem *p = (TCmdGolem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (lvl.currlevel != plr[pnum].data.plrlevel)
		delta_sync_golem(p, pnum, p->currlevel);
	else if (pnum != myplr()) {
		int i;
		// check if this player already has an active golem
		bool addGolem = true;
		for (i = 0; i < nummissiles; i++) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MissileType::GOLEM && missile[mi]._misource == pnum) {
				addGolem = false;
				// BUGFIX: break, don't need to check the rest
			}
		}
		if (addGolem)
			AddMissile(plr[pnum].pos(), { p->_mx, p->_my }, Dir(p->_mdir), MissileType::GOLEM, 0, pnum, 0, 1);
	}

	return sizeof(*p);
}

DWORD On_MONSTDAMAGE(TCmd *pCmd, int pnum)
{
	TCmdParam2 *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr()) {
		if (lvl.currlevel == plr[pnum].data.plrlevel) {
			monsters[p->wParam1].data.mWhoHit |= 1 << pnum;

			if (monsters[p->wParam1].data._mhitpoints) {
				monsters[p->wParam1].data._mhitpoints -= p->wParam2;
				if ((monsters[p->wParam1].data._mhitpoints >> 6) < 1)
					monsters[p->wParam1].data._mhitpoints = 1 << 6;
				delta_monster_hp(p->wParam1, monsters[p->wParam1].data._mhitpoints, plr[pnum].data.plrlevel);
			}
		}
	}

	return sizeof(*p);
}

DWORD On_PLRDEAD(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr())
		plr[pnum].StartPlayerKill(p->wParam1);
	else
		check_update_plr(pnum);

	return sizeof(*p);
}

DWORD On_PLRDAMAGE(TCmd *pCmd, int pnum)
{
	TCmdDamage *p = (TCmdDamage *)pCmd;

	if (p->bPlr == myplr() && lvl.currlevel != 0) {
		if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel && p->dwDam <= 192000) {
			if ((myplr().data._pHitPoints >> 6) > 0) {
				redrawhpflag = true;
				myplr().data._pHitPoints -= p->dwDam;
				myplr().data._pHPBase -= p->dwDam;
				if (myplr().data._pHitPoints > myplr().data._pMaxHP) {
					myplr().data._pHitPoints = myplr().data._pMaxHP;
					myplr().data._pHPBase = myplr().data._pMaxHPBase;
				}
				if ((myplr().data._pHitPoints >> 6) <= 0)
					plr[myplr()].SyncPlrKill(1);
			}
		}
	}

	return sizeof(*p);
}

DWORD On_OPENDOOR(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (lvl.currlevel == plr[pnum].data.plrlevel)
			SyncOpObject(pnum, Cmd::OPENDOOR, p->wParam1);
		delta_sync_object(p->wParam1, Cmd::OPENDOOR, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

void delta_sync_object(int oi, Cmd bCmd, uint8_t bLevel)
{
	if (game.isMultiplayer()) {
		sgbDeltaChanged = true;
		sgLevels[bLevel].object[oi].bCmd = bCmd;
	}
}

DWORD On_CLOSEDOOR(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (lvl.currlevel == plr[pnum].data.plrlevel)
			SyncOpObject(pnum, Cmd::CLOSEDOOR, p->wParam1);
		delta_sync_object(p->wParam1, Cmd::CLOSEDOOR, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

DWORD On_OPERATEOBJ(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (lvl.currlevel == plr[pnum].data.plrlevel)
			SyncOpObject(pnum, Cmd::OPERATEOBJ, p->wParam1);
		delta_sync_object(p->wParam1, Cmd::OPERATEOBJ, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

DWORD On_PLROPOBJ(TCmd *pCmd, int pnum)
{
	TCmdParam2 *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (lvl.currlevel == plr[pnum].data.plrlevel)
			SyncOpObject(p->wParam1, Cmd::PLROPOBJ, p->wParam2);
		delta_sync_object(p->wParam2, Cmd::PLROPOBJ, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

DWORD On_BREAKOBJ(TCmd *pCmd, int pnum)
{
	TCmdParam2 *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (lvl.currlevel == plr[pnum].data.plrlevel)
			SyncBreakObj(p->wParam1, p->wParam2);
		delta_sync_object(p->wParam2, Cmd::BREAKOBJ, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

DWORD On_CHANGEPLRITEMS(TCmd *pCmd, int pnum)
{
	TCmdChItem *p = (TCmdChItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr())
		plr[pnum].inventory.CheckInvSwap(p->bLoc, p->wIndx, p->wCI, p->dwSeed, p->bId);

	return sizeof(*p);
}

DWORD On_DELPLRITEMS(TCmd *pCmd, int pnum)
{
	TCmdDelItem *p = (TCmdDelItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr())
		plr[pnum].inventory.inv_update_rem_item(p->bLoc);

	return sizeof(*p);
}

DWORD On_PLRLEVEL(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= MAXCHARLEVEL && pnum != myplr())
		plr[pnum].data._pLevel = p->wParam1;

	return sizeof(*p);
}

DWORD On_DROPITEM(TCmd *pCmd, int pnum)
{
	TCmdPItem *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else
		delta_put_item(p, { p->x, p->y }, plr[pnum].data.plrlevel);

	return sizeof(*p);
}

DWORD On_SEND_PLRINFO(TCmd *pCmd, int pnum)
{
	TCmdPlrInfoHdr *p = (TCmdPlrInfoHdr *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, p->wBytes + sizeof(*p));
	else
		recv_plrinfo(pnum, p, p->bCmd == Cmd::ACK_PLRINFO);

	return p->wBytes + sizeof(*p);
}

DWORD On_ACK_PLRINFO(TCmd *pCmd, int pnum)
{
	return On_SEND_PLRINFO(pCmd, pnum);
}

DWORD On_PLAYER_JOINLEVEL(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		plr[pnum].data._pLvlChanging = false;
		if (plr[pnum].data._pName[0] && !plr[pnum].data.plractive) {
			plr[pnum].data.plractive = true;
			gbActivePlayers++;
			EventPlrMsg("Player '%s' (level %d) just joined the game", plr[pnum].data._pName, plr[pnum].data._pLevel);
		}

		if (plr[pnum].data.plractive && myplr() != pnum) {
			grid.at({ p->x, p->y }).setPlayer(pnum);
			plr[pnum].data.plrlevel = p->wParam1;
			plr[pnum].data._pGFXLoad = 0;
			if (lvl.currlevel == plr[pnum].data.plrlevel) {
				plr[pnum].LoadPlrGFX(PlayerGraphicFileFlag::STAND);
				plr[pnum].SyncInitPlr();
				if ((plr[pnum].data._pHitPoints >> 6) > 0)
					plr[pnum].StartStand(Dir(0));
				else {
					plr[pnum].data._pgfxnum = 0;
					plr[pnum].LoadPlrGFX(PlayerGraphicFileFlag::DEATH);
					plr[pnum].data._pmode = PlayerMode::DEATH;
					plr[pnum].NewPlrAnim(plr[pnum].data._pDAnim[0], plr[pnum].data._pDFrames, 1, plr[pnum].data._pDWidth);
					plr[pnum].data._pAnimFrame = plr[pnum].data._pAnimLen - 1;
					plr[pnum].data._pVar8 = plr[pnum].data._pAnimLen << 1;
					grid.at(plr[pnum].pos()).dFlags |= DunTileFlag::DEAD_PLAYER;
				}

				plr[pnum].data._pvid = AddVision(plr[pnum].pos(), plr[pnum].data._pLightRad, pnum == myplr());
				plr[pnum].data._plid = -1;
			}
		}
	}

	return sizeof(*p);
}

DWORD On_ACTIVATEPORTAL(TCmd *pCmd, int pnum)
{
	TCmdLocParam3 *p = (TCmdLocParam3 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		ActivatePortal(pnum, { p->x, p->y }, p->wParam1, DunType(p->wParam2), p->wParam3);
		if (pnum != myplr()) {
			if (lvl.currlevel == 0)
				AddInTownPortal(pnum);
			else if (lvl.currlevel == plr[pnum].data.plrlevel) {
				int i;
				bool addPortal = true;
				for (i = 0; i < nummissiles; i++) {
					int mi = missileactive[i];
					if (missile[mi]._mitype == MissileType::TOWN && missile[mi]._misource == pnum) {
						addPortal = false;
						// BUGFIX: break
					}
				}
				if (addPortal)
					AddWarpMissile(pnum, { p->x, p->y });
			} else
				RemovePortalMissile(pnum);
		}
		delta_open_portal(pnum, { p->x, p->y }, p->wParam1, DunType(p->wParam2), p->wParam3);
	}

	return sizeof(*p);
}

void delta_open_portal(int pnum, V2Di pos, uint8_t bLevel, DunType bLType, uint8_t bSetLvl)
{
	sgbDeltaChanged = true;
	sgJunk.portal[pnum].x = pos.x;
	sgJunk.portal[pnum].y = pos.y;
	sgJunk.portal[pnum].level = bLevel;
	sgJunk.portal[pnum].ltype = bLType;
	sgJunk.portal[pnum].setlvl = bSetLvl;
}

DWORD On_DEACTIVATEPORTAL(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else {
		if (PortalOnLevel(pnum))
			RemovePortalMissile(pnum);
		DeactivatePortal(pnum);
		delta_close_portal(pnum);
	}

	return sizeof(*pCmd);
}

DWORD On_RETOWN(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else {
		if (pnum == myplr()) {
			deathflag = false;
			gamemenu_off();
		}
		plr[pnum].RestartTownLvl();
	}

	return sizeof(*pCmd);
}

DWORD On_SETSTR(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr())
		plr[pnum].SetPlrStr(p->wParam1);

	return sizeof(*p);
}

DWORD On_SETDEX(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr())
		plr[pnum].SetPlrDex(p->wParam1);

	return sizeof(*p);
}

DWORD On_SETMAG(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr())
		plr[pnum].SetPlrMag(p->wParam1);

	return sizeof(*p);
}

DWORD On_SETVIT(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr())
		plr[pnum].SetPlrVit(p->wParam1);

	return sizeof(*p);
}

DWORD On_STRING(TCmd *pCmd, int pnum)
{
	return On_STRING2(pnum, pCmd);
}

DWORD On_STRING2(int pnum, TCmd *pCmd)
{
	TCmdString *p = (TCmdString *)pCmd;

	int len = strlen(p->str);
	if (!gbBufferMsgs)
		SendPlrMsg(pnum, p->str);

	return len + 2; // length of string + nul terminator + sizeof(p->bCmd)
}

DWORD On_SYNCQUEST(TCmd *pCmd, int pnum)
{
	TCmdQuest *p = (TCmdQuest *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (pnum != myplr())
			SetMultiQuest(p->q, p->qstate, p->qlog, p->qvar1);
		sgbDeltaChanged = true;
	}

	return sizeof(*p);
}

DWORD On_ENDSHIELD(TCmd *pCmd, int pnum)
{
	int i;

	if (gbBufferMsgs != 1 && pnum != myplr() && lvl.currlevel == plr[pnum].data.plrlevel) {
		for (i = 0; i < nummissiles; i++) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MissileType::MANASHIELD && missile[mi]._misource == pnum) {
				ClearMissileSpot(mi);
				DeleteMissile(mi, i);
			}
		}
	}

	return sizeof(*pCmd);
}

DWORD On_CHEAT_EXPERIENCE(TCmd *pCmd, int pnum)
{
#ifdef _DEBUG
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else if (plr[pnum].data._pLevel < MAXCHARLEVEL - 1) {
		plr[pnum].data._pExperience = plr[pnum].data._pNextExper;
		plr[pnum].NextPlrLevel();
	}
#endif
	return sizeof(*pCmd);
}

DWORD On_CHEAT_SPELL_LEVEL(TCmd *pCmd, int pnum)
{
#ifdef _DEBUG
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else
		plr[pnum].data._pSplLvl[plr[pnum].data._pRSpell]++;
#endif
	return sizeof(*pCmd);
}

DWORD On_DEBUG(TCmd *pCmd, int pnum)
{
	return sizeof(*pCmd);
}

DWORD On_NOVA(TCmd *pCmd, int pnum)
{
	TCmdLoc *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel && pnum != myplr()) {
		plr[pnum].ClrPlrPath();
		plr[pnum].data._pSpell = SpellId::NOVA;
		plr[pnum].data._pSplType = RSpellType::INVALID;
		plr[pnum].data._pSplFrom = 3;
		plr[pnum].data.destAction = DestinationAction::SPELL;
		plr[pnum].data.destParam1 = p->x;
		plr[pnum].data.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD On_SETSHIELD(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1)
		plr[pnum].data.pManaShield = true;

	return sizeof(*pCmd);
}

DWORD On_REMSHIELD(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1)
		plr[pnum].data.pManaShield = false;

	return sizeof(*pCmd);
}

}
