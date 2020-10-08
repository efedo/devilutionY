/**
 * @file xyz.abc
 *
 * Description.
 */
#ifndef __PLAYER_ATTRIBUTES__
#define __PLAYER_ATTRIBUTES__

namespace dvl {

class PAttribute {
public:
	operator int();
	[[nodiscard]] int get();
	[[nodiscard]] int getBase() const noexcept;
	void check() noexcept;
	int statRoom() noexcept;
	void setBase(int nbase) noexcept;
	void changeBase(int chg) noexcept;
	void setBaseMin(int nval) noexcept;
	void setBaseMax(int nval) noexcept;
	void setActMin(int nval) noexcept;
	void setActMax(int nval) noexcept;
private:
	int _base = 1; // current base value
    int _base_min = -10000; // minimum base value
	int _base_max = 750; // maximum base value
	int _modifiers = 0; // current modifiers
	int _act_min = 0; // actual maximum value
	int _act_max = 750; // actual maximum value
	int _regen = 0; // how many points to regen per X (what is X?)
};

}

#endif  // __PLAYER_ATTRIBUTES__
