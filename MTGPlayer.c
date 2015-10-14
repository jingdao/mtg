#include "MTGPlayer.h"

extern CardData cd;
extern List* stack;
extern MTGPlayer* player1;
extern MTGPlayer* player2;

Permanent* NewPermanent(MTGCard* source,MTGPlayer* own) {
    Permanent* p = (Permanent*) calloc(1,sizeof(Permanent));
    p->name = source->name;
    p->subtypes = source->subtypes;
    if (source->subtypes.is_planeswalker)
        p->loyalty = source->loyalty;
    else if (source->subtypes.is_creature){
        p->power = p->sourcePower = source->power;
        p->toughness = p->sourceToughness = source->toughness;
        p->has_summoning_sickness = true;
        p->canAttack = true;
        p->equipment = InitList();
    }
    p->source = source;
    p->owner = own;
    p->controller = own;
    return p;
}

Permanent* NewCreatureToken(MTGPlayer* own,int pow,int tough,const char* nm) {
    Permanent* p = (Permanent*) calloc(1,sizeof(Permanent));
    p->name = nm;
    p->power = p->sourcePower = pow;
    p->toughness = p->sourceToughness = tough;
    p->owner = own;
    p->controller = own;
    p->has_summoning_sickness = true;
    p->canAttack = true;
    p->equipment = InitList();
    p->subtypes.is_creature = true;
    return p;
}

bool Permanent_sameColor(Permanent* p,Permanent* q) {
    return (p->subtypes.is_white && q->subtypes.is_white) ||
        (p->subtypes.is_blue && q->subtypes.is_blue) ||
        (p->subtypes.is_black && q->subtypes.is_black) ||
        (p->subtypes.is_red && q->subtypes.is_red) ||
        (p->subtypes.is_green && q->subtypes.is_blue);
}

void DeletePermanent(Permanent* permanent) {
    if (permanent->equipment)
        DeleteList(permanent->equipment);
    free(permanent);
    
}

