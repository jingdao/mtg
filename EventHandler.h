#pragma once
#include "ViewInterface.h"

void Event_gainLife(MTGPlayer* player,int num);
void Event_tapAbility(Permanent* permanent,int index);

void message(char* msg);