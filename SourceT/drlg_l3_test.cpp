#include <gtest/gtest.h>
#include "all.h"

TEST(Drlg_l3, AddFenceDoors_x) {
	memset(dvl::dungeon, 0, sizeof(dvl::dungeon));
	dvl::dgrid[5][5].dungeon = 7;
	dvl::dgrid[5 - 1][5].dungeon = 130;
	dvl::dgrid[5 + 1][5].dungeon = 152;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dgrid[5][5].dungeon, 146);
}

TEST(Drlg_l3, AddFenceDoors_y) {
	memset(dvl::dungeon, 0, sizeof(dvl::dungeon));
	dvl::dgrid[5][5].dungeon = 7;
	dvl::dgrid[5][5 - 1].dungeon = 130;
	dvl::dgrid[5][5 + 1].dungeon = 152;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dgrid[5][5].dungeon, 147);
}

TEST(Drlg_l3, AddFenceDoors_no) {
	memset(dvl::dungeon, 0, sizeof(dvl::dungeon));
	dvl::dgrid[5][5].dungeon = 7;
	dvl::dgrid[5 - 1][5].dungeon = 130;
	dvl::dgrid[5 + 1][5].dungeon = 153;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dgrid[5][5].dungeon, 7);
}
