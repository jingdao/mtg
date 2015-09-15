#include "AIPlayer.h"

AIState state = AI_NONE;
MTGPlayer* aiplayer;

void AI_init(MTGPlayer* player) {
    aiplayer = player;
}

void AI_getAction() {
    char buffer[128];
    state = (state + 1) % AI_NUMSTATES;
    if (state == AI_START) {
        
    }
    if (state == AI_LAND) {
        Permanent* permanent = NULL;
        for (unsigned int i=0;i<aiplayer->hand->size;i++) {
            MTGCard* card = aiplayer->hand->entries[i];
            if (card->is_land) {
                permanent = NewPermanent(card);
                AppendToList(aiplayer->lands, permanent);
                aiplayer->playedLand = true;
                memmove(aiplayer->hand->entries+i,aiplayer->hand->entries+i+1,(aiplayer->hand->size-1-i)*sizeof(void*));
                aiplayer->hand->size--;
                break;
            }
        }
        if (permanent) {
            sprintf(buffer,"Opponent played a %s",permanent->source->name);
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->mana,false);
            displayLands(aiplayer->lands, false);
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
            displayStats(aiplayer->hp,aiplayer->library->size,aiplayer->hand->size,aiplayer->mana,false);
            displayLands(aiplayer->lands, false);
            message("Opponent tapped all lands");
        } else {
            state = (state + 1) % AI_NUMSTATES;
        }
    }
    if (state == AI_CREATURE) {
        message("Opponent played a creature");
    }
    if (state == AI_ATTACK) {
        message("Opponent is attacking you");
    }
    if (state == AI_NONE) {
        newTurn();
    }
}