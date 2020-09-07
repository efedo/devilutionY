#include <gtest/gtest.h>
#include "all.h"

TEST(Dead, AddDead) {
	dvl::AddDead(21, 48, 8, dvl::DIR_W);
	EXPECT_EQ(dvl::grid[21][48].dDead, 8 + (dvl::DIR_W << 5));
}

TEST(Dead, AddDead_OOB) {
	dvl::AddDead(21, 48, MAXDEAD + 1, dvl::DIR_W);
	EXPECT_EQ(dvl::grid[21][48].dDead, 0 + (dvl::DIR_W << 5));
}
