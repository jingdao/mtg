#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "ViewInterface.h"
#include "MTGPlayer.h"

void loadCardDataTable();
void buildDeck(List* cards);
void shuffleDeck(List* cards);
void apply_mulligan(MTGPlayer* p);
void resolveAttack(MTGPlayer* attacker,List* permanentList);
void newTurn();
MTGPlayer* newGame();
void startGame();
void endGame();
