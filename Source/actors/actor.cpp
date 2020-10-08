/**
 * @file monster_manager.cpp
 *
 * Monster manager class (manages monster instances, not types)
 */
#include "all.h"

namespace dvl {

Actor::Actor(ActorType newtype)
    : type(newtype)
{
	_id = actors._registerActor(this);
}

Actor::~Actor()
{
	actors._unregisterActor(_id);
}

}
