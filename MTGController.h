#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "EventHandler.h"


void Event_gainLife(Permanent* source, MTGPlayer* player,int num);

void loadCardDataTable();
void loadAbilities();
void buildDeck(List* cards,int index);
void shuffleDeck(List* cards);
void apply_mulligan(MTGPlayer* p);
void resolveAttack(MTGPlayer* attacker,List* permanentList);

bool resolveBlock();
bool resolveLethalDamage();
bool resolveStack();
void resolveAI();
void endAttack();
void newTurn();
MTGPlayer* newGame(int deck_index);
void startGame();
void endGame();
