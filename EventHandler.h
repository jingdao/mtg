#pragma once
#include "Callback.h"
#include "AIPlayer.h"

void initEvents();
void DeleteEvents();
MTGPlayer* findTarget(Permanent* p,unsigned int* index);
void Event_gainLife(Permanent* source,MTGPlayer* player,int num);
void Event_loseLife(Permanent* source,MTGPlayer* player,int num);
void Event_damage(Permanent* attacker, Permanent* defender,int num);
void Event_onAbility(Permanent* permanent);
bool Event_onPlay(Permanent* permanent);
bool Event_onPlayAbility(Permanent* permanent);
void Event_onResolve(Permanent* permanent);
void Event_onDestroy(Permanent* permanent);
void Event_onUpkeep(MTGPlayer* player);