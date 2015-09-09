#include "MTGController.h"

extern CardData cd;
MTGPlayer* player1;
MTGPlayer* player2;
MTGPlayer* currentPlayer;

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

void newTurn() {

	if (currentPlayer == player1) {
		if (player1->hand->size > 7)
			discardToSeven(player1);
		currentPlayer = player2;
	} else {
		currentPlayer = player1;
		MTGPlayer_refresh(player1);
		startTurn(player1);
		MTGPlayer_drawCards(player1, 1);
		displayHand(player1->hand);
		displayStats(player1->hp,player1->library->size,player1->hand->size, true);
	}

}

void newGame() {
    player1 = InitMTGPlayer();
	buildDeck(player1->library);
	saveDeck("deck.txt",player1->library);
	shuffleDeck(player1->library);

	player2 = InitMTGPlayer();
	buildDeck(player2->library);
	shuffleDeck(player2->library);

	MTGPlayer_drawCards(player1,7);
	MTGPlayer_drawCards(player2,7);

	displayHand(player1->hand);
    displayStats(player1->hp,player1->library->size,player1->hand->size, true);
    displayStats(player2->hp,player2->library->size,player2->hand->size, false);
  
 	if (rand() % 2) {
		currentPlayer = player1;
		message("You start first");
	} else {
		currentPlayer = player2;
		message("Opponent starts first");
	}

	MTGPlayer_refresh(currentPlayer);
	startTurn(currentPlayer); //starting player does not draw 1 card
}

void endGame() {
    DeleteMTGPlayer(player1);
    DeleteMTGPlayer(player2);
}
