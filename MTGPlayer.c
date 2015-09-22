#include "MTGPlayer.h"

extern CardData cd;

Permanent* NewPermanent(MTGCard* source,MTGPlayer* own) {
    Permanent* p = (Permanent*) malloc(sizeof(Permanent));
    p->is_tapped = false;
    p->has_attacked = false;
    p->has_blocked = false;
    p->subtypes = source->subtypes;
    if (source->subtypes.is_planeswalker)
        p->loyalty = source->loyalty;
    else if (source->subtypes.is_creature){
        p->power = source->power;
        p->toughness = source->toughness;
        p->has_summoning_sickness = true;
    }
    p->source = source;
    p->owner = own;
    p->controller = own;
    return p;
}

bool Permanent_sameColor(Permanent* p,Permanent* q) {
    return (p->subtypes.is_white && q->subtypes.is_white) ||
        (p->subtypes.is_blue && q->subtypes.is_blue) ||
        (p->subtypes.is_black && q->subtypes.is_black) ||
        (p->subtypes.is_red && q->subtypes.is_red) ||
        (p->subtypes.is_green && q->subtypes.is_blue);
}

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

bool MTGPlayer_drawCards(MTGPlayer* p,int num) {
    if (p->library->size < num)
        return false;
	for (int i=0;i<num;i++) {
		AppendToList(p->hand,p->library->entries[p->library->size-1-i]);
	}
	p->library->size -=num;
    return true;
}

bool MTGPlayer_playCard(MTGPlayer* player,int cardIndex, char* err) {
    MTGCard* card = (MTGCard*)player->hand->entries[cardIndex];
    if (! MTGPlayer_payMana(player, card)) {
        sprintf(err,"Not enough mana to play %s (%d/%d)",card->name,player->mana[0],card->cmc);
        return false;
    }
    if (card->subtypes.is_land && player->playedLand) {
        sprintf(err,"You can only play one Land per turn");
        return false;
    }
    
    //create permanent
    Permanent* permanent = NewPermanent(card,player);
        
    //apply card effects
    if (card->subtypes.is_land) {
        player->playedLand = true;
        AppendToList(player->lands, permanent);
    } else if (card->subtypes.is_creature || card->subtypes.is_planeswalker) {
        AppendToList(player->battlefield, permanent);
    }
    
    //remove card from hand
    RemoveListIndex(player->hand,cardIndex);
    
    return true;
}

void MTGPlayer_discard(MTGPlayer* player,int cardIndex) {
    AppendToList(player->graveyard, player->hand->entries[cardIndex]);
    //remove card from hand
    RemoveListIndex(player->hand, cardIndex);
}

void MTGPlayer_discardFromBattlefield(MTGPlayer* player,int cardIndex) {
    AppendToList(player->graveyard, player->battlefield->entries[cardIndex]);
    //remove card from battlefield
    RemoveListIndex(player->battlefield, cardIndex);
}


void MTGPlayer_tap(MTGPlayer* player,Permanent* permanent) {
    if (permanent->source->subtypes.is_land) {
        if (permanent->source == cd.Plains) player->mana[1]++;
        else if (permanent->source == cd.Island) player->mana[2]++;
        else if (permanent->source == cd.Swamp) player->mana[3]++;
        else if (permanent->source == cd.Mountain) player->mana[4]++;
        else if (permanent->source == cd.Forest) player->mana[5]++;
        player->mana[0]++;
    } else {
        if (permanent->source == cd.Soulmender)
            Event_gainLife(player, 1);
    }
    permanent->is_tapped = true;
}

bool MTGPlayer_payMana(MTGPlayer* player,MTGCard* card) {
    if (card->cmc > player->mana[0]) {
        return false;
    }
    for (int i=card->manaCost->size-1;i>=0;i--) {
        Manacost *cost = card->manaCost->entries[i];
        if (cost->isVariable) {
            
        } else if (cost->hasOption) {
            
        } else if (cost->color1 == COLORLESS) {
            if (cost->num == player->mana[0]) {
                memset(player->mana, 0, 6*sizeof(int));
            } else {
                int countNonzero=0;
                int nonzeroIndex=0;
                for (int i=1;i<=5;i++) {
                    if (player->mana[i] > 0) {
                        countNonzero++;
                        nonzeroIndex=i;
                    }
                }
                if (countNonzero == 1 && player->mana[nonzeroIndex] > cost->num) {
                    player->mana[nonzeroIndex] -= cost->num;
                    player->mana[0] -= cost->num;
                } else {
                    selectMana(player->mana,cost->num);
                }
            }
        } else { //one color
            if (player->mana[cost->color1] < cost->num)
                return false;
            else
                player->mana[cost->color1] -= cost->num;
            player->mana[0] -= cost->num;
        }
    }
    return true;
}

void MTGPlayer_refresh(MTGPlayer* player) {
    player->playedLand = false;
    for (unsigned int i=0;i<player->battlefield->size;i++) {
        Permanent* p = player->battlefield->entries[i];
        p->is_tapped = false;
        p->has_summoning_sickness = false;
    }
    for (unsigned int i=0;i<player->lands->size;i++) {
        Permanent* p = player->lands->entries[i];
        p->is_tapped = false;
    }
}

void MTGPlayer_restore(MTGPlayer* player) {
    memset(player->mana,0,6 * sizeof(int));
    for (unsigned int i=0;i<player->battlefield->size;i++) {
        Permanent* p = player->battlefield->entries[i];
        if (p->subtypes.is_creature) {
            p->power = p->source->power;
            p->toughness = p->source->toughness;
            p->has_attacked = false;
            p->has_blocked = false;
        }
    }
}

void DeleteMTGPlayer(MTGPlayer* p) {
	DeleteList(p->library);
	DeleteList(p->hand);
    DeleteList(p->graveyard);
    DeleteList(p->exile);
    for (unsigned int i=0;i<p->lands->size;i++)
        free(p->lands->entries[i]);
	DeleteList(p->lands);
    for (unsigned int i=0;i<p->battlefield->size;i++)
        free(p->battlefield->entries[i]);
	DeleteList(p->battlefield);
	free(p);
}
