#pragma once
#include <stdio.h>
#include "CardData.h"

void selectMana(int* mana,int amount);

typedef struct {
	List* hand;
	List* library;
    List* graveyard;
    List* exile;
    List* lands;
    List* battlefield;
    int mana[6]; //combined,white,blue,black,red,green
    int hp;
    bool playedLand;
} MTGPlayer;

MTGPlayer* InitMTGPlayer();
bool MTGPlayer_drawCards(MTGPlayer* p,int num);
bool MTGPlayer_playCard(MTGPlayer* p,int cardIndex,char* err);
void MTGPlayer_discard(MTGPlayer* player,int cardIndex);
void MTGPlayer_refresh(MTGPlayer* p);
void MTGPlayer_restore(MTGPlayer* player);
void MTGPlayer_tap(MTGPlayer* player,Permanent* perm);
bool MTGPlayer_payMana(MTGPlayer* player,MTGCard* card);
void DeleteMTGPlayer(MTGPlayer* p);
