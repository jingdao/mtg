#pragma once
#include <stdio.h>
#include "CardData.h"

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

typedef struct {
    Subtypes subtypes;
    bool is_tapped;
    bool has_attacked;
    bool has_blocked;
    bool has_summoning_sickness;
    int power;
    int toughness;
    int loyalty;
    MTGCard* source;
    MTGPlayer* owner;
    MTGPlayer* controller;
} Permanent;

Permanent* NewPermanent(MTGCard* source,MTGPlayer* own);
bool Permanent_sameColor(Permanent* p,Permanent* q);

MTGPlayer* InitMTGPlayer();
bool MTGPlayer_drawCards(MTGPlayer* p,int num);
bool MTGPlayer_playCard(MTGPlayer* p,int cardIndex,char* err);
void MTGPlayer_discard(MTGPlayer* player,int cardIndex);
void MTGPlayer_discardFromBattlefield(MTGPlayer* player,int cardIndex);
void MTGPlayer_refresh(MTGPlayer* p);
void MTGPlayer_restore(MTGPlayer* player);
void MTGPlayer_tap(MTGPlayer* player,Permanent* perm);
bool MTGPlayer_payMana(MTGPlayer* player,MTGCard* card);
void DeleteMTGPlayer(MTGPlayer* p);

void selectMana(int* mana,int amount);
void Event_gainLife(MTGPlayer* player,int num);
