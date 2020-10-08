/**
 * @file actor.h
 *
 * Actor classes (in progress).
 */
#ifndef __ACTOR_H__
#define __ACTOR_H__

#include <numeric>

namespace dvl {

enum class ActorType : uint8_t {
	none = 0,
	player = 1,
	monster = 2,
	npc = 3,
	fauna = 4,
};

struct ActorId
{
	using BaseT = uint8_t; // limited to uint8_t until you fix networking
	[[nodiscard]] bool isValid() const { return id != std::numeric_limits<BaseT>::max(); };
	[[nodiscard]] static constexpr ActorId getInvalid() {
		return ActorId{std::numeric_limits<BaseT>::max() };
	}
	[[nodiscard]] ActorId setInvalid() {
		id = std::numeric_limits<BaseT>::max();
	}
	// No conversion constructor, only actor manager should ever create these
	BaseT id = std::numeric_limits<BaseT>::max();
	explicit operator uint8_t() {
		assert(id <= std::numeric_limits<uint8_t>::max());
		return id;
	}
	explicit operator uint16_t() { return id; }
	explicit operator size_t() { return id; }
	friend bool operator==(const ActorId& lhs, const ActorId& rhs) {
		return (lhs.id == rhs.id);
	}
	friend bool operator!=(const ActorId& lhs, const ActorId& rhs) {
		return (lhs.id != rhs.id);
	}
};

class Actor {
   public:
	Actor(ActorType newtype);
	~Actor();
	ActorId id() const { return _id; }
	V2Di _drawpos;
	ActorType type = ActorType::none;
	operator ActorId() { return _id; }
	virtual bool posOk() { return false; }
   private:
	ActorId _id;
};

}

#endif // __ACTOR_H__
