#include <gtest/gtest.h>
#include "all.h"

TEST(Automap, init) {
	dvl::initOnce();
	EXPECT_EQ(dvl::automapflag, false);
	EXPECT_EQ(dvl::AutoMapScale, 50);
	EXPECT_EQ(dvl::AmLine64, 32);
	EXPECT_EQ(dvl::AmLine32, 16);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}

TEST(Automap, start) {
	dvl::start();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
	EXPECT_EQ(dvl::automapflag, true);
}

TEST(Automap, up) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::up();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
}

TEST(Automap, down) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::down();
	EXPECT_EQ(dvl::AutoMapXOfs, 2);
	EXPECT_EQ(dvl::AutoMapYOfs, 2);
}

TEST(Automap, left) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::left();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 2);
}

TEST(Automap, right) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::right();
	EXPECT_EQ(dvl::AutoMapXOfs, 2);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
}

TEST(Automap, zoomIn) {
	dvl::AutoMapScale = 50;
	dvl::zoomIn();
	EXPECT_EQ(dvl::AutoMapScale, 55);
	EXPECT_EQ(dvl::AmLine64, 35);
	EXPECT_EQ(dvl::AmLine32, 17);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}

TEST(Automap, AutomapZoomIn_Max) {
	dvl::AutoMapScale = 195;
	dvl::zoomIn();
	dvl::zoomIn();
	EXPECT_EQ(dvl::AutoMapScale, 200);
	EXPECT_EQ(dvl::AmLine64, 128);
	EXPECT_EQ(dvl::AmLine32, 64);
	EXPECT_EQ(dvl::AmLine16, 32);
	EXPECT_EQ(dvl::AmLine8, 16);
	EXPECT_EQ(dvl::AmLine4, 8);
}

TEST(Automap, zoomOut) {
	dvl::AutoMapScale = 200;
	dvl::zoomOut();
	EXPECT_EQ(dvl::AutoMapScale, 195);
	EXPECT_EQ(dvl::AmLine64, 124);
	EXPECT_EQ(dvl::AmLine32, 62);
	EXPECT_EQ(dvl::AmLine16, 31);
	EXPECT_EQ(dvl::AmLine8, 15);
	EXPECT_EQ(dvl::AmLine4, 7);
}

TEST(Automap, AutomapZoomOut_Min) {
	dvl::AutoMapScale = 55;
	dvl::zoomOut();
	dvl::zoomOut();
	EXPECT_EQ(dvl::AutoMapScale, 50);
	EXPECT_EQ(dvl::AmLine64, 32);
	EXPECT_EQ(dvl::AmLine32, 16);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}

TEST(Automap, zoomReset) {
	dvl::AutoMapScale = 50;
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::zoomReset();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
	EXPECT_EQ(dvl::AutoMapScale, 50);
	EXPECT_EQ(dvl::AmLine64, 32);
	EXPECT_EQ(dvl::AmLine32, 16);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}
