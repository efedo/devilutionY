/**
 * @file actor.h
 *
 * Actor classes (in progress).
 */
#ifndef __ACTOR_H__
#define __ACTOR_H__

DEVILUTION_BEGIN_NAMESPACE

enum class ActorType : uint8_t {
	none = 0,
	player = 1,
	monster = 2,
	towner = 3,
	fauna = 4,
};

class Actor {
   public:
	Actor(ActorType newtype) : type(newtype) {}
	V2Di _drawpos;
	ActorType type = ActorType::none;
};

DEVILUTION_END_NAMESPACE

#endif // __ACTOR_H__
