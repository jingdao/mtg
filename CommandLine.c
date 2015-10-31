#include <stdio.h>
#include "ViewInterface.h"

extern CardData cd;
extern HashTable* cdt;
extern MTGPlayer* myplayer;

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

void displayStats(int hp,int librarySize,int handSize,int graveyardSize,int exileSize, int* mana,bool selfOrOpponent) {
	if (selfOrOpponent) {
		printf("Your HP: %d Library: %d Hand: %d\nGraveyard: %d\nExile: %d\n",hp,librarySize,handSize,graveyardSize,exileSize);
		printf("W: %d U: %d B: %d G: %d R: %d\n",mana[1],mana[2],mana[3],mana[4],mana[5]);
	} else {
		printf("Opponent's HP: %d Library: %d Hand: %d\nGraveyard: %d\n",hp,librarySize,handSize,graveyardSize);
		printf("W: %d U: %d B: %d G: %d R: %d\n",mana[1],mana[2],mana[3],mana[4],mana[5]);
	}
}

void displayStack(List* stack) {

}

void displayWinner(MTGPlayer* winner) {

}

void selectMana(int* mana,int amount) {

}

void selectBlockers(List* permanentList,List* blockersList) {
}

void selectCallback(void (*callback)(Permanent*),char* allowedTargets) {
}

void selectTarget(Permanent* source,char* allowedTargets) {
}

void selectPlayer(Permanent* source) {

}

void selectCreatureOrPlayer(Permanent* source) {

}

void selectAbility(Permanent* source,List* options) {

}

void selectOption(Permanent* source, List* option) {

}

void selectCards(Permanent* source,List* cards,char* allowedTargets) {

}

void triggerSelect(char* msg) {

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
	if (!file)
		return;
	while (fgets(buffer,128,file)) {
		MTGCard* card = (MTGCard*) HashTable_findVar(cdt,buffer,strlen(buffer) - 1);
		AppendToList(cards,card);
	}
	fclose(file);
	
}

void discardCards(MTGPlayer* player,int num) {

}

void startTurn(MTGPlayer* player) {
	if (player == myplayer) {

	} else {

	}
}

void mulligan() {
}

void message(char* msg) {
	printf("%s\n",msg);
}
