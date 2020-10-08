/**
 * @file inv.cpp
 *
 * Inv display rectangle
 */
#include "all.h"

namespace dvl {


PAttribute::operator int() {
	return get();
}

[[nodiscard]] int PAttribute::get() const noexcept {
	const int act = _base + _modifiers;
	if (act > _act_max) {
		return _act_max;
	}
	else if (act < _act_min) {
		return _act_min;
	};
	return act;
}

[[nodiscard]] int PAttribute::getBase() const noexcept {
	return _base;
}

void PAttribute::check() noexcept {
	if (_base > _base_max) _base = _base_max;
	if (_base < _base_min) _base = _base_min;
	if (get() > _act_max) _base -= get() - _act_max;
	if (get() < _act_min) _base += _act_min - get();
}

int PAttribute::statRoom() noexcept {
	assert((_base_max - _base) >= 0);
	return _base_max - _base;
}

void PAttribute::setBase(int nbase) noexcept {
	_base = nbase;
	check();
}

void PAttribute::changeBase(int chg) noexcept {
	_base += chg;
	check();
}

void PAttribute::setBaseMin(int nval) noexcept {
	assert(nval <= _base_max);
	_base_min = nval;
	check();
}

void PAttribute::setBaseMax(int nval) noexcept {
	assert(nval >= _base_min);
	_base_max = nval;
	check();
}

void PAttribute::setActMin(int nval) noexcept {
	assert(nval <= _act_max && nval >= _act_min);
	_act_min = nval;
}

void PAttribute::setActMax(int nval) noexcept {
	assert(nval > 0);
	assert(nval >= _act_min);
	_act_max = nval;
}

}

