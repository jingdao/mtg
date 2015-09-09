#include <stdio.h>
#include "CardData.h"
#pragma once

typedef struct {
	List* hand;
	List* library;
    List* graveyard;
    int mana[6]; //combined,white,blue,black,red,green
    int hp;
    bool playedLand;
} MTGPlayer;

MTGPlayer* InitMTGPlayer();
void MTGPlayer_drawCards(MTGPlayer* p,int num);
bool MTGPlayer_playCard(MTGPlayer* p,int cardIndex,char* err);
void MTGPlayer_refresh(MTGPlayer* p);
void DeleteMTGPlayer(MTGPlayer* p);
