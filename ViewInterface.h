#pragma once
#include "HashTable.h"
#include "MTGPlayer.h"
void displayHand(List* cards);
void displayLands(List* permanents, bool selfOrOpponent);
void displayBattlefield(List* permanents, bool selfOrOpponent);
void displayStack(List* stack);
void displayStats(int hp,int librarySize,int handSize, int graveyardSize, int exileSize,int* mana,bool selfOrOpponent);
void displayWinner(MTGPlayer* winner);
void discardCards(MTGPlayer* player,int num);
void selectMana(int* mana,int amount);
void selectBlockers(List* permanentList,List* blockersList);
void selectTarget(Permanent* source,char* allowedTargets);
void selectCreatureOrPlayer(Permanent* source);
void selectPlayer(Permanent* source);
void selectAbility(Permanent* permanent);
void selectCards(Permanent* permanent,List* cards,char* allowedTargets);
void selectOption(Permanent* source,List* options);
void triggerSelect(char* msg);
void saveDeck(char* name,List* cards);
void loadDeck(char* name,List* cards);
void startTurn(MTGPlayer* player);
void mulligan();
void message(char* msg);
void AI_status(char* msg);

