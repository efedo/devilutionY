// HEADER_GOES_HERE
#ifndef __NPC_H__
#define __NPC_H__

#include "../actor.h"

DEVILUTION_BEGIN_NAMESPACE

// Non-player actor
class Npc : public Actor
{
   public:
	Npc() : Actor(ActorType::npc)
	{
	}
};

DEVILUTION_END_NAMESPACE

#endif __NPC_H__
