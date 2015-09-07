#include "ViewInterface.h"

void displayHand(List* cards) {
	for(unsigned int i=0;i<cards->size;i++) {
		MTGCard* c = (MTGCard*)cards->entries[i];
		printf("%s, ",c->name);
	}
	printf("\n");
}
