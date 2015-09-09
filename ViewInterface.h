#include "List.h"
#include "CardData.h"
#include "MTGPlayer.h"
#pragma once
void displayHand(List* cards);
void displayStats(int hp,int librarySize,int handSize, bool selfOrOpponent);
void discardToSeven(MTGPlayer* player);
void saveDeck(char* name,List* cards);
void loadDeck(char* name,List* cards);
void startTurn(MTGPlayer* player);
void message(char* msg);

