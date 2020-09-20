///**
// * @file path.cpp
// *
// * Implementation of the path finding algorithms.
// */
//#include "all.h"
//#include <array>
//
//
//DEVILUTION_BEGIN_NAMESPACE
///** Notes visisted by the path finding algorithm. */
//std::array <PATHNODE, MAXPATHNODES> path_nodes;
///** size of the pnode_tblptr stack */
//int gdwCurPathStep;
///** the number of in-use nodes in path_nodes */
//int gdwCurNodes;
///**
// * for reconstructing the path after the A* search is done. The longest
// * possible path is actually 24 steps, even though we can fit 25
// */
//int pnode_vals[MAX_PATH_LENGTH];
///** A linked list of all visited nodes */
//PATHNODE *pnode_ptr;
///** A stack for recursively searching nodes */
//PATHNODE *pnode_tblptr[MAXPATHNODES];
///** A linked list of the A* frontier, sorted by distance */
//PATHNODE *path_2_nodes;
//PATHNODE path_unusednodes[MAXPATHNODES];
//
///** For iterating over the 8 possible movement directions */
//const V2Di pathdir[8] = {
//	{ -1, -1 },
//	{ -1,  1 },
//	{  1, -1 },
//	{  1,  1 },
//	{ -1,  0 },
//	{  0, -1 },
//	{  1,  0 },
//	{  0,  1 }
//};
//
//
///* data */
//
///**
// * each step direction is assigned a number like this:
// *       dx
// *     -1 0 1
// *     +-----
// *   -1|5 1 6
// * dy 0|2 0 3
// *    1|8 4 7
// */
//char path_directions[9] = { 5, 1, 6, 2, 0, 3, 8, 4, 7 };
//
///**
// * find the shortest path from (sx,sy) to (dx,dy), using PosOk(PosOkArg,x,y) to
// * check that each step is a valid position. Store the step directions (see
// * path_directions) in path, which must have room for 24 steps
// */
//int FindPath(BOOL (*PosOk)(int, V2Di), int PosOkArg, V2Di s, V2Di d, char *path)
//{
//	PATHNODE *path_start, *next_node, *current;
//	int path_length, i;
//
//	// clear all nodes, create root nodes for the visited/frontier linked lists
//	gdwCurNodes = 0;
//	path_2_nodes = path_new_step();
//	pnode_ptr = path_new_step();
//	gdwCurPathStep = 0;
//	path_start = path_new_step();
//	path_start->g = 0;
//	path_start->h = path_get_h_cost(s, d);
//	path_start->pos = s;
//	path_start->f = path_start->h + path_start->g;
//	path_2_nodes->NextNode = path_start;
//	// A* search until we find (dx,dy) or fail
//	while ((next_node = GetNextPath())) {
//		// reached the end, success!
//		if (next_node->pos == d) {
//			current = next_node;
//			path_length = 0;
//			while (current->Parent) {
//				if (path_length >= MAX_PATH_LENGTH)
//					break;
//				pnode_vals[path_length++] = path_directions[3 * (current->pos.y - current->Parent->pos.y) - current->Parent->pos.x + 4 + current->pos.x];
//				current = current->Parent;
//			}
//			if (path_length != MAX_PATH_LENGTH) {
//				for (i = 0; i < path_length; i++)
//					path[i] = pnode_vals[path_length - i - 1];
//				return i;
//			}
//			return 0;
//		}
//		// ran out of nodes, abort!
//		if (!path_get_path(PosOk, PosOkArg, next_node, d))
//			return 0;
//	}
//	// frontier is empty, no path!
//	return 0;
//}
//
///**
// * @brief heuristic, estimated cost from (sx,sy) to (dx,dy)
// */
//int path_get_h_cost(V2Di s, V2Di d)
//{
//	int delta_x = abs(s.x - d.x);
//	int delta_y = abs(s.y - d.y);
//	int min = delta_x < delta_y ? delta_x : delta_y;
//	int max = delta_x > delta_y ? delta_x : delta_y;
//	return 2 * (min + max); // see path_check_equal for why this is times 2
//}
//
///**
// * @brief return 2 if pPath is horizontally/vertically aligned with (dx,dy), else 3
// *
// * This approximates that diagonal movement on a square grid should have a cost
// * of sqrt(2). That's approximately 1.5, so they multiply all step costs by 2,
// * except diagonal steps which are times 3
// */
//int path_check_equal(PATHNODE *pPath, V2Di d)
//{
//	if (pPath->pos.x == d.x || pPath->pos.y == d.y) return 2;
//	return 3;
//}
//
///**
// * @brief get the next node on the A* frontier to explore (estimated to be closest to the goal), mark it as visited, and return it
// */
//PATHNODE *GetNextPath()
//{
//	PATHNODE *result;
//	result = path_2_nodes->NextNode;
//	if (result == NULL) return result;
//	path_2_nodes->NextNode = result->NextNode;
//	result->NextNode = pnode_ptr->NextNode;
//	pnode_ptr->NextNode = result;
//	return result;
//}
//
///**
// * @brief check if stepping from pPath to (dx,dy) cuts a corner.
// *
// * If you step from A to B, both Xs need to be clear:
// *
// *  AX
// *  XB
// *
// *  @return true if step is allowed
// */
//BOOL path_solid_pieces(PATHNODE *pPath, V2Di d)
//{
//	int dx = d.x;
//	int dy = d.y;
//	BOOL rv = TRUE;
//	switch (path_directions[3 * (dy - pPath->pos.y) + 3 - pPath->pos.x + 1 + dx]) {
//	case 5:
//		rv = !pieces[grid[dx][dy + 1].dPiece].nSolidTable && !pieces[grid[dx + 1][dy].dPiece].nSolidTable;
//		break;
//	case 6:
//		rv = !pieces[grid[dx][dy + 1].dPiece].nSolidTable && !pieces[grid[dx - 1][dy].dPiece].nSolidTable;
//		break;
//	case 7:
//		rv = !pieces[grid[dx][dy - 1].dPiece].nSolidTable && !pieces[grid[dx - 1][dy].dPiece].nSolidTable;
//		break;
//	case 8:
//		rv = !pieces[grid[dx + 1][dy].dPiece].nSolidTable && !pieces[grid[dx][dy - 1].dPiece].nSolidTable;
//		break;
//	}
//	return rv;
//}
//
///**
// * @brief perform a single step of A* bread-first search by trying to step in every possible direction from pPath with goal (x,y). Check each step with PosOk
// *
// * @return FALSE if we ran out of preallocated nodes to use, else TRUE
// */
//BOOL path_get_path(BOOL (*PosOk)(int, V2Di), int PosOkArg, PATHNODE *pPath, V2Di pos)
//{
//	V2Di d;
//	int i;
//	BOOL ok;
//
//	for (i = 0; i < 8; i++) {
//		d = pPath->pos + pathdir[i];
//		ok = PosOk(PosOkArg, d);
//		if (ok && path_solid_pieces(pPath, d) || !ok && d == pos) {
//			if (!path_parent_path(pPath, d, pos))
//				return FALSE;
//		}
//	}
//	return TRUE;
//}
//
///**
// * @brief add a step from pPath to (dx,dy), return 1 if successful, and update the frontier/visited nodes accordingly
// *
// * @return TRUE if step successfully added, FALSE if we ran out of nodes to use
// */
//BOOL path_parent_path(PATHNODE *pPath, V2Di d, V2Di s)
//{
//	int next_g;
//	PATHNODE *dxdy;
//	int i;
//
//	next_g = pPath->g + path_check_equal(pPath, d);
//
//	// 3 cases to consider
//	// case 1: (dx,dy) is already on the frontier
//	dxdy = path_get_node1(d);
//	if (dxdy != NULL) {
//		for (i = 0; i < 8; i++) {
//			if (pPath->Child[i] == NULL)
//				break;
//		}
//		pPath->Child[i] = dxdy;
//		if (next_g < dxdy->g) {
//			if (path_solid_pieces(pPath, d)) {
//				// we'll explore it later, just update
//				dxdy->Parent = pPath;
//				dxdy->g = next_g;
//				dxdy->f = next_g + dxdy->h;
//			}
//		}
//	} else {
//		// case 2: (dx,dy) was already visited
//		dxdy = path_get_node2(d);
//		if (dxdy != NULL) {
//			for (i = 0; i < 8; i++) {
//				if (pPath->Child[i] == NULL)
//					break;
//			}
//			pPath->Child[i] = dxdy;
//			if (next_g < dxdy->g && path_solid_pieces(pPath, d)) {
//				// update the node
//				dxdy->Parent = pPath;
//				dxdy->g = next_g;
//				dxdy->f = next_g + dxdy->h;
//				// already explored, so re-update others starting from that node
//				path_set_coords(dxdy);
//			}
//		} else {
//			// case 3: (dx,dy) is totally new
//			dxdy = path_new_step();
//			if (dxdy == NULL)
//				return FALSE;
//			dxdy->Parent = pPath;
//			dxdy->g = next_g;
//			dxdy->h = path_get_h_cost(d, s);
//			dxdy->f = next_g + dxdy->h;
//			dxdy->pos = d;
//			// add it to the frontier
//			path_next_node(dxdy);
//
//			for (i = 0; i < 8; i++) {
//				if (pPath->Child[i] == NULL)
//					break;
//			}
//			pPath->Child[i] = dxdy;
//		}
//	}
//	return TRUE;
//}
//
///**
// * @brief return a node for (dx,dy) on the frontier, or NULL if not found
// */
//PATHNODE *path_get_node1(V2Di d)
//{
//	PATHNODE *result = path_2_nodes->NextNode;
//	while (result != NULL && (result->pos != d))
//		result = result->NextNode;
//	return result;
//}
//
///**
// * @brief return a node for (dx,dy) if it was visited, or NULL if not found
// */
//PATHNODE *path_get_node2(V2Di d)
//{
//	PATHNODE *result = pnode_ptr->NextNode;
//	while (result != NULL && (result->pos != d))
//		result = result->NextNode;
//	return result;
//}
//
///**
// * @brief insert pPath into the frontier (keeping the frontier sorted by total distance)
// */
//void path_next_node(PATHNODE *pPath)
//{
//	PATHNODE *next, *current;
//	int f;
//
//	next = path_2_nodes;
//	if (!path_2_nodes->NextNode) {
//		path_2_nodes->NextNode = pPath;
//	} else {
//		current = path_2_nodes;
//		next = path_2_nodes->NextNode;
//		f = pPath->f;
//		while (next && next->f < f) {
//			current = next;
//			next = next->NextNode;
//		}
//		pPath->NextNode = next;
//		current->NextNode = pPath;
//	}
//}
//
///**
// * @brief update all path costs using depth-first search starting at pPath
// */
//void path_set_coords(PATHNODE *pPath)
//{
//	PATHNODE *PathOld;
//	PATHNODE *PathAct;
//	int i;
//
//	path_push_active_step(pPath);
//	while (gdwCurPathStep) {
//		PathOld = path_pop_active_step();
//		for (i = 0; i < 8; i++) {
//			PathAct = PathOld->Child[i];
//			if (PathAct == NULL)
//				break;
//
//			if (PathOld->g + path_check_equal(PathOld, PathAct->pos) < PathAct->g) {
//				if (path_solid_pieces(PathOld, PathAct->pos)) {
//					PathAct->Parent = PathOld;
//					PathAct->g = PathOld->g + path_check_equal(PathOld, PathAct->pos);
//					PathAct->f = PathAct->g + PathAct->h;
//					path_push_active_step(PathAct);
//				}
//			}
//		}
//	}
//}
//
///**
// * @brief push pPath onto the pnode_tblptr stack
// */
//void path_push_active_step(PATHNODE *pPath)
//{
//	int stack_index = gdwCurPathStep;
//	gdwCurPathStep++;
//	pnode_tblptr[stack_index] = pPath;
//}
//
///**
// * @brief pop and return a node from the pnode_tblptr stack
// */
//PATHNODE *path_pop_active_step()
//{
//	gdwCurPathStep--;
//	return pnode_tblptr[gdwCurPathStep];
//}
//
///**
// * @brief zero one of the preallocated nodes and return a pointer to it, or NULL if none are available
// */
//PATHNODE *path_new_step()
//{
//	PATHNODE *new_node;
//
//	if (gdwCurNodes == MAXPATHNODES)
//		return NULL;
//
//	new_node = &path_nodes[gdwCurNodes];
//	gdwCurNodes++;
//	memset(new_node, 0, sizeof(PATHNODE));
//	return new_node;
//}
//
//DEVILUTION_END_NAMESPACE
