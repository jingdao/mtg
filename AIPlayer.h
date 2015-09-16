#pragma once
#include "ViewInterface.h"

void newTurn();
void resolveAttack(MTGPlayer* attacker,List* permanentList);

typedef enum {
    AI_START = 0,
    AI_LAND = 1,
    AI_TAP = 2,
    AI_CREATURE = 3,
    AI_ATTACK = 4,
    AI_DISCARD = 5,
    AI_NONE = 6,
    AI_NUMSTATES = 7
} AIState;

void AI_init(MTGPlayer* player);
void AI_getAction();
void AI_getBlockers(List* attackerList, List* blockersList);
