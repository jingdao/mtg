#include "EventHandler.h"

extern CardData cd;
extern MTGPlayer* player1;
extern MTGPlayer* player2;
extern MTGPlayer* currentPlayer;

void Event_gainLife(MTGPlayer* player,int num) {
    player->hp += num;
    char buffer[64];
    sprintf(buffer,"%s gain %d life",player==player1?"You":"Opponent",num);
    message(buffer);
}