MTGPlayer* InitMTGPlayer() {
	MTGPlayer* p = (MTGPlayer*)malloc(sizeof(MTGPlayer));
    p->marker = (Permanent*)calloc(1,sizeof(Permanent));
	p->library=InitList();
	p->hand=InitList();
    p->graveyard=InitList();
    p->exile=InitList();
    p->lands=InitList();
    p->battlefield=InitList();
    p->playedLand = false;
    memset(p->mana,0,6 * sizeof(int));
    p->hp=80;
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

Permanent* MTGPlayer_playCard(MTGPlayer* player,int cardIndex, char* err) {
    MTGCard* card = (MTGCard*)player->hand->entries[cardIndex];
    if (card == cd.IllusoryAngel && !player->hasCastSpell) {
        sprintf(err, "Can play IllusoryAngel only if you have casted another spell");
        return NULL;
    }
    if (player == player1 && !MTGPlayer_payMana(player, card->manaCost)) {
        sprintf(err,"Not enough mana to play %s (%d/%d)",card->name,player->mana[0],card->cmc);
        return NULL;
    } else if (player == player2 && !AI_payMana(card->manaCost))
        return NULL;
    if (card->subtypes.is_land && player->playedLand) {
        sprintf(err,"You can only play one Land per turn");
        return NULL;
    }
    
    
    //create permanent
    Permanent* permanent = NewPermanent(card,player);
        
    //apply card effects
    if (card->subtypes.is_land) {
        player->playedLand = true;
        AppendToList(player->lands, permanent);
    } else
        AppendToList(stack, permanent);
    
    //remove card from hand
    RemoveListIndex(player->hand,cardIndex);
    
    return permanent;
}

void MTGPlayer_discard(MTGPlayer* player,int cardIndex) {
    AppendToList(player->graveyard, player->hand->entries[cardIndex]);
    //remove card from hand
    RemoveListIndex(player->hand, cardIndex);
}

void MTGPlayer_discardFromBattlefield(MTGPlayer* player,int cardIndex,Destination dest) {
    Permanent* p;
    if (cardIndex >= 0)
        p = MTGPlayer_getBattlefieldPermanent(player->battlefield, cardIndex);
    else
        p = player->lands->entries[-cardIndex - 1];
    Event_onDestroy(p,dest);
    if (p->equipment) {
        for (unsigned int j=0;j<p->equipment->size;j++) {
            Permanent* q = p->equipment->entries[j];
            if (q->subtypes.is_equipment)
                AppendToList(q->owner->battlefield, q);
            else {
                Event_onDestroy(q,GRAVEYARD);
                AppendToList(q->owner->graveyard, q->source);
                DeletePermanent(q);
            }
        }
        DeleteList(p->equipment);
    }
    
    if (p->source)
        AppendToList(dest==EXILE?player->exile:dest==GRAVEYARD?player->graveyard:player->hand,p->source);
    //remove card from battlefield
    if (p->subtypes.is_aura || p->subtypes.is_equipment)
        RemoveListObject(p->target->equipment, p);
    else if (cardIndex >= 0)
        RemoveListObject(player->battlefield, p);
    else
        RemoveListIndex(player->lands, -cardIndex - 1);
    free(p);
}

bool MTGPlayer_tap(MTGPlayer* player,Permanent* permanent) {
    if (permanent->source->subtypes.is_land) {
        if (permanent->source == cd.Plains) player->mana[1]++;
        else if (permanent->source == cd.Island) player->mana[2]++;
        else if (permanent->source == cd.Swamp) player->mana[3]++;
        else if (permanent->source == cd.Mountain) player->mana[4]++;
        else if (permanent->source == cd.Forest) player->mana[5]++;
        player->mana[0]++;
        permanent->is_tapped = true;
        return false;
    } else {
        if (permanent->source->abilities->size > 1) {
            if (player == player1)
                selectAbility(permanent);
            else
                AI_selectAbility(permanent);
            return false;
        } else {
            permanent->selectedAbility = 1;
            return true;
        }
    }
}

bool MTGPlayer_activateAbility(MTGPlayer* player,Permanent* permanent,char* err) {
    Ability* a = permanent->source->abilities->entries[permanent->selectedAbility-1];
    if (a->needs_tap) {
        if (permanent->has_summoning_sickness) {
            sprintf(err, "%s has summoning sickness!",permanent->name);
            return false;
        } else if (permanent->is_tapped) {
            sprintf(err, "%s is already tapped!",permanent->name);
            return false;
        }
    }
    if (permanent->equipment) {
        for (unsigned int i=0;i<permanent->equipment->size;i++) {
            Permanent* q = permanent->equipment->entries[i];
            if (q->source == cd.Encrust) {
                sprintf(err,"%s cannot use abilities (%s)",permanent->name,q->name);
                return false;
            }
        }
    }
    if ((player==player1 && MTGPlayer_payMana(player,a->manaCost)) || (player==player2 && AI_payMana(a->manaCost))) {
        if (a->needs_tap)
            permanent->is_tapped = true;
        if (a->lifeCost > 0)
            Event_loseLife(permanent, player, a->lifeCost);
        RemoveListObject(player->battlefield, permanent);
        AppendToList(stack, permanent);
        return true;
    } else {
        sprintf(err,"Not enough mana to activate %s",permanent->name);
        return false;
    }
}

bool MTGPlayer_payMana(MTGPlayer* player,List* manaCost) {
    int manaBuffer[6];
    memcpy(manaBuffer, player->mana, 6*sizeof(int));
    int userSelect = 0;
    for (int i=manaCost->size-1;i>=0;i--) {
        Manacost *cost = manaCost->entries[i];
        if (cost->isVariable) {
            userSelect = -1;
        } else if (cost->hasOption) {
            
        } else if (cost->color1 == COLORLESS) {
            if (cost->num > manaBuffer[0])
                return false;
            else if (cost->num == manaBuffer[0]) {
                memset(manaBuffer, 0, 6*sizeof(int));
            } else {
                int countNonzero=0;
                int nonzeroIndex=0;
                for (int i=1;i<=5;i++) {
                    if (manaBuffer[i] > 0) {
                        countNonzero++;
                        nonzeroIndex=i;
                    }
                }
                if (countNonzero == 1 && manaBuffer[nonzeroIndex] > cost->num) {
                    manaBuffer[nonzeroIndex] -= cost->num;
                    manaBuffer[0] -= cost->num;
                } else {
                    userSelect = cost->num;
                }
            }
        } else { //one color
            if (manaBuffer[cost->color1] < cost->num)
                return false;
            else
                manaBuffer[cost->color1] -= cost->num;
            manaBuffer[0] -= cost->num;
        }
    }
    memcpy(player->mana, manaBuffer, 6*sizeof(int));
    if (userSelect)
        selectMana(manaBuffer, userSelect);
    return true;
}

bool MTGPlayer_block(Permanent* attacker,List* defenders,char* err) {
    if (attacker->subtypes.is_islandwalk && defenders->size>0) {
        Permanent* p = defenders->entries[0];
        for (unsigned int i=0;i<p->controller->lands->size;i++) {
            Permanent* q = p->controller->lands->entries[i];
            if (q->source->subtypes.is_island) {
                sprintf(err,"%s cannot be blocked (Islandwalk)",attacker->name);
                return false;
            }
        }
    }
    for (unsigned int i=0;i<defenders->size;i++) {
        Permanent* p = defenders->entries[i];
        for (unsigned int j=0;j<p->equipment->size;j++) {
            Permanent* q = p->equipment->entries[j];
            if (q->source == cd.CripplingBlight) {
                sprintf(err,"%s cannot block (%s)",p->name,q->name);
                return false;
            }
        }
    }
    return true;
}

Permanent* MTGPlayer_getBattlefieldPermanent(List* bt,unsigned int index) {
    unsigned int i,j=0;
    Permanent* p = NULL;
    for (i=0;i<bt->size;i++) {
        p = bt->entries[i];
        if (j==index)
            return p;
        j++;
        if (p->equipment) {
            if (j+p->equipment->size > index)
                return p->equipment->entries[index-j];
            else
                j+=p->equipment->size;
        }
    }
    return p;
}

void MTGPlayer_refresh(MTGPlayer* player) {
    memset(player->mana,0,6 * sizeof(int));
    player->playedLand = false;
    player->hasCastSpell = false;
    for (unsigned int i=0;i<player->battlefield->size;i++) {
        Permanent* p = player->battlefield->entries[i];
        if (p->is_untap_blocked)
            p->is_untap_blocked=false;
        else
            p->is_tapped = false;
        p->has_summoning_sickness = false;
    }
    for (unsigned int i=0;i<player->lands->size;i++) {
        Permanent* p = player->lands->entries[i];
        p->is_tapped = false;
    }
}

void MTGPlayer_restore(MTGPlayer* player) {
    for (unsigned int i=0;i<player->battlefield->size;i++) {
        Permanent* p = player->battlefield->entries[i];
        if (p->source)
            p->subtypes = p->source->subtypes;
        if (p->subtypes.is_creature) {
            p->power = p->sourcePower + p->bonusPower;
            p->toughness = p->sourceToughness + p->bonusToughness;
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
    for (unsigned int i=0;i<p->lands->size;i++) {
        free(p->lands->entries[i]);
    }
	DeleteList(p->lands);
    for (unsigned int i=0;i<p->battlefield->size;i++) {
        Permanent* q = p->battlefield->entries[i];
        if (q->equipment) {
            for (unsigned int j=0;j<q->equipment->size;j++)
                free(q->equipment->entries[j]);
            DeleteList(q->equipment);
        }
        free(q);
    }
	DeleteList(p->battlefield);
    free(p->marker);
	free(p);
}
