#include "MTGPlayer.h"

MTGPlayer* InitMTGPlayer() {
	MTGPlayer* p = (MTGPlayer*)malloc(sizeof(MTGPlayer));
	p->library=InitList();
	p->hand=InitList();
    p->graveyard=InitList();
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
        
    //apply card effects
    if (card->is_land) {
        player->playedLand = true;
    }
    
    //remove card from hand
    memmove(player->hand->entries+cardIndex,player->hand->entries+cardIndex+1,(player->hand->size-1-cardIndex)*sizeof(void*));
    player->hand->size--;
    
    return true;
}

void MTGPlayer_refresh(MTGPlayer* player) {
    player->playedLand = false;
}

void DeleteMTGPlayer(MTGPlayer* p) {
	DeleteList(p->library);
	DeleteList(p->hand);
    DeleteList(p->graveyard);
	free(p);
}
