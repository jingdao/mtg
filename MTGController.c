#include "MTGController.h"

extern CardData cd;

void buildDeck(List* cards) {
	for (int i=0;i<10;i++)
		AppendToList(cards,cd.SelflessCathar);
	for (int i=0;i<10;i++)
		AppendToList(cards,cd.OreskosSwiftclaw);
	for (int i=0;i<10;i++)
		AppendToList(cards,cd.RazorfootGriffin);
	for (int i=0;i<30;i++)
		AppendToList(cards,cd.Plains);
}

void shuffleDeck(List* cards) {
    unsigned int n = cards->size;
    List* tmp;
    while (n > 1) {
        unsigned int i = rand() % n;
        tmp = cards->entries[i];
        cards->entries[i] = cards->entries[n-1];
        cards->entries[n-1] = tmp;
        n--;
    }
}

void newGame() {
	List* deck = InitList();
	buildDeck(deck);
	shuffleDeck(deck);
	displayHand(deck);
    displayLifepoints(20, true);
    displayLifepoints(20, false);
	DeleteList(deck);
}
