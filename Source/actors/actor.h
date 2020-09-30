/**
 * @file actor.h
 *
 * Actor classes (in progress).
 */
#ifndef __ACTOR_H__
#define __ACTOR_H__

DEVILUTION_BEGIN_NAMESPACE

class ActorType {
};

class Actor {
};

// class Player : public actor

// Non-player actor
class NpcType : public ActorType {
};

// Non-player actor
class Npc : public Actor {
};

class Towner : public Npc {
};

// class Monster : public Npa

DEVILUTION_END_NAMESPACE

#endif // __ACTOR_H__
