#pragma once
#include "ViewInterface.h"

void initEvents();
void DeleteEvents();
void Event_gainLife(MTGPlayer* player,int num);
void Event_damage(Permanent* attacker, Permanent* defender,int num);
void Event_tapAbility(Permanent* permanent,int index);