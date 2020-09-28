/**
 * @file msg.cpp
 *
 * Implementation of function for sending and reciving network messages.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"

DEVILUTION_BEGIN_NAMESPACE

static DWORD sgdwOwnerWait;
static DWORD sgdwRecvOffset;
static int sgnCurrMegaPlayer;
static DLevel sgLevels[NUMLEVELS];
static BYTE sbLastCmd;
static TMegaPkt *sgpCurrPkt;
static BYTE sgRecvBuf[sizeof(DLevel) + 1];
static BYTE sgbRecvCmd;
static LocalLevel sgLocals[NUMLEVELS];
static DJunk sgJunk;
static TMegaPkt *sgpMegaPkt;
static BOOLEAN sgbDeltaChanged;
static BYTE sgbDeltaChunks;
bool deltaload;
BYTE gbBufferMsgs;
int dwRecCount;

void msg_send_drop_pkt(int pnum, int reason)
{
	TFakeDropPlr cmd;

	cmd.dwReason = reason;
	cmd.bCmd = FAKE_CMD_DROPID;
	cmd.bPlr = pnum;
	msg_send_packet(pnum, &cmd, sizeof(cmd));
}

void msg_send_packet(int pnum, const void *packet, DWORD dwSize)
{
	TMegaPkt *packeta;
	TFakeCmdPlr cmd;

	if (pnum != sgnCurrMegaPlayer) {
		sgnCurrMegaPlayer = pnum;
		cmd.bCmd = FAKE_CMD_SETID;
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
	sgbRecvCmd = CMD_DLEVEL_END;
	gbBufferMsgs = 1;
	sgdwOwnerWait = SDL_GetTicks();
	success = UiProgressDialog("Waiting for game data...", 1, msg_wait_for_turns, 20);
	gbBufferMsgs = 0;
	if (!success) {
		msg_free_packets();
		return FALSE;
	}

	if (gbGameDestroyed) {
		DrawDlg("The game ended");
		msg_free_packets();
		return FALSE;
	}

	if (sgbDeltaChunks != 21) {
		DrawDlg("Unable to get level data");
		msg_free_packets();
		return FALSE;
	}

	return TRUE;
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
	if (nthread_has_500ms_passed(FALSE))
		nthread_recv_turns(&received);

	if (gbGameDestroyed)
		return 100;
	if (gbDeltaSender >= MAX_PLRS) {
		sgbDeltaChunks = 0;
		sgbRecvCmd = CMD_DLEVEL_END;
		gbDeltaSender = myplr();
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
	if (plr.isMultiplayer()) {
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

	pkt = sgpMegaPkt;
	for (i = -1; pkt; pkt = pkt->pNext) {
		spaceLeft = 32000;
		cmd = (TFakeCmdPlr *)pkt->data;
		while (spaceLeft != pkt->dwSpaceLeft) {
			if (cmd->bCmd == FAKE_CMD_SETID) {
				tmpCmd = cmd;
				cmd++;
				i = tmpCmd->bPlr;
				spaceLeft -= sizeof(*cmd);
			} else if (cmd->bCmd == FAKE_CMD_DROPID) {
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
	BYTE *dst, *dstEnd;
	int size, i;
	char src;

	if (sgbDeltaChanged) {
		dst = (BYTE *)DiabloAllocPtr(4722);
		for (i = 0; i < NUMLEVELS; i++) {
			dstEnd = dst + 1;
			dstEnd = DeltaExportItem(dstEnd, sgLevels[i].item);
			dstEnd = DeltaExportObject(dstEnd, sgLevels[i].object);
			dstEnd = DeltaExportMonster(dstEnd, sgLevels[i].monster);
			size = msg_comp_level(dst, dstEnd);
			dthread_send_delta(pnum, i + CMD_DLEVEL_0, dst, size);
		}
		dstEnd = dst + 1;
		dstEnd = DeltaExportJunk(dstEnd);
		size = msg_comp_level(dst, dstEnd);
		dthread_send_delta(pnum, CMD_DLEVEL_JUNK, dst, size);
		mem_free_dbg(dst);
	}
	src = 0;
	dthread_send_delta(pnum, CMD_DLEVEL_END, &src, 1);
}

BYTE *DeltaExportItem(BYTE *dst, TCmdPItem *src)
{
	int i;

	for (i = 0; i < MAXITEMS; i++) {
		if (src->bCmd == 0xFF) {
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

BYTE *DeltaExportObject(BYTE *dst, DObjectStr *src)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return dst + sizeof(DObjectStr) * MAXOBJECTS;
}

BYTE *DeltaExportMonster(BYTE *dst, DMonsterStr *src)
{
	int i;

	for (i = 0; i < MAXMONSTERS; i++) {
		if (*(BYTE *)src == 0xFF) {
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

BYTE *DeltaExportJunk(BYTE *dst)
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

int msg_comp_level(BYTE *buffer, BYTE *end)
{
	int size, pkSize;

	size = end - buffer - 1;
	pkSize = PkwareCompress(buffer + 1, size);
	*buffer = size != pkSize;

	return pkSize + 1;
}

void delta_init()
{
	sgbDeltaChanged = FALSE;
	memset(&sgJunk, 0xFF, sizeof(sgJunk));
	memset(sgLevels, 0xFF, sizeof(sgLevels));
	memset(sgLocals, 0, sizeof(sgLocals));
	deltaload = FALSE;
}

void delta_kill_monster(int mi, V2Di pos, BYTE bLevel)
{
	DMonsterStr *pD;

	if (plr.isMultiplayer()) {
		sgbDeltaChanged = TRUE;
		pD = &sgLevels[bLevel].monster[mi];
		pD->_mx = pos.x;
		pD->_my = pos.y;
		pD->_mdir = int(monsters[mi].data._mdir);
		pD->_mhitpoints = 0;
	}
}

void delta_monster_hp(int mi, int hp, BYTE bLevel)
{
	DMonsterStr *pD;

	if (plr.isMultiplayer()) {
		sgbDeltaChanged = TRUE;
		pD = &sgLevels[bLevel].monster[mi];
		if (pD->_mhitpoints > hp)
			pD->_mhitpoints = hp;
	}
}

void delta_sync_monster(const TSyncMonster *pSync, BYTE bLevel)
{
	DMonsterStr *pD;

	if (plr.isSingleplayer()) {
		return;
	}

	/// ASSERT: assert(pSync != NULL);
	/// ASSERT: assert(bLevel < NUMLEVELS);
	sgbDeltaChanged = TRUE;

	pD = &sgLevels[bLevel].monster[pSync->_mndx];
	if (pD->_mhitpoints != 0) {
		pD->_mx = pSync->_mx;
		pD->_my = pSync->_my;
		pD->_mactive = UCHAR_MAX;
		pD->_menemy = pSync->_menemy;
	}
}

void delta_sync_golem(TCmdGolem *pG, int pnum, BYTE bLevel)
{
	DMonsterStr *pD;

	if (plr.isMultiplayer()) {
		sgbDeltaChanged = TRUE;
		pD = &sgLevels[bLevel].monster[pnum];
		pD->_mx = pG->_mx;
		pD->_my = pG->_my;
		pD->_mactive = UCHAR_MAX;
		pD->_menemy = pG->_menemy;
		pD->_mdir = pG->_mdir;
		pD->_mhitpoints = pG->_mhitpoints;
	}
}

void delta_leave_sync(BYTE bLevel)
{
	int i, ma;
	DMonsterStr *pD;

	if (plr.isMultiplayer()) {
		if (lvl.currlevel == 0) {
			glSeedTbl[0] = GetRndSeed();
		}
		if (lvl.currlevel > 0) {
			for (i = 0; i < nummonsters; ++i) {
				ma = monstactive[i];
				if (monsters[ma].data._mhitpoints) {
					sgbDeltaChanged = TRUE;
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

void DeltaAddItem(int ii)
{
	int i;
	TCmdPItem *pD;

	if (plr.isSingleplayer()) {
		return;
	}
	pD = sgLevels[lvl.currlevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != 0xFF
		    && pD->wIndx == item[ii].IDidx
		    && pD->wCI == item[ii]._iCreateInfo
		    && pD->dwSeed == item[ii]._iSeed
		    && (pD->bCmd == CMD_WALKXY || pD->bCmd == CMD_STAND)) {
			return;
		}
	}

	pD = sgLevels[lvl.currlevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == 0xFF) {
			pD->bCmd = CMD_STAND;
			sgbDeltaChanged = TRUE;
			pD->x = item[ii]._i.x;
			pD->y = item[ii]._i.y;
			pD->wIndx = item[ii].IDidx;
			pD->wCI = item[ii]._iCreateInfo;
			pD->dwSeed = item[ii]._iSeed;
			pD->bId = item[ii]._iIdentified;
			pD->bDur = item[ii]._iDurability;
			pD->bMDur = item[ii]._iMaxDur;
			pD->bCh = item[ii]._iCharges;
			pD->bMCh = item[ii]._iMaxCharges;
			pD->wValue = item[ii]._ivalue;
			return;
		}
	}
}

void DeltaSaveLevel()
{
	int i;

	if (plr.isMultiplayer()) {
		for (i = 0; i < MAX_PLRS; i++) {
			if (i != myplr())
				plr[i].data._pGFXLoad = 0;
		}
		myplr().data._pLvlVisited[lvl.currlevel] = TRUE;
		delta_leave_sync(lvl.currlevel);
	}
}

void DeltaLoadLevel()
{
	int ii, ot;
	int i, j, k, l;
	V2Di pos, posb;
	bool done;

	if (plr.isSingleplayer()) {
		return;
	}

	deltaload = TRUE;
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
					if (monsters[i].data._mAi != AI_DIABLO) {
						if (!monsters[i].data._uniqtype)
							/// ASSERT: assert(monsters[i].data.MType != NULL);
							AddDead(monsters[i].data._m, monsters[i].data.MType->mdeadval, monsters[i].data._mdir);
						else
							AddDead(monsters[i].data._m, monsters[i].data._udeadval, monsters[i].data._mdir);
					}
					monsters[i].data._mDelFlag = TRUE;
					monsters[i].M_UpdateLeader();
				} else {
					decode_enemy(i, sgLevels[lvl.currlevel].monster[i]._menemy);
					if (monsters[i].data._m.x && monsters[i].data._m.x != 1 || monsters[i].data._m.y)
						grid[monsters[i].data._m.x][monsters[i].data._m.y].setMonster(i);
					if (i < MAX_PLRS) {
						monsters[i].MAI_Golum();
						monsters[i].data._mFlags |= (MFLAG_TARGETS_MONSTER | MFLAG_GOLEM);
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
			if (sgLevels[lvl.currlevel].item[i].bCmd == CMD_WALKXY) {
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
			if (sgLevels[lvl.currlevel].item[i].bCmd == CMD_ACK_PLRINFO) {
				ii = itemavail[0];
				itemavail[0] = itemavail[MAXITEMS - numitems - 1];
				itemactive[numitems] = ii;
				if (sgLevels[lvl.currlevel].item[i].wIndx == IDI_EAR) {
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
						item[ii]._iIdentified = TRUE;
					item[ii]._iDurability = sgLevels[lvl.currlevel].item[i].bDur;
					item[ii]._iMaxDur = sgLevels[lvl.currlevel].item[i].bMDur;
					item[ii]._iCharges = sgLevels[lvl.currlevel].item[i].bCh;
					item[ii]._iMaxCharges = sgLevels[lvl.currlevel].item[i].bMCh;
				}
				pos.x = sgLevels[lvl.currlevel].item[i].x;
				pos.y = sgLevels[lvl.currlevel].item[i].y;
				if (!CanPut(pos)) {
					done = FALSE;
					for (k = 1; k < 50 && !done; k++) {
						for (j = -k; j <= k && !done; j++) {
							posb.y = pos.y + j;
							for (l = -k; l <= k && !done; l++) {
								posb.x = pos.x + l;
								if (CanPut(posb)) {
									done = TRUE;
									pos = posb;
								}
							}
						}
					}
				}
				item[ii]._i = pos;
				grid.at(pos).setItem(ii);
				RespawnItem(ii, FALSE);
				numitems++;
			}
		}
	}

	if (lvl.currlevel != 0) {
		for (i = 0; i < MAXOBJECTS; i++) {
			switch (sgLevels[lvl.currlevel].object[i].bCmd) {
			case CMD_OPENDOOR:
			case CMD_CLOSEDOOR:
			case CMD_OPERATEOBJ:
			case CMD_PLROPOBJ:
				SyncOpObject(-1, sgLevels[lvl.currlevel].object[i].bCmd, i);
				break;
			case CMD_BREAKOBJ:
				SyncBreakObj(-1, i);
				break;
			}
		}

		for (i = 0; i < nobjects; i++) {
			ot = object[objectactive[i]]._otype;
			if (ot == OBJ_TRAPL || ot == OBJ_TRAPR)
				Obj_Trap(objectactive[i]);
		}
	}
	deltaload = FALSE;
}

void NetSendCmd(bool bHiPri, BYTE bCmd)
{
	TCmd cmd;

	cmd.bCmd = bCmd;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdGolem(V2Di pos, BYTE dir, BYTE menemy, int hp, BYTE cl)
{
	TCmdGolem cmd;

	cmd.bCmd = CMD_AWAKEGOLEM;
	cmd._mx = pos.x;
	cmd._my = pos.y;
	cmd._mdir = dir;
	cmd._menemy = menemy;
	cmd._mhitpoints = hp;
	cmd.currlevel = cl;
	NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdLoc(bool bHiPri, BYTE bCmd, V2Di pos)
{
	ALIGN_BY_1 TCmdLoc cmd;

	cmd.bCmd = bCmd;
	cmd.x = pos.x;
	cmd.y = pos.y;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam1(bool bHiPri, BYTE bCmd, V2Di pos, WORD wParam1)
{
	TCmdLocParam1 cmd;

	cmd.bCmd = bCmd;
	cmd.x = pos.x;
	cmd.y = pos.y;
	cmd.wParam1 = wParam1;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam2(bool bHiPri, BYTE bCmd, V2Di pos, WORD wParam1, WORD wParam2)
{
	TCmdLocParam2 cmd;

	cmd.bCmd = bCmd;
	cmd.x = pos.x;
	cmd.y = pos.y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam3(bool bHiPri, BYTE bCmd, V2Di pos, WORD wParam1, WORD wParam2, WORD wParam3)
{
	TCmdLocParam3 cmd;

	cmd.bCmd = bCmd;
	cmd.x = pos.x;
	cmd.y = pos.y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdParam1(bool bHiPri, BYTE bCmd, WORD wParam1)
{
	ALIGN_BY_1 TCmdParam1 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdParam2(bool bHiPri, BYTE bCmd, WORD wParam1, WORD wParam2)
{
	TCmdParam2 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdParam3(bool bHiPri, BYTE bCmd, WORD wParam1, WORD wParam2, WORD wParam3)
{
	TCmdParam3 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdQuest(bool bHiPri, BYTE q)
{
	TCmdQuest cmd;

	cmd.q = q;
	cmd.bCmd = CMD_SYNCQUEST;
	cmd.qstate = quests[q]._qactive;
	cmd.qlog = quests[q]._qlog;
	cmd.qvar1 = quests[q]._qvar1;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdGItem(bool bHiPri, BYTE bCmd, BYTE mast, BYTE pnum, BYTE ii)
{
	TCmdGItem cmd;

	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;
	cmd.bLevel = lvl.currlevel;
	cmd.bCursitem = ii;
	cmd.dwTime = 0;
	cmd.x = item[ii]._i.x;
	cmd.y = item[ii]._i.y;
	cmd.wIndx = item[ii].IDidx;

	if (item[ii].IDidx == IDI_EAR) {
		cmd.wCI = item[ii]._iName[8] | (item[ii]._iName[7] << 8);
		cmd.dwSeed = item[ii]._iName[12] | ((item[ii]._iName[11] | ((item[ii]._iName[10] | (item[ii]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = item[ii]._iName[13];
		cmd.bDur = item[ii]._iName[14];
		cmd.bMDur = item[ii]._iName[15];
		cmd.bCh = item[ii]._iName[16];
		cmd.bMCh = item[ii]._iName[17];
		cmd.wValue = item[ii]._ivalue | (item[ii]._iName[18] << 8) | ((item[ii]._iCurs - 19) << 6);
		cmd.dwBuff = item[ii]._iName[22] | ((item[ii]._iName[21] | ((item[ii]._iName[20] | (item[ii]._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = item[ii]._iCreateInfo;
		cmd.dwSeed = item[ii]._iSeed;
		cmd.bId = item[ii]._iIdentified;
		cmd.bDur = item[ii]._iDurability;
		cmd.bMDur = item[ii]._iMaxDur;
		cmd.bCh = item[ii]._iCharges;
		cmd.bMCh = item[ii]._iMaxCharges;
		cmd.wValue = item[ii]._ivalue;
	}

	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdGItem2(bool usonly, BYTE bCmd, BYTE mast, BYTE pnum, TCmdGItem *p)
{
	int ticks;
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.bPnum = pnum;
	cmd.bCmd = bCmd;
	cmd.bMaster = mast;

	if (!usonly) {
		cmd.dwTime = 0;
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
		return;
	}

	ticks = SDL_GetTicks();
	if (!cmd.dwTime) {
		cmd.dwTime = ticks;
	} else if (ticks - cmd.dwTime > 5000) {
		return;
	}

	multi_msg_add((BYTE *)&cmd.bCmd, sizeof(cmd));
}

bool NetSendCmdReq2(BYTE bCmd, BYTE mast, BYTE pnum, TCmdGItem *p)
{
	int ticks;
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;

	ticks = SDL_GetTicks();
	if (!cmd.dwTime) {
		cmd.dwTime = ticks;
	} else if (ticks - cmd.dwTime > 5000) {
		return FALSE;
	}

	multi_msg_add((BYTE *)&cmd.bCmd, sizeof(cmd));

	return TRUE;
}

void NetSendCmdExtra(TCmdGItem *p)
{
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.dwTime = 0;
	cmd.bCmd = CMD_ITEMEXTRA;
	NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdPItem(bool bHiPri, BYTE bCmd, BYTE x, BYTE y)
{
	TCmdPItem cmd;

	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wIndx = myplr().data.HoldItem.IDidx;

	if (myplr().data.HoldItem.IDidx == IDI_EAR) {
		cmd.wCI = myplr().data.HoldItem._iName[8] | (myplr().data.HoldItem._iName[7] << 8);
		cmd.dwSeed = myplr().data.HoldItem._iName[12] | ((myplr().data.HoldItem._iName[11] | ((myplr().data.HoldItem._iName[10] | (myplr().data.HoldItem._iName[9] << 8)) << 8)) << 8);
		cmd.bId = myplr().data.HoldItem._iName[13];
		cmd.bDur = myplr().data.HoldItem._iName[14];
		cmd.bMDur = myplr().data.HoldItem._iName[15];
		cmd.bCh = myplr().data.HoldItem._iName[16];
		cmd.bMCh = myplr().data.HoldItem._iName[17];
		cmd.wValue = myplr().data.HoldItem._ivalue | (myplr().data.HoldItem._iName[18] << 8) | ((myplr().data.HoldItem._iCurs - 19) << 6);
		cmd.dwBuff = myplr().data.HoldItem._iName[22] | ((myplr().data.HoldItem._iName[21] | ((myplr().data.HoldItem._iName[20] | (myplr().data.HoldItem._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = myplr().data.HoldItem._iCreateInfo;
		cmd.dwSeed = myplr().data.HoldItem._iSeed;
		cmd.bId = myplr().data.HoldItem._iIdentified;
		cmd.bDur = myplr().data.HoldItem._iDurability;
		cmd.bMDur = myplr().data.HoldItem._iMaxDur;
		cmd.bCh = myplr().data.HoldItem._iCharges;
		cmd.bMCh = myplr().data.HoldItem._iMaxCharges;
		cmd.wValue = myplr().data.HoldItem._ivalue;
	}

	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdChItem(bool bHiPri, BYTE bLoc)
{
	TCmdChItem cmd;

	cmd.bCmd = CMD_CHANGEPLRITEMS;
	cmd.bLoc = bLoc;
	cmd.wIndx = myplr().data.HoldItem.IDidx;
	cmd.wCI = myplr().data.HoldItem._iCreateInfo;
	cmd.dwSeed = myplr().data.HoldItem._iSeed;
	cmd.bId = myplr().data.HoldItem._iIdentified;

	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdDelItem(bool bHiPri, BYTE bLoc)
{
	TCmdDelItem cmd;

	cmd.bLoc = bLoc;
	cmd.bCmd = CMD_DELPLRITEMS;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdDItem(bool bHiPri, int ii)
{
	TCmdPItem cmd;

	cmd.bCmd = CMD_DROPITEM;
	cmd.x = item[ii]._i.x;
	cmd.y = item[ii]._i.y;
	cmd.wIndx = item[ii].IDidx;

	if (item[ii].IDidx == IDI_EAR) {
		cmd.wCI = item[ii]._iName[8] | (item[ii]._iName[7] << 8);
		cmd.dwSeed = item[ii]._iName[12] | ((item[ii]._iName[11] | ((item[ii]._iName[10] | (item[ii]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = item[ii]._iName[13];
		cmd.bDur = item[ii]._iName[14];
		cmd.bMDur = item[ii]._iName[15];
		cmd.bCh = item[ii]._iName[16];
		cmd.bMCh = item[ii]._iName[17];
		cmd.wValue = item[ii]._ivalue | (item[ii]._iName[18] << 8) | ((item[ii]._iCurs - 19) << 6);
		cmd.dwBuff = item[ii]._iName[22] | ((item[ii]._iName[21] | ((item[ii]._iName[20] | (item[ii]._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = item[ii]._iCreateInfo;
		cmd.dwSeed = item[ii]._iSeed;
		cmd.bId = item[ii]._iIdentified;
		cmd.bDur = item[ii]._iDurability;
		cmd.bMDur = item[ii]._iMaxDur;
		cmd.bCh = item[ii]._iCharges;
		cmd.bMCh = item[ii]._iMaxCharges;
		cmd.wValue = item[ii]._ivalue;
	}

	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdDamage(bool bHiPri, BYTE bPlr, DWORD dwDam)
{
	TCmdDamage cmd;

	cmd.bCmd = CMD_PLRDAMAGE;
	cmd.bPlr = bPlr;
	cmd.dwDam = dwDam;
	if (bHiPri)
		NetSendHiPri((BYTE *)&cmd, sizeof(cmd));
	else
		NetSendLoPri((BYTE *)&cmd, sizeof(cmd));
}

void NetSendCmdString(int pmask, const char *pszStr)
{
	int dwStrLen;
	TCmdString cmd;

	dwStrLen = strlen(pszStr);
	cmd.bCmd = CMD_STRING;
	strcpy(cmd.str, pszStr);
	multi_send_msg_packet(pmask, (BYTE *)&cmd.bCmd, dwStrLen + 2);
}

void delta_close_portal(int pnum)
{
	memset(&sgJunk.portal[pnum], 0xFF, sizeof(sgJunk.portal[pnum]));
	sgbDeltaChanged = TRUE;
}

DWORD ParseCmd(int pnum, TCmd *pCmd)
{
	sbLastCmd = pCmd->bCmd;
	if (sgwPackPlrOffsetTbl[pnum] != 0 && sbLastCmd != CMD_ACK_PLRINFO && sbLastCmd != CMD_SEND_PLRINFO)
		return 0;

	switch (pCmd->bCmd) {
	case CMD_SYNCDATA:
		return On_SYNCDATA(pCmd, pnum);
	case CMD_WALKXY:
		return On_WALKXY(pCmd, pnum);
	case CMD_ADDSTR:
		return On_ADDSTR(pCmd, pnum);
	case CMD_ADDDEX:
		return On_ADDDEX(pCmd, pnum);
	case CMD_ADDMAG:
		return On_ADDMAG(pCmd, pnum);
	case CMD_ADDVIT:
		return On_ADDVIT(pCmd, pnum);
	case CMD_SBSPELL:
		return On_SBSPELL(pCmd, pnum);
	case CMD_GOTOGETITEM:
		return On_GOTOGETITEM(pCmd, pnum);
	case CMD_REQUESTGITEM:
		return On_REQUESTGITEM(pCmd, pnum);
	case CMD_GETITEM:
		return On_GETITEM(pCmd, pnum);
	case CMD_GOTOAGETITEM:
		return On_GOTOAGETITEM(pCmd, pnum);
	case CMD_REQUESTAGITEM:
		return On_REQUESTAGITEM(pCmd, pnum);
	case CMD_AGETITEM:
		return On_AGETITEM(pCmd, pnum);
	case CMD_ITEMEXTRA:
		return On_ITEMEXTRA(pCmd, pnum);
	case CMD_PUTITEM:
		return On_PUTITEM(pCmd, pnum);
	case CMD_SYNCPUTITEM:
		return On_SYNCPUTITEM(pCmd, pnum);
	case CMD_RESPAWNITEM:
		return On_RESPAWNITEM(pCmd, pnum);
	case CMD_ATTACKXY:
		return On_ATTACKXY(pCmd, pnum);
	case CMD_SATTACKXY:
		return On_SATTACKXY(pCmd, pnum);
	case CMD_RATTACKXY:
		return On_RATTACKXY(pCmd, pnum);
	case CMD_SPELLXYD:
		return On_SPELLXYD(pCmd, pnum);
	case CMD_SPELLXY:
		return On_SPELLXY(pCmd, pnum);
	case CMD_TSPELLXY:
		return On_TSPELLXY(pCmd, pnum);
	case CMD_OPOBJXY:
		return On_OPOBJXY(pCmd, pnum);
	case CMD_DISARMXY:
		return On_DISARMXY(pCmd, pnum);
	case CMD_OPOBJT:
		return On_OPOBJT(pCmd, pnum);
	case CMD_ATTACKID:
		return On_ATTACKID(pCmd, pnum);
	case CMD_ATTACKPID:
		return On_ATTACKPID(pCmd, pnum);
	case CMD_RATTACKID:
		return On_RATTACKID(pCmd, pnum);
	case CMD_RATTACKPID:
		return On_RATTACKPID(pCmd, pnum);
	case CMD_SPELLID:
		return On_SPELLID(pCmd, pnum);
	case CMD_SPELLPID:
		return On_SPELLPID(pCmd, pnum);
	case CMD_TSPELLID:
		return On_TSPELLID(pCmd, pnum);
	case CMD_TSPELLPID:
		return On_TSPELLPID(pCmd, pnum);
	case CMD_KNOCKBACK:
		return On_KNOCKBACK(pCmd, pnum);
	case CMD_RESURRECT:
		return On_RESURRECT(pCmd, pnum);
	case CMD_HEALOTHER:
		return On_HEALOTHER(pCmd, pnum);
	case CMD_TALKXY:
		return On_TALKXY(pCmd, pnum);
	case CMD_DEBUG:
		return On_DEBUG(pCmd, pnum);
	case CMD_NEWLVL:
		return On_NEWLVL(pCmd, pnum);
	case CMD_WARP:
		return On_WARP(pCmd, pnum);
	case CMD_MONSTDEATH:
		return On_MONSTDEATH(pCmd, pnum);
	case CMD_KILLGOLEM:
		return On_KILLGOLEM(pCmd, pnum);
	case CMD_AWAKEGOLEM:
		return On_AWAKEGOLEM(pCmd, pnum);
	case CMD_MONSTDAMAGE:
		return On_MONSTDAMAGE(pCmd, pnum);
	case CMD_PLRDEAD:
		return On_PLRDEAD(pCmd, pnum);
	case CMD_PLRDAMAGE:
		return On_PLRDAMAGE(pCmd, pnum);
	case CMD_OPENDOOR:
		return On_OPENDOOR(pCmd, pnum);
	case CMD_CLOSEDOOR:
		return On_CLOSEDOOR(pCmd, pnum);
	case CMD_OPERATEOBJ:
		return On_OPERATEOBJ(pCmd, pnum);
	case CMD_PLROPOBJ:
		return On_PLROPOBJ(pCmd, pnum);
	case CMD_BREAKOBJ:
		return On_BREAKOBJ(pCmd, pnum);
	case CMD_CHANGEPLRITEMS:
		return On_CHANGEPLRITEMS(pCmd, pnum);
	case CMD_DELPLRITEMS:
		return On_DELPLRITEMS(pCmd, pnum);
	case CMD_PLRLEVEL:
		return On_PLRLEVEL(pCmd, pnum);
	case CMD_DROPITEM:
		return On_DROPITEM(pCmd, pnum);
	case CMD_ACK_PLRINFO:
		return On_ACK_PLRINFO(pCmd, pnum);
	case CMD_SEND_PLRINFO:
		return On_SEND_PLRINFO(pCmd, pnum);
	case CMD_PLAYER_JOINLEVEL:
		return On_PLAYER_JOINLEVEL(pCmd, pnum);
	case CMD_ACTIVATEPORTAL:
		return On_ACTIVATEPORTAL(pCmd, pnum);
	case CMD_DEACTIVATEPORTAL:
		return On_DEACTIVATEPORTAL(pCmd, pnum);
	case CMD_RETOWN:
		return On_RETOWN(pCmd, pnum);
	case CMD_SETSTR:
		return On_SETSTR(pCmd, pnum);
	case CMD_SETMAG:
		return On_SETMAG(pCmd, pnum);
	case CMD_SETDEX:
		return On_SETDEX(pCmd, pnum);
	case CMD_SETVIT:
		return On_SETVIT(pCmd, pnum);
	case CMD_STRING:
		return On_STRING(pCmd, pnum);
	case CMD_SYNCQUEST:
		return On_SYNCQUEST(pCmd, pnum);
	case CMD_ENDSHIELD:
		return On_ENDSHIELD(pCmd, pnum);
	case CMD_CHEAT_EXPERIENCE:
		return On_CHEAT_EXPERIENCE(pCmd, pnum);
	case CMD_CHEAT_SPELL_LEVEL:
		return On_CHEAT_SPELL_LEVEL(pCmd, pnum);
	case CMD_NOVA:
		return On_NOVA(pCmd, pnum);
	case CMD_SETSHIELD:
		return On_SETSHIELD(pCmd, pnum);
	case CMD_REMSHIELD:
		return On_REMSHIELD(pCmd, pnum);
	}

	if (pCmd->bCmd < CMD_DLEVEL_0 || pCmd->bCmd > CMD_DLEVEL_END) {
		SNetDropPlayer(pnum, 0x40000006);
		return 0;
	}

	return On_DLEVEL(pnum, pCmd);
}

DWORD On_DLEVEL(int pnum, TCmd *pCmd)
{
	TCmdPlrInfoHdr *p = (TCmdPlrInfoHdr *)pCmd;

	if (gbDeltaSender != pnum) {
		if (p->bCmd == CMD_DLEVEL_END) {
			gbDeltaSender = pnum;
			sgbRecvCmd = CMD_DLEVEL_END;
		} else if (p->bCmd == CMD_DLEVEL_0 && p->wOffset == 0) {
			gbDeltaSender = pnum;
			sgbRecvCmd = CMD_DLEVEL_END;
		} else {
			return p->wBytes + sizeof(*p);
		}
	}
	if (sgbRecvCmd == CMD_DLEVEL_END) {
		if (p->bCmd == CMD_DLEVEL_END) {
			sgbDeltaChunks = 20;
			return p->wBytes + sizeof(*p);
		} else if (p->bCmd == CMD_DLEVEL_0 && p->wOffset == 0) {
			sgdwRecvOffset = 0;
			sgbRecvCmd = p->bCmd;
		} else {
			return p->wBytes + sizeof(*p);
		}
	} else if (sgbRecvCmd != p->bCmd) {
		DeltaImportData(sgbRecvCmd, sgdwRecvOffset);
		if (p->bCmd == CMD_DLEVEL_END) {
			sgbDeltaChunks = 20;
			sgbRecvCmd = CMD_DLEVEL_END;
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

void DeltaImportData(BYTE cmd, DWORD recv_offset)
{
	BYTE i;
	BYTE *src;

	if (sgRecvBuf[0])
		PkwareDecompress(&sgRecvBuf[1], recv_offset, 4721);

	src = &sgRecvBuf[1];
	if (cmd == CMD_DLEVEL_JUNK) {
		DeltaImportJunk(src);
	} else if (cmd >= CMD_DLEVEL_0 && cmd <= CMD_DLEVEL_16) {
		i = cmd - CMD_DLEVEL_0;
		src = DeltaImportItem(src, sgLevels[i].item);
		src = DeltaImportObject(src, sgLevels[i].object);
		DeltaImportMonster(src, sgLevels[i].monster);
	} else {
		app_fatal("msg:1");
	}

	sgbDeltaChunks++;
	sgbDeltaChanged = TRUE;
}

BYTE *DeltaImportItem(BYTE *src, TCmdPItem *dst)
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

BYTE *DeltaImportObject(BYTE *src, DObjectStr *dst)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return src + sizeof(DObjectStr) * MAXOBJECTS;
}

BYTE *DeltaImportMonster(BYTE *src, DMonsterStr *dst)
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

void DeltaImportJunk(BYTE *src)
{
	int i;
	MultiQuests *mq;

	for (i = 0; i < MAXPORTAL; i++) {
		if (*src == 0xFF) {
			memset(&sgJunk.portal[i], 0xFF, sizeof(DPortal));
			src++;
			SetPortalStats(i, FALSE, { 0, 0 }, 0, DTYPE_TOWN);
		} else {
			memcpy(&sgJunk.portal[i], src, sizeof(DPortal));
			src += sizeof(DPortal);
			SetPortalStats(
			    i,
			    TRUE,
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
	return sync_update(pnum, (const BYTE *)pCmd);
}

DWORD On_WALKXY(TCmd *pCmd, int pnum)
{
	TCmdLoc *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].MakePlrPath({ p->x, p->y }, TRUE);
		plr[pnum].data.destAction = ACTION_NONE;
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
			plr[pnum].data.destAction = ACTION_SPELL;
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
		plr[pnum].MakePlrPath({ p->x, p->y }, FALSE);
		plr[pnum].data.destAction = ACTION_PICKUPITEM;
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
				NetSendCmdGItem2(FALSE, CMD_GETITEM, myplr(), p->bPnum, p);
				if (p->bPnum != myplr())
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					myplr().inventory.InvGetItem(ii);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTGITEM, myplr(), p->bPnum, p))
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
						ii = myplr().inventory.SyncPutItem(myplr().pos(), p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
						if (ii != -1)
							myplr().inventory.InvGetItem(ii);
					} else
						myplr().inventory.InvGetItem(ii);
				} else
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
			}
		} else
			NetSendCmdGItem2(TRUE, CMD_GETITEM, p->bMaster, p->bPnum, p);
	}

	return sizeof(*p);
}

bool delta_get_item(TCmdGItem *pI, BYTE bLevel)
{
	bool result;
	TCmdPItem *pD;
	int i;
	bool found;

	result = TRUE;
	found = FALSE;
	if (plr.isMultiplayer()) {
		pD = sgLevels[bLevel].item;
		for (i = 0; i < MAXITEMS; i++, pD++) {
			if (pD->bCmd != 0xFF && pD->wIndx == pI->wIndx && pD->wCI == pI->wCI && pD->dwSeed == pI->dwSeed) {
				found = TRUE;
				break;
			}
		}
		if (found) {
			if (pD->bCmd == CMD_WALKXY) {
				return result;
			}
			if (pD->bCmd == CMD_STAND) {
				sgbDeltaChanged = 1;
				pD->bCmd = CMD_WALKXY;
				return result;
			}
			if (pD->bCmd == CMD_ACK_PLRINFO) {
				pD->bCmd = 0xFF;
				sgbDeltaChanged = 1;
				return result;
			}
			app_fatal("delta:1");
		}
		if (((pI->wCI >> 8) & 0x80) == 0)
			return FALSE;
		pD = sgLevels[bLevel].item;
		for (i = 0; i < MAXITEMS; i++, pD++) {
			if (pD->bCmd == 0xFF) {
				sgbDeltaChanged = 1;
				pD->bCmd = CMD_WALKXY;
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
				result = TRUE;
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
		plr[pnum].MakePlrPath({ p->x, p->y }, FALSE);
		plr[pnum].data.destAction = ACTION_PICKUPAITEM;
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
				NetSendCmdGItem2(FALSE, CMD_AGETITEM, myplr(), p->bPnum, p);
				if (p->bPnum != myplr())
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					myplr().inventory.AutoGetItem(p->bCursitem);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTAGITEM, myplr(), p->bPnum, p))
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
						int ii = myplr().inventory.SyncPutItem(myplr().pos(), p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
						if (ii != -1)
							myplr().inventory.AutoGetItem(ii);
					} else
						myplr().inventory.AutoGetItem(p->bCursitem);
				} else
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
			}
		} else
			NetSendCmdGItem2(TRUE, CMD_AGETITEM, p->bMaster, p->bPnum, p);
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
			ii = myplr().inventory.InvPutItem({ p->x, p->y });
		else
			ii = myplr().inventory.SyncPutItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
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

void delta_put_item(TCmdPItem *pI, V2Di pos, BYTE bLevel)
{
	int i;
	TCmdPItem *pD;

	if (plr.isSingleplayer()) {
		return;
	}
	pD = sgLevels[bLevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != CMD_WALKXY
		    && pD->bCmd != 0xFF
		    && pD->wIndx == pI->wIndx
		    && pD->wCI == pI->wCI
		    && pD->dwSeed == pI->dwSeed) {
			if (pD->bCmd == CMD_ACK_PLRINFO)
				return;
			app_fatal("Trying to drop a floor item?");
		}
	}

	pD = sgLevels[bLevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == 0xFF) {
			sgbDeltaChanged = TRUE;
			memcpy(pD, pI, sizeof(TCmdPItem));
			pD->bCmd = CMD_ACK_PLRINFO;
			pD->x = pos.x;
			pD->y = pos.y;
			return;
		}
	}
}

void check_update_plr(int pnum)
{
	if (plr.isMultiplayer() && pnum == myplr())
		pfile_update(TRUE);
}

DWORD On_SYNCPUTITEM(TCmd *pCmd, int pnum)
{
	TCmdPItem *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (lvl.currlevel == plr[pnum].data.plrlevel) {
		int ii = myplr().inventory.SyncPutItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
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
			myplr().inventory.SyncPutItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff);
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
		plr[pnum].MakePlrPath({ p->x, p->y }, FALSE);
		plr[pnum].data.destAction = ACTION_ATTACK;
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
		plr[pnum].data.destAction = ACTION_ATTACK;
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
		plr[pnum].data.destAction = ACTION_RATTACK;
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
			plr[pnum].data.destAction = ACTION_SPELLWALL;
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
			plr[pnum].data.destAction = ACTION_SPELL;
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
			plr[pnum].data.destAction = ACTION_SPELL;
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
			plr[pnum].MakePlrPath({ p->x, p->y }, FALSE);
		else
			plr[pnum].MakePlrPath({ p->x, p->y }, TRUE);
		plr[pnum].data.destAction = ACTION_OPERATE;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_DISARMXY(TCmd *pCmd, int pnum)
{
	TCmdLocParam1 *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		if (object[p->wParam1]._oSolidFlag || object[p->wParam1]._oDoorFlag)
			plr[pnum].MakePlrPath({ p->x, p->y }, FALSE);
		else
			plr[pnum].MakePlrPath({ p->x, p->y }, TRUE);
		plr[pnum].data.destAction = ACTION_DISARM;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_OPOBJT(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].data.destAction = ACTION_OPERATETK;
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
			plr[pnum].MakePlrPath({ monsters[p->wParam1].data._mfut.x, monsters[p->wParam1].data._mfut.y }, FALSE);
		plr[pnum].data.destAction = ACTION_ATTACKMON;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_ATTACKPID(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].MakePlrPath(plr[p->wParam1].futpos(), FALSE);
		plr[pnum].data.destAction = ACTION_ATTACKPLR;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_RATTACKID(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].data.destAction = ACTION_RATTACKMON;
		plr[pnum].data.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD On_RATTACKPID(TCmd *pCmd, int pnum)
{
	TCmdParam1 *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && lvl.currlevel == plr[pnum].data.plrlevel) {
		plr[pnum].ClrPlrPath();
		plr[pnum].data.destAction = ACTION_RATTACKPLR;
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
			plr[pnum].data.destAction = ACTION_SPELLMON;
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
			plr[pnum].data.destAction = ACTION_SPELLPLR;
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
			plr[pnum].data.destAction = ACTION_SPELLMON;
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
			plr[pnum].data.destAction = ACTION_SPELLPLR;
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
		monsters[p->wParam1].M_StartHit(pnum, 0);
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
		plr[pnum].MakePlrPath({ p->x, p->y }, FALSE);
		plr[pnum].data.destAction = ACTION_TALK;
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
		if (pnum == myplr() && pcurs >= CURSOR_FIRSTITEM) {
			item[MAXITEMS] = myplr().data.HoldItem;
			myplr().inventory.AutoGetItem(MAXITEMS);
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
		bool addGolem = TRUE;
		for (i = 0; i < nummissiles; i++) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MIS_GOLEM && missile[mi]._misource == pnum) {
				addGolem = FALSE;
				// BUGFIX: break, don't need to check the rest
			}
		}
		if (addGolem)
			AddMissile(plr[pnum].pos(), { p->_mx, p->_my }, Dir(p->_mdir), MIS_GOLEM, 0, pnum, 0, 1);
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
				drawhpflag = TRUE;
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
			SyncOpObject(pnum, CMD_OPENDOOR, p->wParam1);
		delta_sync_object(p->wParam1, CMD_OPENDOOR, plr[pnum].data.plrlevel);
	}

	return sizeof(*p);
}

void delta_sync_object(int oi, BYTE bCmd, BYTE bLevel)
{
	if (plr.isMultiplayer()) {
		sgbDeltaChanged = TRUE;
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
			SyncOpObject(pnum, CMD_CLOSEDOOR, p->wParam1);
		delta_sync_object(p->wParam1, CMD_CLOSEDOOR, plr[pnum].data.plrlevel);
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
			SyncOpObject(pnum, CMD_OPERATEOBJ, p->wParam1);
		delta_sync_object(p->wParam1, CMD_OPERATEOBJ, plr[pnum].data.plrlevel);
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
			SyncOpObject(p->wParam1, CMD_PLROPOBJ, p->wParam2);
		delta_sync_object(p->wParam2, CMD_PLROPOBJ, plr[pnum].data.plrlevel);
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
		delta_sync_object(p->wParam2, CMD_BREAKOBJ, plr[pnum].data.plrlevel);
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
		recv_plrinfo(pnum, p, p->bCmd == CMD_ACK_PLRINFO);

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
		plr[pnum].data._pLvlChanging = FALSE;
		if (plr[pnum].data._pName[0] && !plr[pnum].data.plractive) {
			plr[pnum].data.plractive = TRUE;
			gbActivePlayers++;
			EventPlrMsg("Player '%s' (level %d) just joined the game", plr[pnum].data._pName, plr[pnum].data._pLevel);
		}

		if (plr[pnum].data.plractive && myplr() != pnum) {
			grid.at({ p->x, p->y }).setPlayer(pnum);
			plr[pnum].data.plrlevel = p->wParam1;
			plr[pnum].data._pGFXLoad = 0;
			if (lvl.currlevel == plr[pnum].data.plrlevel) {
				plr[pnum].LoadPlrGFX(PFILE_STAND);
				plr[pnum].SyncInitPlr();
				if ((plr[pnum].data._pHitPoints >> 6) > 0)
					plr[pnum].StartStand(Dir(0));
				else {
					plr[pnum].data._pgfxnum = 0;
					plr[pnum].LoadPlrGFX(PFILE_DEATH);
					plr[pnum].data._pmode = PM_DEATH;
					plr[pnum].NewPlrAnim(plr[pnum].data._pDAnim[0], plr[pnum].data._pDFrames, 1, plr[pnum].data._pDWidth);
					plr[pnum].data._pAnimFrame = plr[pnum].data._pAnimLen - 1;
					plr[pnum].data._pVar8 = plr[pnum].data._pAnimLen << 1;
					grid.at(plr[pnum].pos()).dFlags |= BFLAG_DEAD_PLAYER;
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
		ActivatePortal(pnum, { p->x, p->y }, p->wParam1, p->wParam2, p->wParam3);
		if (pnum != myplr()) {
			if (lvl.currlevel == 0)
				AddInTownPortal(pnum);
			else if (lvl.currlevel == plr[pnum].data.plrlevel) {
				int i;
				bool addPortal = TRUE;
				for (i = 0; i < nummissiles; i++) {
					int mi = missileactive[i];
					if (missile[mi]._mitype == MIS_TOWN && missile[mi]._misource == pnum) {
						addPortal = FALSE;
						// BUGFIX: break
					}
				}
				if (addPortal)
					AddWarpMissile(pnum, { p->x, p->y });
			} else
				RemovePortalMissile(pnum);
		}
		delta_open_portal(pnum, { p->x, p->y }, p->wParam1, p->wParam2, p->wParam3);
	}

	return sizeof(*p);
}

void delta_open_portal(int pnum, V2Di pos, BYTE bLevel, BYTE bLType, BYTE bSetLvl)
{
	sgbDeltaChanged = TRUE;
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
			deathflag = FALSE;
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
		sgbDeltaChanged = TRUE;
	}

	return sizeof(*p);
}

DWORD On_ENDSHIELD(TCmd *pCmd, int pnum)
{
	int i;

	if (gbBufferMsgs != 1 && pnum != myplr() && lvl.currlevel == plr[pnum].data.plrlevel) {
		for (i = 0; i < nummissiles; i++) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == pnum) {
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
		plr[pnum].data._pSpell = SPL_NOVA;
		plr[pnum].data._pSplType = RSPLTYPE_INVALID;
		plr[pnum].data._pSplFrom = 3;
		plr[pnum].data.destAction = ACTION_SPELL;
		plr[pnum].data.destParam1 = p->x;
		plr[pnum].data.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD On_SETSHIELD(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1)
		plr[pnum].data.pManaShield = TRUE;

	return sizeof(*pCmd);
}

DWORD On_REMSHIELD(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1)
		plr[pnum].data.pManaShield = FALSE;

	return sizeof(*pCmd);
}

DEVILUTION_END_NAMESPACE
