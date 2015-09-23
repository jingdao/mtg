#pragma once
#include "Callback.h"

void initEvents();
void DeleteEvents();
MTGPlayer* findTarget(Permanent* p,unsigned int* index);
void Event_gainLife(Permanent* source,MTGPlayer* player,int num);
void Event_damage(Permanent* attacker, Permanent* defender,int num);
void Event_tapAbility(Permanent* permanent,int index);
bool Event_onPlay(Permanent* permanent);
void Event_onResolve(Permanent* permanent);
void Event_onDestroy(Permanent* permanent);