/**
 * @file monster_manager.cpp
 *
 * Monster manager class (manages monster instances, not types)
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Actor::Actor(ActorType newtype)
    : type(newtype)
{
	_id = actors._registerActor(this);
}

Actor::~Actor()
{
	actors._unregisterActor(_id);
}

DEVILUTION_END_NAMESPACE
