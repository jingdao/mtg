#include "AIPlayer.h"

bool DEBUG_AI = 0;
bool castedCreature = false;
bool castedSorcery = false;
bool castedAbility = false;
AIState state;
MTGPlayer* aiplayer;
extern CardData cd;
extern MTGPlayer* player1;
int X;

void AI_init(MTGPlayer* player) {
    state = AI_NONE;
    aiplayer = player;
}

void AI_getBlockers(List* attackerList, List* blockersList){
    char buffer[128];
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
    for (unsigned int j=0;j<attackerList->size;j++) {
        Permanent* attacker = attackerList->entries[j];
        List* blockers = blockersList->entries[j];
        if (!MTGPlayer_block(attacker, blockers, buffer))
            blockers->size = 0;
    }
}

bool AI_payMana(List* manaCost) {
    int manaBuffer[6];
    X=0;
    memcpy(manaBuffer,aiplayer->mana,6*sizeof(int));
    int j;
    for (j=manaCost->size - 1;j>=0;j--) {
        Manacost* cost = manaCost->entries[j];
        if (cost->isVariable) {
            X = manaBuffer[0];
            memset(manaBuffer, 0, 6*sizeof(int));
        } else if (cost->color1 == COLORLESS) {
            if (cost->num <= manaBuffer[0]) {
                int pendingMana = cost->num;
                int numColorless = manaBuffer[0]-(manaBuffer[1]+manaBuffer[2]+manaBuffer[3]+manaBuffer[4]+manaBuffer[5]);
                if (numColorless) {
                    if (numColorless > pendingMana) {
                        manaBuffer[0] -= pendingMana;
                        pendingMana = 0;
                    } else {
                        pendingMana -= numColorless;
                        manaBuffer[0] -= numColorless;
                    }
                }
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
        return true;
    } else return false;
}

void AI_discard(int num) {
    for (int i=0;i<num;i++) {
        int k = rand() % aiplayer->hand->size;
        MTGPlayer_discard(aiplayer, k);
    }
}

void AI_selectTarget(Permanent* source,char* allowedTargets) {
    unsigned long l = strlen(allowedTargets);
    MTGPlayer* targetPlayer = strncmp(allowedTargets+(l-5), "(+ve)", 5)==0 ? aiplayer : player1;
    if (strstr(allowedTargets,"player")) {
        if (allowedTargets[0] == '3')
            source->target3 = targetPlayer->marker;
        else if (allowedTargets[0] == '2')
            source->target2 = targetPlayer->marker;
        else
            source->target = targetPlayer->marker;
        return;
    }
    if (strstr(allowedTargets,"creature")) {
        for (int i=0;i<10;i++) {
            List* targetBattlefield = rand()%2 ? aiplayer->battlefield : player1->battlefield;
            if (targetBattlefield->size == 0)
                continue;
            int index = rand() % targetBattlefield->size;
            Permanent* p = targetBattlefield->entries[index];
            if (p->subtypes.is_creature && p->controller == targetPlayer) {
                if (allowedTargets[0] == '2')
                    source->target2 = p;
                else
                    source->target = p;
                return;
            }
        }
    }
    if (strstr(allowedTargets,"artifact")) {
        for (int i=0;i<10;i++) {
            List* targetBattlefield = rand()%2 ? aiplayer->battlefield : player1->battlefield;
            if (targetBattlefield->size == 0)
                continue;
            int index = rand() % targetBattlefield->size;
            Permanent* p = targetBattlefield->entries[index];
            if (p->subtypes.is_artifact && p->controller == targetPlayer) {
                source->target = p;
                return;
            }
            if (p->equipment) {
                for (unsigned int j=0;j<p->equipment->size;j++) {
                    Permanent* q = p->equipment->entries[j];
                    if (q->subtypes.is_artifact && p->controller == targetPlayer) {
                        source->target = q;
                        return;
                    }
                }
            }
        }
    }
    if (strstr(allowedTargets,"enchantment")) {
        for (int i=0;i<10;i++) {
            List* targetBattlefield = rand()%2 ? aiplayer->battlefield : player1->battlefield;
            if (targetBattlefield->size == 0)
                continue;
            int index = rand() % targetBattlefield->size;
            Permanent* p = targetBattlefield->entries[index];
            if (p->subtypes.is_enchantment && p->controller == targetPlayer) {
                source->target = p;
                return;
            }
            if (p->equipment) {
                for (unsigned int j=0;j<p->equipment->size;j++) {
                    Permanent* q = p->equipment->entries[j];
                    if (q->subtypes.is_enchantment && p->controller == targetPlayer) {
                        source->target = q;
                        return;
                    }
                }
            }
        }
    }
    if (strstr(allowedTargets,"land")) {
        for (int i=0;i<10;i++) {
            List* targetLands = rand()%2 ? aiplayer->lands : player1->lands;
            if (targetLands->size == 0)
                continue;
            int index = rand() % targetLands->size;
            Permanent* p = targetLands->entries[index];
            if (p->subtypes.is_land && p->controller == targetPlayer) {
                source->target = p;
                return;
            }
        }
    }
}

void AI_selectPlayer(Permanent* source) {
    MTGPlayer* targetPlayer;
    if (source->source == cd.SigninBlood)
        targetPlayer = player1;
    else
        targetPlayer = aiplayer;
    source->target = targetPlayer->marker;
}

void AI_selectCards(Permanent* permanent,List* cards,char* allowedTargets) {
    if (cards->size == 0)
        return;
    permanent->target = permanent + rand() % cards->size;
    if (allowedTargets[0] == '2')
        permanent->target2 = permanent + rand() % cards->size;
}

void AI_selectOption(Permanent* permanent,List* options) {
    permanent->target = permanent + rand() % options->size;
}

void AI_selectAbility(Permanent* permanent) {
    permanent->selectedAbility = rand() % permanent->source->abilities->size + 1;
}

void AI_getAction() {
    char buffer[128];
    state = (state + 1) % AI_NUMSTATES;
    if (state == AI_START) {
        castedCreature = false;
        castedSorcery = false;
        castedAbility = false;
        if (DEBUG_AI)
            displayHand(aiplayer->hand);
    }
    if (state == AI_LAND) {
        Permanent* permanent = NULL;
        for (unsigned int i=0;i<aiplayer->hand->size;i++) {
            MTGCard* card = aiplayer->hand->entries[i];
            if (card->subtypes.is_land) {
                permanent = NewPermanent(card,aiplayer);
                Event_onPlay(permanent);
                AppendToList(aiplayer->lands, permanent);
                aiplayer->playedLand = true;
                RemoveListIndex(aiplayer->hand, i);
                break;
            }
        }
        if (permanent) {
            sprintf(buffer,"Opponent played a %s",permanent->name);
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->graveyard->size,aiplayer->exile->size,aiplayer->mana,false);
            displayLands(aiplayer->lands, false);
            if (DEBUG_AI)
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
        Permanent* p = NULL;
        for (unsigned int i=0;i<aiplayer->hand->size;i++) {
            MTGCard* card = aiplayer->hand->entries[i];
            if (!card->subtypes.is_creature)
                continue;
            if ((p  = MTGPlayer_playCard(aiplayer, i, buffer))) {
                p->X = X;
                Event_onPlay(p);
                castedCreature = true;
                break;
            }
        }
        if (p) {
            int k = sprintf(buffer,"Opponent played %s",p->name);
            if (p->target && (p->target - p > 60 || p->target - p < 0))
                k += sprintf(buffer+k," (target=%s,",p->target->name);
            if (p->target2 && (p->target2 - p > 60 || p->target2 - p < 0))
                k += sprintf(buffer+k,"%s,",p->target->name);
            if (p->target3 && (p->target3 - p > 60 || p->target3 - p < 0))
                k += sprintf(buffer+k,"%s,",p->target->name);
            if (p->target && (p->target - p > 60 || p->target - p < 0))
                k += sprintf(buffer+k,")");
            message(buffer);
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->graveyard->size,aiplayer->exile->size,aiplayer->mana,false);
            if (DEBUG_AI)
                displayHand(aiplayer->hand);
            state--;
        } else if (!castedCreature){
            state = (state+1) % AI_NUMSTATES;
        }
    }
    if (state == AI_SORCERY) {
        Permanent* p = NULL;
        if (aiplayer->battlefield->size>0 || player1->battlefield->size>0) {
            for (unsigned int i=0;i<aiplayer->hand->size;i++) {
                MTGCard* card = aiplayer->hand->entries[i];
                if (card->subtypes.is_enchantment || card->subtypes.is_sorcery || card->subtypes.is_instant ||
                    card->subtypes.is_artifact || card->subtypes.is_planeswalker) {
                    if ((p  = MTGPlayer_playCard(aiplayer, i, buffer))) {
                        p->X = X;
                        Event_onPlay(p);
                        castedSorcery = true;
                        break;
                    }
                }
            }
        }
        if (p) {
            int k = sprintf(buffer,"Opponent played %s",p->name);
            if (p->target && (p->target - p > 60 || p->target - p < 0))
                k += sprintf(buffer+k," (target=%s,",p->target->name);
            if (p->target2 && (p->target2 - p > 60 || p->target2 - p < 0))
                k += sprintf(buffer+k,"%s,",p->target2->name);
            if (p->target3 && (p->target3 - p > 60 || p->target3 - p < 0))
                k += sprintf(buffer+k,"%s,",p->target3->name);
            if (p->target && (p->target - p > 60 || p->target - p < 0))
                k += sprintf(buffer+k,")");
            message(buffer);
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->graveyard->size,aiplayer->exile->size,aiplayer->mana,false);
            if (DEBUG_AI)
                displayHand(aiplayer->hand);
            state--;
        } else if (!castedSorcery){
            state = (state+1) % AI_NUMSTATES;
        }
    }
    if (state == AI_ABILITY) {
        bool used_ability=false;
        Permanent* p = NULL;
        for (unsigned int i=0;i<aiplayer->battlefield->size;i++) {
            p = aiplayer->battlefield->entries[i];
            if (p->subtypes.has_instant && rand() % 4) {
                Ability* a = p->source->abilities->entries[0];
                if (a->lifeCost == 0) {
                    MTGPlayer_tap(aiplayer, p);
                    if (MTGPlayer_activateAbility(aiplayer, p,buffer)) {
                            p->X = X;
                        Event_onPlay(p);
                        used_ability=true;
                        castedAbility=true;
                        break;
                    }
                }
            }
        }
        if (used_ability) {
            int k = sprintf(buffer,"Opponent activated %s",p->name);
            if (p->target && p->target->name)
                sprintf(buffer+k," (target=%s)",p->target->name);
            message(buffer);
            displayBattlefield(aiplayer->battlefield, false);
            state--;
        } else if (!castedAbility) {
            state = (state+1) % AI_NUMSTATES;
        }
    }
    if (state == AI_ATTACK) {
        List* permanentList = InitList();
        for (unsigned int i=0;i<aiplayer->battlefield->size;i++) {
            Permanent* p = aiplayer->battlefield->entries[i];
            if (p->subtypes.is_creature && (p->subtypes.is_haste||!p->has_summoning_sickness) && !p->is_tapped && !p->subtypes.is_defender && p->power>0) {
                if (p->source==cd.Juggernaut || (rand()%4)) {
                    p->has_attacked = true;
                    AppendToList(permanentList, p);
                }
            }
        }
        if (permanentList->size > 0) {
            if (Event_attack(aiplayer,permanentList,buffer)) {
                for (unsigned int i=0;i<permanentList->size;i++) {
                    Permanent* p = permanentList->entries[i];
                    if (!p->subtypes.is_vigilance)
                        p->is_tapped = true;
                }
                resolveAttack(aiplayer, permanentList);
            }
        } else {
            state = (state + 1) % AI_NUMSTATES;
        }
        DeleteList(permanentList);
    }
    if (state == AI_DISCARD) {
        if (aiplayer->hand->size > 7) {
            AI_discard(aiplayer->hand->size - 7);
            if (DEBUG_AI)
                displayHand(aiplayer->hand);
            message("Opponent discards down to 7 cards");
        } else
            state = (state + 1) % AI_NUMSTATES;
    }
    if (state == AI_NONE)
        newTurn();
}
