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

void displayLifepoints(int hp,bool selfOrOppponent) {
	if (selfOrOppponent)
		printf("Your HP: %d\n",hp);
	else
		printf("Opponent's HP: %d\n",hp);
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
