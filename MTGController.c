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
	
}

void newGame() {
	List* deck = InitList();
	buildDeck(deck);
	shuffleDeck(deck);
	displayHand(deck);
	DeleteList(deck);
}
