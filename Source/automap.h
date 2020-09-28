/**
 * @file automap.h
 *
 * Interface of the in-game map overlay.
 */
#ifndef __AUTOMAP_H__
#define __AUTOMAP_H__

DEVILUTION_BEGIN_NAMESPACE

class Automap {
public:
	void enable(bool var = true) { automapflag = var; }
	void disable() { enable(false); }
	[[nodiscard]] bool enabled() { return automapflag; }
	void initOnce();
	void init();
	void start();
	void up();
	void down();
	void left();
	void right();
	void zoomIn();
	void zoomOut();
	void zoomReset();
	void setScale(int scale) { AutoMapScale = scale; }
	void draw();
	void drawTile(V2Di s, WORD automap_type);
	void drawPlayer();
	void drawText();


	WORD GetAutomapType(V2Di pos, bool view);
	void SetAutomapView(V2Di pos);
	auto & getView() { return automapview; }
	int &getScale() { return AutoMapScale; }
	void addOffsetX(int offset) { AutoMapOfs.x += offset; }
	void addOffsetY(int offset) { AutoMapOfs.y += offset; }
private:
	WORD automaptype[512];  // Maps from tile_id to automap type.
							// BUGFIX: only the first 256 elements are ever read

	V2Di AutoMap;
	bool automapflag; // Specifies whether the automap is enabled.
	char AmShiftTab[31];
	bool automapview[DMAXX][DMAXY]; // Tracks the explored areas of the map.
	int AutoMapScale; // Specifies the scale of the automap.
	V2Di AutoMapOfs;
	//int AutoMapXOfs; // X offset
	//int AutoMapYOfs; // Y offset
	int AmLine64;
	int AmLine32;
	int AmLine16;
	int AmLine8;
	int AmLine4;
};

extern Automap automap;

DEVILUTION_END_NAMESPACE

#endif /* __AUTOMAP_H__ */
