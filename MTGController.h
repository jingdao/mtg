#include <stdio.h>
#include <stdlib.h>
#include "ViewInterface.h"
#include "MTGPlayer.h"
#pragma once

void buildDeck(List* cards);
void shuffleDeck(List* cards);
void apply_mulligan(MTGPlayer* p);
void newTurn();
MTGPlayer* newGame();
void startGame();
void endGame();
