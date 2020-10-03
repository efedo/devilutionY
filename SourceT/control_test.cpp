#include <gtest/gtest.h>
#include "all.h"

TEST(Control, SetSpell) {
	dvl::pnumlines = 1;
	dvl::pinfoflag = true;
	dvl::pSpell = dvl::SpellId::FIREBOLT;
	dvl::pSplType = dvl::RSpellType::CHARGES;
	dvl::SetSpell();
	EXPECT_EQ(dvl::spselflag, false);
	EXPECT_EQ(dvl::plr[dvl::myplr()]._pRSpell, dvl::SpellId::FIREBOLT);
	EXPECT_EQ(dvl::plr[dvl::myplr()]._pRSplType, dvl::RSpellType::CHARGES);
	EXPECT_EQ(dvl::pnumlines, 0);
	EXPECT_EQ(dvl::pinfoflag, false);
	EXPECT_EQ(dvl::force_redraw, 255);
}

TEST(Control, ClearPanel) {
	dvl::pnumlines = 1;
	dvl::pinfoflag = true;
	dvl::ClearPanel();
	EXPECT_EQ(dvl::pnumlines, 0);
	EXPECT_EQ(dvl::pinfoflag, false);
}
