#include <stdio.h>
#include "ViewInterface.h"

extern CardData cd;
extern HashTable* cdt;

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

void displayStats(int hp,int librarySize,int handSize, int* mana,bool selfOrOpponent) {
	if (selfOrOpponent) {
		printf("Your HP: %d Library: %d Hand: %d\n",hp,librarySize,handSize);
		printf("W: %d U: %d B: %d G: %d R: %d\n",mana[1],mana[2],mana[3],mana[4],mana[5]);
	} else {
		printf("Opponent's HP: %d Library: %d Hand: %d\n",hp,librarySize,handSize);
		printf("W: %d U: %d B: %d G: %d R: %d\n",mana[1],mana[2],mana[3],mana[4],mana[5]);
	}
}

void selectMana(int* mana,int amount) {

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
	char buffer[128];
	FILE* file = fopen(name,"r");
	while (fgets(buffer,128,file)) {
		MTGCard* card = (MTGCard*) HashTable_findVar(cdt,buffer,strlen(buffer) - 1);
		AppendToList(cards,card);
	}
	fclose(file);
	
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
