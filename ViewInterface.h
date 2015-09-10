#include "List.h"
#include "CardData.h"
#include "MTGPlayer.h"
#pragma once
void displayHand(List* cards);
void displayLands(List* permanents, bool selfOrOpponent);
void displayBattlefield(List* permanents, bool selfOrOpponent);
void displayStats(int hp,int librarySize,int handSize, bool selfOrOpponent);
void discardToSeven(MTGPlayer* player);
void saveDeck(char* name,List* cards);
void loadDeck(char* name,List* cards);
void startTurn(MTGPlayer* player);
void mulligan();
void message(char* msg);

