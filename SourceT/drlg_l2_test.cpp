#include <gtest/gtest.h>
#include "all.h"

TEST(Drlg_l2, InitDungeon) {
	dvl::InitDungeon();
	EXPECT_EQ(dvl::predungeon[0][0], 32);
	EXPECT_EQ(dvl::dgrid[0][0].dflags, 0);
}
