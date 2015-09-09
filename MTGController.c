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
	MTGPlayer* player1 = InitMTGPlayer();
	buildDeck(player1->deck);
	saveDeck("deck.txt",player1->deck);
	shuffleDeck(player1->deck);

	MTGPlayer* player2 = InitMTGPlayer();
	buildDeck(player2->deck);
	shuffleDeck(player2->deck);

	MTGPlayer_drawCards(player1,7);
	MTGPlayer_drawCards(player2,7);

	displayHand(player1->hand);
    displayLifepoints(20, true);
    displayLifepoints(20, false);

	DeleteMTGPlayer(player1);
	DeleteMTGPlayer(player2);
}
