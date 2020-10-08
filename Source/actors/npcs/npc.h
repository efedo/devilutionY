// HEADER_GOES_HERE
#ifndef __NPC_H__
#define __NPC_H__

#include "../actor.h"

namespace dvl {

// Non-player actor
class Npc : public Actor
{
   public:
	Npc() : Actor(ActorType::npc)
	{
	}
};

}

#endif __NPC_H__
