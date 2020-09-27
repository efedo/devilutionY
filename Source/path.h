///**
// * @file path.h
// *
// * Interface of the path finding algorithms.
// */
//#ifndef __PATH_H__
//#define __PATH_H__
//
//DEVILUTION_BEGIN_NAMESPACE
//
//extern std::array<PATHNODE, MAXPATHNODES> path_nodes;
//
//extern int gdwCurPathStep;
//extern int gdwCurNodes;
//extern int pnode_vals[MAX_PATH_LENGTH];
//extern PATHNODE *pnode_ptr;
//extern PATHNODE *pnode_tblptr[MAXPATHNODES];
//extern PATHNODE *path_2_nodes;
//extern PATHNODE path_unusednodes[MAXPATHNODES];
//
//int FindPath(bool (*PosOk)(int, V2Di), int PosOkArg, V2Di s, V2Di d, char *path); // * ext
//
//int path_get_h_cost(V2Di s, V2Di d);
//int path_check_equal(PATHNODE *pPath, V2Di d);
//PATHNODE *GetNextPath();
//bool path_solid_pieces(PATHNODE *pPath, V2Di d);
//bool path_get_path(bool (*PosOk)(int, V2Di), int PosOkArg, PATHNODE *pPath, V2Di pos);
//bool path_parent_path(PATHNODE *pPath, V2Di d, V2Di s);
//PATHNODE *path_get_node1(V2Di d);
//PATHNODE *path_get_node2(V2Di d);
//void path_next_node(PATHNODE *pPath);
//void path_set_coords(PATHNODE *pPath);
//void path_push_active_step(PATHNODE *pPath);
//PATHNODE *path_pop_active_step();
//PATHNODE *path_new_step();
//
///* rdata */

////extern const char pathxdir[8];
////extern const char pathydir[8];
//
///* data */
//extern char path_directions[9];
//
//DEVILUTION_END_NAMESPACE
//
//#endif /* __PATH_H__ */
