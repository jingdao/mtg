#include <stdio.h>
#include "ViewInterface.h"

extern CardData cd;

void displayHand(List* cards) {
	for(unsigned int i=0;i<cards->size;i++) {
		MTGCard* c = (MTGCard*)cards->entries[i];
		printf("%s, ",c->name);
	}
	printf("\n");
}

void displayBattlefield(List* permanents, bool selfOrOpponent) {

}

void displayLands(List* permanents, bool selfOrOpponent) {

}

void displayStats(int hp,int librarySize,int handSize, bool selfOrOpponent) {
	if (selfOrOpponent)
		printf("Your HP: %d Library: %d Hand: %d\n",hp,librarySize,handSize);
	else
		printf("Opponent's HP: %d Library: %d Hand: %d\n",hp,librarySize,handSize);
}

void saveDeck(char* name,List* cards) {
	FILE* file = fopen(name,"w");
	for (unsigned int i=0;i<cards->size;i++) {
		MTGCard* card = (MTGCard*)cards->entries[i];
		fprintf(file,"%s\n",card->name);
	}
	fclose(file);
}

void loadDeck(char* name,List* cards) {
	
}

void discardToSeven(MTGPlayer* player) {

}

void startTurn(MTGPlayer* player) {

}

void mulligan() {
}

void message(char* msg) {
	printf("%s\n",msg);
}
