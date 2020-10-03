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
	npc = 3,
	fauna = 4,
};

struct ActorId
{
	// No conversion constructor, only actor manager should ever create these
	uint16_t id;
	operator uint16_t() { return id; }
};

class Actor {
   public:
	Actor(ActorType newtype);
	~Actor();
	ActorId id() const { return _id; }
	V2Di _drawpos;
	ActorType type = ActorType::none;
   private:
	ActorId _id;
};

DEVILUTION_END_NAMESPACE

#endif // __ACTOR_H__
