#pragma once
#include "ViewInterface.h"

void newTurn();
void resolveAttack(MTGPlayer* attacker,List* permanentList);
bool Event_onPlay(Permanent* permanent);

typedef enum {
    AI_START = 0,
    AI_LAND = 1,
    AI_TAP = 2,
    AI_CREATURE = 3,
    AI_SORCERY = 4,
    AI_ATTACK = 5,
    AI_DISCARD = 6,
    AI_NONE = 7,
    AI_NUMSTATES = 8
} AIState;

void AI_init(MTGPlayer* player);
void AI_getAction();
void AI_getBlockers(List* attackerList, List* blockersList);
bool AI_payMana(MTGCard* card);
void AI_selectTarget(Permanent* source,char* allowedTargets);
void AI_selectPlayer(Permanent* source);
