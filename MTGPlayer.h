#include "CardData.h"
#pragma once

typedef struct {
	List* hand;
	List* deck;
} MTGPlayer;

MTGPlayer* InitMTGPlayer();
void MTGPlayer_drawCards(MTGPlayer* p,int num);
void DeleteMTGPlayer(MTGPlayer* p);
