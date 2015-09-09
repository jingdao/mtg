#include "List.h"
#include "CardData.h"
#pragma once
void displayHand(List* cards);
void displayLifepoints(int hp,bool selfOrOppponent);
void saveDeck(char* name,List* cards);
void loadDeck(char* name,List* cards);

