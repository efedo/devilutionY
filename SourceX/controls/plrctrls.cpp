#include "controls/plrctrls.h"

#include <cstdint>
#include <algorithm>
#include <list>

#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "controls/game_controls.h"

namespace dvl {

bool sgbControllerActive = false;
coords speedspellscoords[50];
const int repeatRate = 100;
int speedspellcount = 0;

// Native game menu, controlled by simulating a keyboard.
bool InGameMenu()
{
	return stextflag > 0
	    || helpflag
	    || talkflag
	    || qtextflag
	    || gmenu_is_active()
	    || PauseMode == 2
//	    || myplr().data._pInvincible
		;
}

namespace {

DWORD invmove = 0;
int hsr[3] = { 0, 0, 0 }; // hot spell row counts
int slot = SLOTXY_INV_FIRST;
int spbslot = 0;

/**
 * Number of angles to turn to face the coordinate
 * @param x Tile coordinates
 * @param y Tile coordinates
 * @return -1 == down
 */
int GetRotaryDistance(V2Di p)
{
	if (myplr().futpos() == p) return -1;
	Dir d1 = myplr().data._pdir;
	Dir d2 = GetDirection(myplr().futpos(), p);
	int d = abs(int(d1) - int(d2));
	if (d > 4) return 4 - (d % 4);
	return d;
}

/**
 * @brief Get the best case walking steps to coordinates
 * @param dx Tile coordinates
 * @param dy Tile coordinates
 */
int GetMinDistance(V2Di p)
{
	return (myplr().futpos() - p).maxabs();
}

/**
 * @brief Get walking steps to coordinate
 * @param dx Tile coordinates
 * @param dy Tile coordinates
 * @param maxDistance the max number of steps to search
 * @return number of steps, or 0 if not reachable
 */
int GetDistance(V2Di d, int maxDistance)
{
	if (GetMinDistance(d) > maxDistance) return 0;
	//char walkpath[MAX_PATH_LENGTH];
	std::queue<PathNode> walkpath;
	int steps = pathfinder.FindPath(PosOkPlayer, myplr(), myplr().futpos(), d, walkpath);
	if (steps > maxDistance) return 0;
	return steps;
}

/**
 * @brief Get distance to coordinate
 * @param dx Tile coordinates
 * @param dy Tile coordinates
 */
int GetDistanceRanged(V2Di d)
{
	V2Di off = myplr().futpos() - d;
	return sqrt(off.x * off.x + off.y * off.y);
}

void FindItemOrObject()
{
	V2Di m = myplr().futpos();
	int rotations = 5;

	// As the player can not stand on the edge fo the map this is safe from OOB
	for (int x = -1; x < 2; x++) {
		for (int y = -1; y < 2; y++) {
			V2Di tp = m + V2Di( x, y );

			if (!grid.at(tp).isItem()) continue;
			int i = grid.at(tp).getItem();
			if (item[i]._itype == ITYPE_NONE
			    || item[i]._iSelFlag == 0)
				continue;
			int newRotations = GetRotaryDistance(tp);
			if (rotations < newRotations)
				continue;
			if (x != 0 && y != 0 && GetDistance(tp, 1) == 0)
				continue;
			rotations = newRotations;
			pcursitem = i;
			cursm = tp;
		}
	}

	if (lvl.type() == DunType::town || pcursitem != -1)
		return; // Don't look for objects in town

	for (int x = -1; x < 2; x++) {
		for (int y = -1; y < 2; y++) {
			V2Di tp = m + V2Di(x, y);

			if (!grid.at(tp).isObject()) continue;
			int o = grid.at(tp).getObject();
			if (object[o]._oSelFlag == 0)
				continue;
			if (x == 0 && y == 0 && object[o]._oDoorFlag)
				continue; // Ignore doorway so we don't get stuck behind barrels
			int newRotations = GetRotaryDistance(tp);
			if (rotations < newRotations)
				continue;
			if (x != 0 && y != 0 && GetDistance(tp, 1) == 0)
				continue;
			rotations = newRotations;
			pcursobj = o;
			cursm = tp;
		}
	}
}

void CheckTownersNearby()
{
	for (int i = 0; i < 16; i++) {
		int distance = GetDistance(towner[i]._t, 2);
		if (distance == 0)
			continue;
		pcursmonst = i;
	}
}

bool HasRangedSpell()
{
	int spl = myplr().data._pRSpell;

	return spl != SPL_INVALID
	    && spl != SPL_TOWN
	    && spl != SPL_TELEPORT
	    && spelldata[spl].sTargeted
	    && !spelldata[spl].sTownSpell;
}

bool CanTargetMonster(int mi)
{
	const MonsterStruct &monst = monsters[mi].data;

	if (monst._mFlags & (MFLAG_HIDDEN | MFLAG_GOLEM))
		return false;
	if (monst._mhitpoints >> 6 <= 0) // dead
		return false;

	const V2Di m = monst._m;
	if (!(grid.at(m).dFlags & BFLAG_LIT)) // not visable
		return false;
	if (grid.at(m).getActor() == 0)
		return false;

	return true;
}

void FindRangedTarget()
{
	int distance, rotations;
	bool canTalk;

	// The first MAX_PLRS monsters are reserved for players' golems.
	for (int mi = MAX_PLRS; mi < MAXMONSTERS; mi++) {
		const MonsterStruct &monst = monsters[mi].data;
		const V2Di m = monst._mfut;
		if (!CanTargetMonster(mi)) continue;

		const bool newCanTalk = CanTalkToMonst(mi);
		if (pcursmonst != -1 && !canTalk && newCanTalk)
			continue;
		const int newDdistance = GetDistanceRanged(m);
		const int newRotations = GetRotaryDistance(m);
		if (pcursmonst != -1 && canTalk == newCanTalk) {
			if (distance < newDdistance) continue;
			if (distance == newDdistance && rotations < newRotations) continue;
		}
		distance = newDdistance;
		rotations = newRotations;
		canTalk = newCanTalk;
		pcursmonst = mi;
	}
}

void FindMeleeTarget()
{
	bool visited[MAXDUNX][MAXDUNY] = { { 0 } };
	int maxSteps = 25; // Max steps for FindPath is 25
	int rotations;
	bool canTalk;

	struct SearchNode {
		V2Di p;
		int steps;
	};
	std::list<SearchNode> queue;

	{
		const V2Di start = myplr().futpos();
		visited[start.x][start.y] = true;
		queue.push_back({ start, 0 });
	}

	while (!queue.empty()) {
		SearchNode node = queue.front();
		queue.pop_front();

		for (int i = 0; i < 8; i++) {
			V2Di d = node.p + offset(Dir(i));

			if (visited[d.x][d.y])
				continue; // already visisted

			if (node.steps > maxSteps) {
				visited[d.x][d.y] = true;
				continue;
			}

			if (!PosOkPlayer(myplr(), d)) {
				visited[d.x][d.y] = true;

				const int m = grid.at(d).getActor();
				if (m != 0) {
					const int mi = m > 0 ? m - 1 : -(m + 1);
					if (CanTargetMonster(mi)) {
						const bool newCanTalk = CanTalkToMonst(mi);
						if (pcursmonst != -1 && !canTalk && newCanTalk)
							continue;
						const int newRotations = GetRotaryDistance(d);
						if (pcursmonst != -1 && canTalk == newCanTalk && rotations < newRotations)
							continue;
						rotations = newRotations;
						canTalk = newCanTalk;
						pcursmonst = mi;
						if (!canTalk) maxSteps = node.steps; // Monsters found, cap search to current steps
					}
				}

				continue;
			}

			//PATHNODE pPath;
			//pPath.pos = node.p;

			//if (path_solid_pieces(&pPath, d)) {
			//	queue.push_back({ d, node.steps + 1 });
			//	visited[d.x][d.y] = true;
			//}
		}
	}
}

void CheckMonstersNearby()
{
	if (myplr().data._pwtype == WT_RANGED || HasRangedSpell()) {
		FindRangedTarget();
		return;
	}

	FindMeleeTarget();
}

void CheckPlayerNearby()
{
	int distance, newDdistance, rotations;

	if (pcursmonst != -1)
		return;

	int spl = myplr().data._pRSpell;
	if (FriendlyMode && spl != SPL_RESURRECT && spl != SPL_HEALOTHER)
		return;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (i == myplr()) continue;
		const V2Di m = plr[i].futpos();
		if (grid.at(m).isPlayer()
		    || !(grid.at(m).dFlags & BFLAG_LIT)
		    || (plr[i].data._pHitPoints == 0 && spl != SPL_RESURRECT))
			continue;

		if (myplr().data._pwtype == WT_RANGED || HasRangedSpell() || spl == SPL_HEALOTHER) {
			newDdistance = GetDistanceRanged(m);
		} else {
			newDdistance = GetDistance(m, distance);
			if (newDdistance == 0) continue;
		}

		if (pcursplr != -1 && distance < newDdistance) continue;
		const int newRotations = GetRotaryDistance(m);
		if (pcursplr != -1 && distance == newDdistance && rotations < newRotations)
			continue;

		distance = newDdistance;
		rotations = newRotations;
		pcursplr = i;
	}
}

void FindActor()
{
	if (lvl.type() != DunType::town) {
		CheckMonstersNearby();
	} else {
		CheckTownersNearby();
	}

	if (plr.isMultiplayer()) CheckPlayerNearby();
}

int pcursmissile;
int pcurstrig;
int pcursquest;

void FindTrigger()
{
	int distance, rotations;

	if (pcursitem != -1 || pcursobj != -1)
		return; // Prefer showing items/objects over triggers (use of cursm* conflicts)

	for (int i = 0; i < nummissiles; i++) {
		int mi = missileactive[i];
		if (missile[mi]._mitype == MIS_TOWN || missile[mi]._mitype == MIS_RPORTAL) {

			V2Di miv = missile[mi]._mi;
			const int newDdistance = GetDistance(miv, 2);
			if (newDdistance == 0)
				continue;
			if (pcursmissile != -1 && distance < newDdistance)
				continue;
			const int newRotations = GetRotaryDistance(miv);
			if (pcursmissile != -1 && distance == newDdistance && rotations < newRotations)
				continue;
			cursm = miv;
			pcursmissile = mi;
			distance = newDdistance;
			rotations = newRotations;
		}
	}

	if (pcursmissile == -1) {
		for (int i = 0; i < numtrigs; i++) {
			V2Di t = trigs[i]._t;
			if (trigs[i]._tlvl == 13)
				t.y -= 1;
			const int newDdistance = GetDistance(t, 2);
			if (newDdistance == 0)
				continue;
			cursm = t;
			pcurstrig = i;
		}

		if (pcurstrig == -1) {
			for (int i = 0; i < MAXQUESTS; i++) {
				if (i == Q_BETRAYER || lvl.currlevel != quests[i]._qlevel || quests[i]._qslvl == SetLvl::None)
					continue;
				const int newDdistance = GetDistance(quests[i]._qt, 2);
				if (newDdistance == 0)
					continue;
				cursm = quests[i]._qt;
				pcursquest = i;
			}
		}
	}

	if (pcursmonst != -1 || pcursplr != -1 || cursm.x == -1 || cursm.y == -1)
		return; // Prefer monster/player info text

	CheckTrigForce();
	CheckTown();
	CheckRportal();
}

void Interact()
{
	if (lvl.type() == DunType::town && pcursmonst != -1) {
		NetSendCmdLocParam1(true, CMD_TALKXY, towner[pcursmonst]._t, pcursmonst);
	} else if (pcursmonst != -1) {
		if (myplr().data._pwtype != WT_RANGED || CanTalkToMonst(pcursmonst)) {
			NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
		} else {
			NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
		}
	} else if (lvl.type() != DunType::town && pcursplr != -1 && !FriendlyMode) {
		NetSendCmdParam1(true, myplr().data._pwtype == WT_RANGED ? CMD_RATTACKPID : CMD_ATTACKPID, pcursplr);
	}
}

void AttrIncBtnSnap(MoveDirectionY dir)
{
	if (dir == MoveDirectionY_NONE)
		return;

	if (chrbtnactive && myplr().data._pStatPts <= 0)
		return;

	DWORD ticks = SDL_GetTicks();
	if (ticks - invmove < repeatRate) {
		return;
	}
	invmove = ticks;

	// first, find our cursor location
	int slot = 0;
	for (int i = 0; i < 4; i++) {
		if (Mouse.x >= ChrBtnsRect[i].x
		    && Mouse.x <= ChrBtnsRect[i].x + ChrBtnsRect[i].w
		    && Mouse.y >= ChrBtnsRect[i].y
		    && Mouse.y <= ChrBtnsRect[i].h + ChrBtnsRect[i].y) {
			slot = i;
			break;
		}
	}

	if (dir == MoveDirectionY_UP) {
		if (slot > 0)
			--slot;
	} else if (dir == MoveDirectionY_DOWN) {
		if (slot < 3)
			++slot;
	}

	// move cursor to our new location
	int x = ChrBtnsRect[slot].x + (ChrBtnsRect[slot].w / 2);
	int y = ChrBtnsRect[slot].y + (ChrBtnsRect[slot].h / 2);
	SetCursorPos(x, y);
}

// move the cursor around in our inventory
// if mouse coords are at SLOTXY_CHEST_LAST, consider this center of equipment
// small inventory squares are 29x29 (roughly)
void InvMove(MoveDirection dir)
{
	DWORD ticks = SDL_GetTicks();
	if (ticks - invmove < repeatRate) {
		return;
	}
	invmove = ticks;
	int x = Mouse.x;
	int y = Mouse.y;

	// check which inventory rectangle the mouse is in, if any
	for (int r = 0; (DWORD)r < NUM_XY_SLOTS; r++) {
		if (x >= InvRect[r].X && x < InvRect[r].X + (INV_SLOT_SIZE_PX + 1) && y >= InvRect[r].Y - (INV_SLOT_SIZE_PX + 1) && y < InvRect[r].Y) {
			slot = r;
			break;
		}
	}

	if (slot < 0)
		slot = 0;
	if (slot > SLOTXY_BELT_LAST)
		slot = SLOTXY_BELT_LAST;

	// when item is on cursor, this is the real cursor XY
	if (dir.x == MoveDirectionX_LEFT) {
		if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) {
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) {
			x = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_AMULET) {
			x = InvRect[SLOTXY_HEAD_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HEAD_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_RIGHT) {
			x = InvRect[SLOTXY_RING_LEFT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_LEFT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_BELT_FIRST) {
			// do nothing
		} else if (slot == SLOTXY_RING_LEFT) {                                        // left ring
			                                                                          // do nothing
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) { // left hand
			                                                                          // do nothing
		} else if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) {           // head
			                                                                          // do nothing
		} else if (slot > SLOTXY_INV_FIRST) {                                         // general inventory
			if (slot != SLOTXY_INV_FIRST && slot != 35 && slot != 45 && slot != 55) { // left bounds
				slot -= 1;
				x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
				y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
			}
		}
	} else if (dir.x == MoveDirectionX_RIGHT) {
		if (slot == SLOTXY_RING_LEFT) {
			x = InvRect[SLOTXY_RING_RIGHT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_RIGHT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) {
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) {
			x = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) { // head to amulet
			x = InvRect[SLOTXY_AMULET].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_AMULET].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) { // right hand
			                                                                            // do nothing
		} else if (slot == SLOTXY_AMULET) {
			// do nothing
		} else if (slot == SLOTXY_RING_RIGHT) {
			// do nothing
		} else if (slot < SLOTXY_BELT_LAST && slot >= SLOTXY_INV_FIRST) {            // general inventory
			if (slot != 34 && slot != 44 && slot != 54 && slot != SLOTXY_INV_LAST) { // right bounds
				slot += 1;
				x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
				y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
			}
		}
	}
	if (dir.y == MoveDirectionY_UP) {
		if (slot > 24 && slot <= 27) { // first 3 general slots
			x = InvRect[SLOTXY_RING_LEFT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_LEFT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= 28 && slot <= 32) { // middle 4 general slots
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= 33 && slot < 35) { // last 3 general slots
			x = InvRect[SLOTXY_RING_RIGHT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_RIGHT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) { // chest to head
			x = InvRect[SLOTXY_HEAD_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HEAD_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_LEFT) { // left ring to left hand
			x = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_RIGHT) { // right ring to right hand
			x = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) { // right hand to amulet
			x = InvRect[SLOTXY_AMULET].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_AMULET].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) {
			// do nothing
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) { // left hand to head
			x = InvRect[SLOTXY_HEAD_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HEAD_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_AMULET) {
			// do nothing
		} else if (slot >= (SLOTXY_INV_FIRST + 10)) { // general inventory
			slot -= 10;
			x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
		}
	} else if (dir.y == MoveDirectionY_DOWN) {
		if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) {
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) {
			x = InvRect[30].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[30].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) {
			x = InvRect[SLOTXY_RING_LEFT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_LEFT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_LEFT) {
			x = InvRect[26].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[26].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_RIGHT) {
			x = InvRect[34].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[34].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_AMULET) {
			x = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) {
			x = InvRect[SLOTXY_RING_RIGHT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_RIGHT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot < (SLOTXY_BELT_LAST - 10)) { // general inventory
			slot += 10;
			x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
		}
	}

	if (x == Mouse.x && y == Mouse.y) {
		return; // Avoid wobeling when scalled
	}

	if (pcurs > 1) {       // [3] Keep item in the same slot, don't jump it up
		if (x != Mouse.x) { // without this, the cursor keeps moving -10
			x -= 10;
			y -= 10;
		}
	}
	SetCursorPos(x, y);
}

// check if hot spell at X Y exists
bool HSExists(int x, int y)
{
	for (int r = 0; r < speedspellcount; r++) { // speedbook cells are 56x56
		if (Mouse.x >= speedspellscoords[r].x - 28 && Mouse.x < speedspellscoords[r].x + (28) && Mouse.y >= speedspellscoords[r].y - (28) && Mouse.y < speedspellscoords[r].y + 28) {
			return true;
		}
	}
	return false;
}

void HotSpellMove(MoveDirection dir)
{
	int x = 0;
	int y = 0;

	DWORD ticks = SDL_GetTicks();
	if (ticks - invmove < repeatRate) {
		return;
	}
	invmove = ticks;

	for (int r = 0; r < speedspellcount; r++) { // speedbook cells are 56x56
		// our 3 rows by y axis
		if (speedspellscoords[r].y == 307)
			hsr[0]++;
		if (speedspellscoords[r].y == 251)
			hsr[1]++;
		if (speedspellscoords[r].y == 195)
			hsr[2]++;
		if (Mouse.x >= speedspellscoords[r].x - 28 && Mouse.x < speedspellscoords[r].x + (28) && Mouse.y >= speedspellscoords[r].y - (28) && Mouse.y < speedspellscoords[r].y + 28) {
			spbslot = r;
			//sprintf(tempstr, "IN HOT SPELL CELL NUM:%i", r);
			//NetSendCmdString(1 << myplr(), tempstr);
		}
	}

	if (dir.y == MoveDirectionY_UP) {
		if (speedspellscoords[spbslot].y == 307 && hsr[1] > 0) { // we're in row 1, check if row 2 has spells
			if (HSExists(Mouse.x, 251)) {
				x = Mouse.x;
				y = 251;
			}
		} else if (speedspellscoords[spbslot].y == 251 && hsr[2] > 0) { // we're in row 2, check if row 3 has spells
			if (HSExists(Mouse.x, 195)) {
				x = Mouse.x;
				y = 195;
			}
		}
	} else if (dir.y == MoveDirectionY_DOWN) {
		if (speedspellscoords[spbslot].y == 251) { // we're in row 2
			if (HSExists(Mouse.x, 307)) {
				x = Mouse.x;
				y = 307;
			}
		} else if (speedspellscoords[spbslot].y == 195) { // we're in row 3
			if (HSExists(Mouse.x, 251)) {
				x = Mouse.x;
				y = 251;
			}
		}
	}
	if (dir.x == MoveDirectionX_LEFT) {
		if (spbslot >= speedspellcount - 1)
			return;
		spbslot++;
		x = speedspellscoords[spbslot].x;
		y = speedspellscoords[spbslot].y;
	} else if (dir.x == MoveDirectionX_RIGHT) {
		if (spbslot <= 0)
			return;
		spbslot--;
		x = speedspellscoords[spbslot].x;
		y = speedspellscoords[spbslot].y;
	}

	if (x > 0 && y > 0) {
		SetCursorPos(x, y);
	}
}

void SpellBookMove(MoveDirection dir)
{
	DWORD ticks = SDL_GetTicks();
	if (ticks - invmove < repeatRate) {
		return;
	}
	invmove = ticks;

	if (dir.x == MoveDirectionX_LEFT) {
		if (sbooktab > 0)
			sbooktab--;
	} else if (dir.x == MoveDirectionX_RIGHT) {
		if (sbooktab < 3)
			sbooktab++;
	}
}

static const Dir kFaceDir[3][3] = {
	// NONE      UP      DOWN
	{ Dir::OMNI, Dir::N, Dir::S }, // NONE
	{ Dir::W, Dir::NW, Dir::SW },  // LEFT
	{ Dir::E, Dir::NE, Dir::SE },  // RIGHT
};
static const V2Di kOffsets[8] = {
	{ 1, 1 },   // Dir::S
	{ 0, 1 },   // Dir::SW
	{ -1, 1 },  // Dir::W
	{ -1, 0 },  // Dir::NW
	{ -1, -1 }, // Dir::N
	{ 0, -1 },  // Dir::NE
	{ 1, -1 },  // Dir::E
	{ 1, 0 },   // Dir::SE
};

/**
 * @brief check if stepping in direction (dir) from x, y is blocked.
 *
 * If you step from A to B, at leat one of the Xs need to be clear:
 *
 *  AX
 *  XB
 *
 *  @return true if step is blocked
 */
bool IsPathBlocked(V2Di pos, Dir dir)
{
	Dir d1, d2;

	switch (dir) {
	case Dir::N:
		d1 = Dir::NW;
		d2 = Dir::NE;
		break;
	case Dir::E:
		d1 = Dir::NE;
		d2 = Dir::SE;
		break;
	case Dir::S:
		d1 = Dir::SE;
		d2 = Dir::SW;
		break;
	case Dir::W:
		d1 = Dir::SW;
		d2 = Dir::NW;
		break;
	default:
		return false;
	}

	V2Di d1v = kOffsets[int(d1)];
	V2Di d2v = kOffsets[int(d2)];

	if (!grid.at(d1v).isSolid() && !grid.at(d2v).isSolid())
		return false;

	return !PosOkPlayer(myplr(), d1v) && !PosOkPlayer(myplr(), d2v);
}

void WalkInDir(MoveDirection dir)
{
	V2Di pos = myplr().futpos();

	if (dir.x == MoveDirectionX_NONE && dir.y == MoveDirectionY_NONE) {
		if (sgbControllerActive && !myplr().data.wkpath.empty() && myplr().data.destAction == ACTION_NONE)
			NetSendCmdLoc(true, CMD_WALKXY, pos); // Stop walking
		return;
	}

	const Dir pdir = kFaceDir[static_cast<std::size_t>(dir.x)][static_cast<std::size_t>(dir.y)];
	const V2Di dv = pos + kOffsets[int(pdir)];
	myplr().data._pdir = pdir;

	if (PosOkPlayer(myplr(), dv) && IsPathBlocked(pos, pdir))
		return; // Don't start backtrack around obstacles

	NetSendCmdLoc(true, CMD_WALKXY, dv);
}

void Movement()
{
	if (InGameMenu() || questlog
	    || IsControllerButtonPressed(ControllerButton_BUTTON_START)
	    || IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
		return;

	MoveDirection move_dir = GetMoveDirection();
	if (move_dir.x != MoveDirectionX_NONE || move_dir.y != MoveDirectionY_NONE) {
		sgbControllerActive = true;
	}

	if (invflag) {
		InvMove(move_dir);
	} else if (chrflag && myplr().data._pStatPts > 0) {
		AttrIncBtnSnap(move_dir.y);
	} else if (spselflag) {
		HotSpellMove(move_dir);
	} else if (sbookflag) {
		SpellBookMove(move_dir);
	} else {
		WalkInDir(move_dir);
	}
}

struct RightStickAccumulator {

	RightStickAccumulator()
	{
		lastTc = SDL_GetTicks();
		hiresDX = 0;
		hiresDY = 0;
	}

	void pool(int *x, int *y, int slowdown)
	{
		DWORD tc = SDL_GetTicks();
		hiresDX += rightStickX * (tc - lastTc);
		hiresDY += rightStickY * (tc - lastTc);
		*x += hiresDX / slowdown;
		*y += -hiresDY / slowdown;
		lastTc = tc;
		// keep track of remainder for sub-pixel motion
		hiresDX %= slowdown;
		hiresDY %= slowdown;
	}

	void clear()
	{
		lastTc = SDL_GetTicks();
	}

	DWORD lastTc;
	int hiresDX;
	int hiresDY;
};

} // namespace

bool IsAutomapActive()
{
	return automap.enabled() && lvl.type() != DunType::town;
}

void HandleRightStickMotion()
{
	static RightStickAccumulator acc;
	// deadzone is handled in ScaleJoystickAxes() already
	if (rightStickX == 0 && rightStickY == 0) {
		acc.clear();
		return;
	}

	if (IsAutomapActive()) { // move map
		int dx = 0, dy = 0;
		acc.pool(&dx, &dy, 32);
		automap.addOffsetX(dy + dx);
		automap.addOffsetY(dy - dx);
		return;
	}

	{ // move cursor
		sgbControllerActive = false;
		int x = Mouse.x;
		int y = Mouse.y;
		acc.pool(&x, &y, 2);
		x = std::min(std::max(x, 0), SCREEN_WIDTH - 1);
		y = std::min(std::max(y, 0), SCREEN_HEIGHT - 1);
		SetCursorPos(x, y);
	}
}

/**
 * @brief Moves the mouse to the first inventory slot.
 */
void FocusOnInventory()
{
	SetCursorPos(InvRect[25].X + (INV_SLOT_SIZE_PX / 2), InvRect[25].Y - (INV_SLOT_SIZE_PX / 2));
}

void plrctrls_after_check_curs_move()
{
	HandleRightStickMotion();

	// check for monsters first, then items, then towners.
	if (sgbControllerActive) {
		// Clear focuse set by cursor
		pcursplr = -1;
		pcursmonst = -1;
		pcursitem = -1;
		pcursobj = -1;
		pcursmissile = -1;
		pcurstrig = -1;
		pcursquest = -1;
		cursm = { -1, -1 };
		if (!invflag) {
			*infostr = '\0';
			ClearPanel();
			FindActor();
			FindItemOrObject();
			FindTrigger();
		}
	}
}

void plrctrls_after_game_logic()
{
	Movement();
}

void UseBeltItem(int type)
{
	for (int i = 0; i < MAXBELTITEMS; i++) {
		const int id = AllItemsList[myplr().data.SpdList[i].IDidx].iMiscId;
		const int spellId = AllItemsList[myplr().data.SpdList[i].IDidx].iSpell;
		if ((type == BLT_HEALING && (id == IMISC_HEAL || id == IMISC_FULLHEAL || (id == IMISC_SCROLL && spellId == SPL_HEAL)))
		    || (type == BLT_MANA && (id == IMISC_MANA || id == IMISC_FULLMANA))
		    || id == IMISC_REJUV || id == IMISC_FULLREJUV) {
			if (myplr().data.SpdList[i]._itype > -1) {
				myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + i);
				break;
			}
		}
	}
}

void PerformPrimaryAction()
{
	if (invflag) { // inventory is open
		if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
			TryIconCurs();
			SetCursor_(CURSOR_HAND);
		} else {
			CheckInvItem();
		}
		return;
	}

	if (spselflag) {
		SetSpell();
		return;
	}

	if (chrflag && !chrbtnactive && myplr().data._pStatPts > 0) {
		CheckChrBtns();
		for (int i = 0; i < 4; i++) {
			if (Mouse.x >= ChrBtnsRect[i].x
			    && Mouse.x <= ChrBtnsRect[i].x + ChrBtnsRect[i].w
			    && Mouse.y >= ChrBtnsRect[i].y
			    && Mouse.y <= ChrBtnsRect[i].h + ChrBtnsRect[i].y) {
				chrbtn[i] = 1;
				chrbtnactive = true;
				ReleaseChrBtns();
			}
		}
		return;
	}

	Interact();
}

bool SpellHasActorTarget()
{
	int spl = myplr().data._pRSpell;
	if (spl == SPL_TOWN || spl == SPL_TELEPORT)
		return false;

	if (spl == SPL_FIREWALL && pcursmonst != -1) {
		cursm = monsters[pcursmonst].data._m;
	}

	return pcursplr != -1 || pcursmonst != -1;
}

void UpdateSpellTarget()
{
	if (SpellHasActorTarget()) return;
	pcursplr = -1;
	pcursmonst = -1;
	int range = 1;
	if (myplr().data._pRSpell == SPL_TELEPORT) range = 4;
	cursm = myplr().futpos() + kOffsets[int(myplr().data._pdir)] * range;
}

/**
 * @brief Try dropping item in all 9 possible places
 */
bool TryDropItem()
{
	cursm.x = myplr().futpos().x + 1;
	cursm.y = myplr().futpos().y;
	if (!DropItemBeforeTrig()) {
		// Try to drop on the other side
		cursm.x = myplr().futpos().x;
		cursm.y = myplr().futpos().y + 1;
		DropItemBeforeTrig();
	}
	return pcurs == CURSOR_HAND;
}

void PerformSpellAction()
{
	if (InGameMenu() || questlog || sbookflag)
		return;

	if (invflag) {
		if (pcurs >= CURSOR_FIRSTITEM)
			TryDropItem();
		else if (pcurs > CURSOR_HAND) {
			TryIconCurs();
			SetCursor_(CURSOR_HAND);
		}
		return;
	}

	if (pcurs >= CURSOR_FIRSTITEM && !TryDropItem())
		return;
	if (pcurs > CURSOR_HAND)
		SetCursor_(CURSOR_HAND);

	if (spselflag) {
		SetSpell();
		return;
	}

	int spl = myplr().data._pRSpell;
	if ((pcursplr == -1 && (spl == SPL_RESURRECT || spl == SPL_HEALOTHER))
	    || (pcursobj == -1 && spl == SPL_DISARM)) {
		if (myplr().data._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR27);
		} else if (myplr().data._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE27);
		} else if (myplr().data._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE27);
		}
		return;
	}

