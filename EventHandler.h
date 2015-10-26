#pragma once
#include "Callback.h"
#include "AIPlayer.h"

void shuffleDeck(List* cards);
void initEvents();
void DeleteEvents();
MTGPlayer* findTarget(Permanent* p,int* index);
void Event_gainLife(Permanent* source,MTGPlayer* player,int num);
void Event_loseLife(Permanent* source,MTGPlayer* player,int num);
bool Event_damage(Permanent* defender,int num);
bool Event_combat(Permanent* attacker, Permanent* defender,int num);
bool Event_onAbility(Permanent* permanent);
bool Event_onPlay(Permanent* permanent);
bool Event_onPlayAbility(Permanent* permanent);
bool Event_onResolve(Permanent* permanent);
void Event_onDestroy(Permanent* permanent,Destination dest);
void Event_onUpkeep(MTGPlayer* player);
void Event_onRefresh(MTGPlayer* player);
void Event_onRestore(MTGPlayer* player);
bool Event_attack(MTGPlayer *player,List* attackers,char* err);