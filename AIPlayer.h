#pragma once
#include "ViewInterface.h"

void newTurn();

typedef enum {
    AI_START = 0,
    AI_LAND = 1,
    AI_TAP = 2,
    AI_CREATURE = 3,
    AI_ATTACK = 4,
    AI_NONE = 5,
    AI_NUMSTATES = 6
} AIState;

void AI_init(MTGPlayer* player);
void AI_getAction();