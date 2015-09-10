#include "MTGPlayer.h"

extern CardData cd;

MTGPlayer* InitMTGPlayer() {
	MTGPlayer* p = (MTGPlayer*)malloc(sizeof(MTGPlayer));
	p->library=InitList();
	p->hand=InitList();
    p->graveyard=InitList();
    p->exile=InitList();
    p->lands=InitList();
    p->battlefield=InitList();
    p->playedLand = false;
    memset(p->mana,0,6 * sizeof(int));
    p->hp=20;
	return p;
}

void MTGPlayer_drawCards(MTGPlayer* p,int num) {
	for (int i=0;i<num;i++) {
		AppendToList(p->hand,p->library->entries[p->library->size-1-i]);
	}
	p->library->size -=num;
}

bool MTGPlayer_playCard(MTGPlayer* player,int cardIndex, char* err) {
    MTGCard* card = (MTGCard*)player->hand->entries[cardIndex];
    if (card->cmc > player->mana[0]) {
        sprintf(err,"Not enough mana to play %s (%d/%d)",card->name,player->mana[0],card->cmc);
        return false;
    }
    if (card->is_land && player->playedLand) {
        sprintf(err,"You can only play one Land per turn");
        return false;
    }
    
    //create permanent
    Permanent* permanent = NewPermanent(card);
        
    //apply card effects
    if (card->is_land) {
        player->playedLand = true;
        AppendToList(player->lands, permanent);
    } else if (card->is_creature || card->is_planeswalker) {
        AppendToList(player->battlefield, permanent);
    }
    
    //remove card from hand
    memmove(player->hand->entries+cardIndex,player->hand->entries+cardIndex+1,(player->hand->size-1-cardIndex)*sizeof(void*));
    player->hand->size--;
    
    return true;
}

void MTGPlayer_tapLand(MTGPlayer* player,int cardIndex) {
    Permanent* permanent = player->lands->entries[cardIndex];
    permanent->is_tapped = true;
    if (permanent->source == cd.Plains) player->mana[1]++;
    else if (permanent->source == cd.Island) player->mana[2]++;
    else if (permanent->source == cd.Swamp) player->mana[3]++;
    else if (permanent->source == cd.Mountain) player->mana[4]++;
    else if (permanent->source == cd.Forest) player->mana[5]++;
}

void MTGPlayer_refresh(MTGPlayer* player) {
    player->playedLand = false;
    memset(player->mana,0,6 * sizeof(int));
}

void DeleteMTGPlayer(MTGPlayer* p) {
	DeleteList(p->library);
	DeleteList(p->hand);
    DeleteList(p->graveyard);
    DeleteList(p->exile);
    for (unsigned int i=0;i<p->lands->size;i++)
        free(p->lands->entries[i]);
    for (unsigned int i=0;i<p->battlefield->size;i++)
        free(p->battlefield->entries[i]);
	free(p);
}
