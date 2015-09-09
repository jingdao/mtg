#include "List.h"
#include "CardData.h"
#include "MTGPlayer.h"
#pragma once
void displayHand(List* cards);
void displayStats(int hp,int librarySize,int handSize, bool selfOrOpponent);
void saveDeck(char* name,List* cards);
void loadDeck(char* name,List* cards);
void startTurn(MTGPlayer* player);

