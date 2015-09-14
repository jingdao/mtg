#include "MTGController.h"

extern CardData cd;
extern HashTable* cdt;
MTGPlayer* player1;
MTGPlayer* player2;
MTGPlayer* currentPlayer;

void loadCardDataTable() {
	cdt = InitHashTable();
	int numCards = sizeof(CardData) / sizeof(MTGCard*);
	MTGCard** cardArray = & cd.AbbotofKeralKeep;
	for (int i=0;i<numCards;i++) {
		HashTable_insertVar(cdt,(void*)(*cardArray)->name, strlen((*cardArray)->name), *cardArray);
		cardArray++;
	}
}

void buildDeck(List* cards) {
	for (int i=0;i<10;i++)
		AppendToList(cards,cd.SelflessCathar);
	for (int i=0;i<10;i++)
		AppendToList(cards,cd.OreskosSwiftclaw);
	for (int i=0;i<10;i++)
		AppendToList(cards,cd.RazorfootGriffin);
	for (int i=0;i<10;i++)
		AppendToList(cards,cd.Plains);
    for (int i=0;i<10;i++)
        AppendToList(cards,cd.Swamp);
    for (int i=0;i<10;i++)
        AppendToList(cards,cd.Forest);
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

void apply_mulligan(MTGPlayer* p) {
    unsigned int sz = p->hand->size;
    if (sz == 0)
        return;
    for (unsigned int i=0;i<sz;i++) {
        AppendToList(p->library, p->hand->entries[i]);
    }
    p->hand->size = 0;
    shuffleDeck(p->library);
    MTGPlayer_drawCards(p, sz - 1);
    displayHand(p->hand);
    displayStats(p->hp,p->library->size,p->hand->size, p->mana,true);
}

void resolveAttack(MTGPlayer* attacker,List* permanentList) {
    MTGPlayer* defender = attacker==player1 ? player2 : player1;
    for(unsigned int i=0;i<permanentList->size;i++) {
        Permanent* p = permanentList->entries[i];
        defender->hp -= p->power;
    }
    displayStats(player1->hp,player1->library->size,player1->hand->size, player1->mana,true);
    displayBattlefield(player1->battlefield, true);
    displayStats(player2->hp,player2->library->size,player2->hand->size, player2->mana,false);
    displayBattlefield(player2->battlefield, false);
}

void newTurn() {

	if (currentPlayer == player1) {
		currentPlayer = player2;
        newTurn();
	} else {
		currentPlayer = player1;
		MTGPlayer_refresh(player1);
		startTurn(player1);
		MTGPlayer_drawCards(player1, 1);
		displayHand(player1->hand);
		displayStats(player1->hp,player1->library->size,player1->hand->size, player1->mana,true);
        displayLands(player1->lands, true);
        displayBattlefield(player1->battlefield, true);
	}

}

MTGPlayer* newGame() {
    player1 = InitMTGPlayer();
//	buildDeck(player1->library);
	loadDeck("deck.txt",player1->library);
	shuffleDeck(player1->library);
	saveDeck("deck.txt",player1->library);

	player2 = InitMTGPlayer();
	buildDeck(player2->library);
	shuffleDeck(player2->library);

	MTGPlayer_drawCards(player1,7);
	MTGPlayer_drawCards(player2,7);

	displayHand(player1->hand);
    displayStats(player1->hp,player1->library->size,player1->hand->size, player1->mana,true);
    displayLands(player1->lands, true);
    displayBattlefield(player1->battlefield, true);
    displayStats(player2->hp,player2->library->size,player2->hand->size,player2->mana ,false);
    displayLands(player2->lands, false);
    displayBattlefield(player2->battlefield, false);
  
 	if (rand() % 2) {
		currentPlayer = player1;
		message("You start first");
	} else {
		currentPlayer = player2;
		message("Opponent starts first");
	}
    
    mulligan();
    return player1;
}

void startGame() {
    if (currentPlayer == player1)
        startTurn(currentPlayer); //starting player does not draw 1 card
    else
        newTurn();
}

void endGame() {
    DeleteMTGPlayer(player1);
    DeleteMTGPlayer(player2);
}
