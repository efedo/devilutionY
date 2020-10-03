//HEADER_GOES_HERE
#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "../actor.h"

#include <string>
#include <array>
#include <vector>

DEVILUTION_BEGIN_NAMESPACE
// These are all character class templates

class CharType {
public:
	std::array<char, 11> PlrGFXAnimLens;
	int PWVel[3] = { 2048, 1024, 512 };
	int AnimLenFromClass = 8; // Total number of frames in walk animation.
	int strength = 0;         // starting strength
	int magic = 0;
	int dexterity = 0;
	int vitality = 0;
	int block = 0;
	std::array<int, 4> MaxStats = { 0, 0, 0, 0 };
	std::string name = "unnamed";
};

class CharacterTypes {
public:
	const int numclasses = 3;
	CharacterTypes()
	{
		init();
	}
	void init()
	{
		list[0].name = "Warrior";
		list[0].PlrGFXAnimLens = { 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 };
		list[0].strength = 30;
		list[0].magic = 10;
		list[0].dexterity = 20;
		list[0].vitality = 25;
		list[0].block = 30;
		list[0].MaxStats = { 250, 50, 60, 100 };

		list[1].name = "Rogue";
		list[1].PlrGFXAnimLens = { 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 };
		list[1].strength = 20;
		list[1].magic = 15;
		list[1].dexterity = 30;
		list[1].vitality = 20;
		list[1].block = 20;
		list[1].MaxStats = { 55, 70, 250, 80 };

		list[2].name = "Sorceror";
		list[2].PlrGFXAnimLens = { 8, 16, 8, 6, 20, 12, 8, 20, 8, 12, 8 };
		list[2].strength = 15;
		list[2].magic = 35;
		list[2].dexterity = 15;
		list[2].vitality = 20;
		list[2].block = 10;
		list[2].MaxStats = { 45, 250, 85, 80 };
	}

	CharType get(PlayerClass &pclass)
	{
		return list[int(pclass)];
	}

private:
	CharType list[3];
};

extern CharacterTypes classes;

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