	UpdateSpellTarget();
	CheckPlrSpell();
}

void CtrlUseInvItem()
{
	ItemStruct *Item;

	if (pcursinvitem == -1)
		return;

	if (pcursinvitem <= INVITEM_INV_LAST)
		Item = &myplr().data.InvList[pcursinvitem - INVITEM_INV_FIRST];
	else
		Item = &myplr().data.SpdList[pcursinvitem - INVITEM_BELT_FIRST];

	if ((Item->_iMiscId == IMISC_SCROLLT || Item->_iMiscId == IMISC_SCROLL) && spelldata[Item->_iSpell].sTargeted) {
		return;
	}

	myplr().inventory.UseInvItem(pcursinvitem);
}

void PerformSecondaryAction()
{
	if (invflag) {
		CtrlUseInvItem();
		return;
	}

	if (pcurs >= CURSOR_FIRSTITEM && !TryDropItem())
		return;
	if (pcurs > CURSOR_HAND)
		SetCursor_(CURSOR_HAND);

	if (pcursitem != -1) {
		NetSendCmdLocParam1(true, CMD_GOTOAGETITEM, cursm, pcursitem);
	} else if (pcursobj != -1) {
		NetSendCmdLocParam1(true, CMD_OPOBJXY, cursm, pcursobj);
	} else if (pcursmissile != -1) {
		myplr().MakePlrPath(missile[pcursmissile]._mi, true);
		myplr().data.destAction = ACTION_WALK;
	} else if (pcurstrig != -1) {
		myplr().MakePlrPath(trigs[pcurstrig]._t, true);
		myplr().data.destAction = ACTION_WALK;
	} else if (pcursquest != -1) {
		myplr().MakePlrPath(quests[pcursquest]._qt, true);
		myplr().data.destAction = ACTION_WALK;
	}
}

} // namespace dvl
