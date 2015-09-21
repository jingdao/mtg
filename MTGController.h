#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "ViewInterface.h"
#include "MTGPlayer.h"
#include "AIPlayer.h"

void loadCardDataTable();
void buildDeck(List* cards,int index);
void shuffleDeck(List* cards);
void apply_mulligan(MTGPlayer* p);
void resolveAttack(MTGPlayer* attacker,List* permanentList);
bool resolveBlock();
bool resolveLethalDamage();
void resolveAI();
void endAttack();
void newTurn();
MTGPlayer* newGame(int deck_index);
void startGame();
void endGame();
