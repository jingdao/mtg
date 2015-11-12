#pragma once
#include "ViewInterface.h"

void newTurn();
void resolveAttack(MTGPlayer* attacker,List* permanentList);
bool Event_onPlay(Permanent* permanent);
bool Event_onPlayAbility(Permanent* permanent);
bool Event_attack(MTGPlayer *player,List* attackers,char* err);

typedef enum {
    AI_START = 0,
    AI_LAND = 1,
    AI_TAP = 2,
    AI_CREATURE = 3,
    AI_SORCERY = 4,
    AI_ABILITY = 5,
    AI_ATTACK = 6,
    AI_DISCARD = 7,
    AI_NONE = 8,
    AI_NUMSTATES = 9
} AIState;

void AI_init(MTGPlayer* player);
void AI_getAction();
void AI_getBlockers(List* attackerList, List* blockersList);
bool AI_payMana(List* manaCost);
void AI_discard(int num);
void AI_selectTarget(Permanent* source,char* allowedTargets);
void AI_selectPlayer(Permanent* source);
void AI_selectAbility(Permanent* permanent,List* options);
void AI_selectCards(Permanent* permanent,List* cards,char* allowedTargets);
void AI_selectOption(Permanent* permanent,List* options);
void AI_selectConvoke(MTGPlayer* player,int index);
