#pragma once
#include <stdio.h>
#include "CardData.h"

struct permanent_struct;

typedef enum {
    GRAVEYARD,
    EXILE,
    HAND
} Destination;

typedef struct {
	List* hand;
	List* library;
    List* graveyard;
    List* exile;
    List* lands;
    List* battlefield;
    struct permanent_struct *marker;
    int mana[6]; //combined,white,blue,black,red,green
    int hp;
    bool playedLand;
    bool hasCastSpell;
} MTGPlayer;

struct permanent_struct{
    const char* name;
    Subtypes subtypes;
    bool is_tapped;
    bool is_untap_blocked;
    bool has_attacked;
    bool has_blocked;
    bool has_summoning_sickness;
    bool is_activated;
    bool canAttack;
    int power,sourcePower,bonusPower;
    int toughness,sourceToughness,bonusToughness;
    int loyalty;
    int selectedAbility;
    int X;
    List* equipment;
    struct permanent_struct *target;
    struct permanent_struct *target2;
    struct permanent_struct *target3;
    MTGCard* source;
    MTGPlayer* owner;
    MTGPlayer* controller;
};

typedef struct permanent_struct Permanent;

Permanent* NewPermanent(MTGCard* source,MTGPlayer* own);
Permanent* NewCreatureToken(MTGPlayer* own,int pow,int tough,const char* nm);
bool Permanent_sameColor(Permanent* p,Permanent* q);
void DeletePermanent(Permanent* permanent);

MTGPlayer* InitMTGPlayer();
bool MTGPlayer_drawCards(MTGPlayer* p,int num);
Permanent* MTGPlayer_playCard(MTGPlayer* p,int cardIndex,char* err);
void MTGPlayer_discard(MTGPlayer* player,int cardIndex);
void MTGPlayer_discardFromBattlefield(MTGPlayer* player,int cardIndex,Destination dest);
void MTGPlayer_refresh(MTGPlayer* p);
void MTGPlayer_restore(MTGPlayer* player);
bool MTGPlayer_tap(MTGPlayer* player,Permanent* perm);
bool MTGPlayer_activateAbility(MTGPlayer* player,Permanent* perm,char* err);
bool MTGPlayer_payMana(MTGPlayer* player,List* manaCost);
bool MTGPlayer_block(Permanent* attacker,List* defenders,char* err);
Permanent* MTGPlayer_getBattlefieldPermanent(List* bt,unsigned int index);
void DeleteMTGPlayer(MTGPlayer* p);

void selectMana(int* mana,int amount);
void selectAbility(Permanent* permanent);
bool AI_payMana(List* manaCost);
void AI_selectAbility(Permanent* permanent);
void Event_tapAbility(Permanent* permanent,int index);
void Event_onDestroy(Permanent* permanent,Destination dest);
void Event_loseLife(Permanent* source,MTGPlayer* player,int num);
