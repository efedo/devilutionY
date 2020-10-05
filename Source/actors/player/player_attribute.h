/**
 * @file xyz.abc
 *
 * Description.
 */
#ifndef __PLAYER_ATTRIBUTES__
#define __PLAYER_ATTRIBUTES__

DEVILUTION_BEGIN_NAMESPACE

class PAttribute {
public:

	operator int() {
		return get();
	}

	[[nodiscard]] int get() const noexcept {
		const int act = _base + _modifiers;
		if (act > _act_max) {
			return _act_max;
		}
		else if (act < _act_min) {
			return _act_min;
		};
		return act;
	}

	[[nodiscard]] int getBase() const noexcept {
		return _base;
	}

	void check() noexcept {
		if (_base > _base_max) _base = _base_max;
		if (_base < _base_min) _base = _base_min;
		if (get() > _act_max) _base -= get() - _act_max;
		if (get() < _act_min) _base += _act_min - get();
	}

	int statRoom() noexcept {
		assert((_base_max - _base) >= 0);
		return _base_max - _base;
	}

	void setBase(int nbase) noexcept {
		_base = nbase;
		check();
	}

	void changeBase(int chg) noexcept {
		_base += chg;
		check();
	}

	void setBaseMin(int nval) noexcept {
		assert(nval <= _base_max);
		_base_min = nval;
		check();
	}

	void setBaseMax(int nval) noexcept {
		assert(nval >= _base_min);
		_base_max = nval;
		check();
	}

	void setActMin(int nval) noexcept {
		assert(nval <= _act_max && nval >= _act_min);
		_act_min = nval;
	}

	void setActMax(int nval) noexcept {
		assert(nval > 0);
		assert(nval >= _act_min);
		_act_max = nval;
	}

private:
	int _base = 1; // current base value
    int _base_min = -10000; // minimum base value
	int _base_max = 750; // maximum base value
	int _modifiers = 0; // current modifiers
	int _act_min = 0; // actual maximum value
	int _act_max = 750; // actual maximum value
	int _regen = 0; // how many points to regen per X (what is X?)
};

DEVILUTION_END_NAMESPACE

#endif  // __PLAYER_ATTRIBUTES__
