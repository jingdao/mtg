#include "EventHandler.h"

extern CardData cd;
extern MTGPlayer* player1;
extern MTGPlayer* player2;
extern MTGPlayer* currentPlayer;

List* gainLifeSubscribers = NULL;

void Event_gainLife(MTGPlayer* player,int num) {
    player->hp += num;
    char buffer[64];
    sprintf(buffer,"%s gain %d life",player==player1?"You":"Opponent",num);
    message(buffer);
    
    for (unsigned int i=0;i<gainLifeSubscribers->size;i++) {
        Permanent* p = gainLifeSubscribers->entries[i];
        if (p->source == cd.AjanisPridemate && player == p->controller) {
            p->bonusPower++; p->bonusToughness++; p->power++; p->toughness++;
            sprintf(buffer,"%s gets +1/+1 counter",p->source->name);
            message(buffer);
            displayBattlefield(player->battlefield, player==player1);
        }
    }
    
    displayStats(player->hp,player->library->size,player->hand->size,player->graveyard->size,player->mana,player==player1);
}

void Event_damage(Permanent* attacker, Permanent* defender,int num) {
    if (attacker->subtypes.is_deathtouch)
        defender->toughness = 0;
    else
        defender->toughness -= num;
    if (defender->source == cd.WallofEssence) {
        Event_gainLife(defender->controller, num);
    }
}

void initEvents() {
    gainLifeSubscribers = InitList();
}

void DeleteEvents() {
    if (gainLifeSubscribers)
        DeleteList(gainLifeSubscribers);
}

