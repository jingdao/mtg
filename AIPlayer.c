#include "AIPlayer.h"

AIState state;
MTGPlayer* aiplayer;

void AI_init(MTGPlayer* player) {
    state = AI_NONE;
    aiplayer = player;
}

void AI_getBlockers(List* attackerList, List* blockersList){
	for (unsigned int i=0;i<aiplayer->battlefield->size;i++) {
		Permanent* p = aiplayer->battlefield->entries[i];
		if (p->subtypes.is_creature && !p->is_tapped) {
            for (unsigned int k=0;k<attackerList->size;k++) {
                int j = rand() % attackerList->size;
                Permanent* attacker = attackerList->entries[j];
                if ((!attacker->subtypes.is_flying || p->subtypes.is_flying || p->subtypes.is_reach) &&
                    (!attacker->subtypes.is_intimidate || p->subtypes.is_artifact || Permanent_sameColor(attacker, p))) {
                    if (p->toughness > attacker->power || rand() % 2) {
                        List* blockers = blockersList->entries[j];
                        AppendToList(blockers,p);
                        break;
                    }
                }
            }
		}
	}

}

void AI_getAction() {
    char buffer[128];
    state = (state + 1) % AI_NUMSTATES;
    if (state == AI_START) {
        displayHand(aiplayer->hand);
    }
    if (state == AI_LAND) {
        Permanent* permanent = NULL;
        for (unsigned int i=0;i<aiplayer->hand->size;i++) {
            MTGCard* card = aiplayer->hand->entries[i];
            if (card->subtypes.is_land) {
                permanent = NewPermanent(card,aiplayer);
                AppendToList(aiplayer->lands, permanent);
                aiplayer->playedLand = true;
                RemoveListIndex(aiplayer->hand, i);
                break;
            }
        }
        if (permanent) {
            sprintf(buffer,"Opponent played a %s",permanent->source->name);
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->graveyard->size,aiplayer->exile->size,aiplayer->mana,false);
            displayLands(aiplayer->lands, false);
            displayHand(aiplayer->hand);
            message(buffer);
        } else {
            state = (state + 1) % AI_NUMSTATES;
        }
    }
    if (state == AI_TAP) {
        if (aiplayer->lands->size > 0) {
            for (unsigned int i=0;i<aiplayer->lands->size;i++) {
                Permanent* p = aiplayer->lands->entries[i];
                MTGPlayer_tap(aiplayer, p);
            }
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->graveyard->size,aiplayer->exile->size,aiplayer->mana,false);
            displayLands(aiplayer->lands, false);
            message("Opponent tapped all lands");
        } else {
            state = (state + 1) % AI_NUMSTATES;
        }
    }
    if (state == AI_CREATURE) {
        int num_creature = 0;
        int manaBuffer[6];
        for (unsigned int i=0;i<aiplayer->hand->size;i++) {
            memcpy(manaBuffer,aiplayer->mana,6*sizeof(int));
            MTGCard* card = aiplayer->hand->entries[i];
            if (!card->subtypes.is_creature)
                continue;
            int j;
            for (j=card->manaCost->size - 1;j>=0;j--) {
                Manacost* cost = card->manaCost->entries[j];
                if (cost->color1 == COLORLESS) {
                    if (cost->num <= manaBuffer[0]) {
                        int pendingMana = cost->num;
                        while (pendingMana > 0) {
                            int k = rand() % 5 + 1;
                            if (manaBuffer[k] > 0) {
                                manaBuffer[k]--;
                                manaBuffer[0]--;
                                pendingMana--;
                            }
                        }
                    } else {
                        break;
                    }
                } else {
                    if (manaBuffer[cost->color1] >= cost->num) {
                        manaBuffer[cost->color1] -= cost->num;
                        manaBuffer[0] -= cost->num;
                    } else
                        break;
                }
            }
            if (j<0) {
                memcpy(aiplayer->mana, manaBuffer, 6*sizeof(int));
                num_creature ++;
                Permanent* p = NewPermanent(card,aiplayer);
                AppendToList(aiplayer->battlefield, p);
                RemoveListIndex(aiplayer->hand, i);
                i--;
            }
        }
        if (num_creature > 0) {
            sprintf(buffer,"Opponent played %d creatures",num_creature);
            message(buffer);
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->graveyard->size,aiplayer->exile->size,aiplayer->mana,false);
            displayBattlefield(aiplayer->battlefield, false);
            displayHand(aiplayer->hand);
        } else {
            state = (state+1) % AI_NUMSTATES;
        }
    }
    if (state == AI_ATTACK) {
        List* permanentList = InitList();
        for (unsigned int i=0;i<aiplayer->battlefield->size;i++) {
            Permanent* p = aiplayer->battlefield->entries[i];
            if ((p->subtypes.is_haste||!p->has_summoning_sickness) && !p->is_tapped && !p->subtypes.is_defender && p->power>0 && (rand()%4)) {
                p->has_attacked = true;
                if (!p->subtypes.is_vigilance)
                    p->is_tapped = true;
                AppendToList(permanentList, p);
            }
        }
        if (permanentList->size > 0) {
            resolveAttack(aiplayer, permanentList);
        } else {
            state = (state + 1) % AI_NUMSTATES;
        }
        DeleteList(permanentList);
    }
    if (state == AI_DISCARD) {
        if (aiplayer->hand->size > 7) {
            while (aiplayer->hand->size > 7) {
                int k = rand() % aiplayer->hand->size;
                MTGPlayer_discard(aiplayer, k);
            }
            displayHand(aiplayer->hand);
            message("Opponent discards down to 7 cards");
        } else
            state = (state + 1) % AI_NUMSTATES;
    }
    if (state == AI_NONE)
        newTurn();
}
