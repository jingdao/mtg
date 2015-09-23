#include "EventHandler.h"

extern CardData cd;
extern MTGPlayer* player1;
extern MTGPlayer* player2;
extern MTGPlayer* currentPlayer;
extern List* stack;

List* gainLifeSubscribers = NULL;
List* castSpellSubscribers = NULL;

MTGPlayer* findTarget(Permanent* p,unsigned int* index) {
    *index = 0;
    for (unsigned int i=0;i<player1->battlefield->size;i++) {
        Permanent* q = player1->battlefield->entries[i];
        if (p == q) return player1;
        (*index)++;
        if (q->equipment) {
            for (unsigned int j=0;j<q->equipment->size;j++) {
                if (q->equipment->entries[j] == p) return player1;
                (*index)++;
            }
        }
    }
    *index = 0;
    for (unsigned int i=0;i<player2->battlefield->size;i++) {
        Permanent* q = player2->battlefield->entries[i];
        if (p == q) return player2;
        (*index)++;
        if (q->equipment) {
            for (unsigned int j=0;j<q->equipment->size;j++) {
                if (q->equipment->entries[j] == p) return player2;
                (*index)++;
            }
        }
    }
    return NULL;
}


void Event_gainLife(Permanent* source, MTGPlayer* player,int num) {
    player->hp += num;
    char buffer[64];
    if (source) {
        sprintf(buffer,"%s gain %d life (%s)",player==player1?"You":"Opponent",num,source->source->name);
        message(buffer);
    }
    
    for (unsigned int i=0;i<gainLifeSubscribers->size;i++) {
        Permanent* p = gainLifeSubscribers->entries[i];
        if ((p->source == cd.AjanisPridemate || p->source == cd.WallofLimbs) && player == p->controller) {
            p->bonusPower++; p->bonusToughness++; p->power++; p->toughness++;
            sprintf(buffer,"%s gets +1/+1 counter",p->source->name);
            message(buffer);
            displayBattlefield(player->battlefield, player==player1);
        }
    }
    
    displayStats(player->hp,player->library->size,player->hand->size,player->graveyard->size,player->exile->size,player->mana,player==player1);
}

void Event_damage(Permanent* attacker, Permanent* defender,int num) {
    if (attacker->subtypes.is_deathtouch)
        defender->toughness = 0;
    else
        defender->toughness -= num;
    if (defender->source == cd.WallofEssence) {
        Event_gainLife(defender,defender->controller, num);
    }
}

bool Event_onPlay(Permanent* permanent) {
    MTGCard* card = permanent->source;
    for (unsigned int i=0;i<castSpellSubscribers->size;i++) {
        Permanent* p = castSpellSubscribers->entries[i];
        if (p->source == cd.StaffoftheDeathMagus && currentPlayer == p->controller && (permanent->subtypes.is_black || permanent->subtypes.is_swamp)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.StaffoftheSunMagus && currentPlayer == p->controller && (permanent->subtypes.is_white || permanent->subtypes.is_plains)) {
            Event_gainLife(p,currentPlayer, 1);
        }
    }
    
    displayStack(stack);
    
    if (card == cd.AjanisPridemate || card == cd.WallofLimbs)
        AppendToList(gainLifeSubscribers,permanent);
    else if (card == cd.KinsbaileSkirmisher)
        selectCallback(KinsbaileSkirmisher_onPlay,"creature");
    else if (card == cd.TirelessMissionaries)
        Event_gainLife(permanent,permanent->controller,3);
    else if (card == cd.ResoluteArchangel && permanent->controller->hp < 20)
        Event_gainLife(permanent,permanent->controller,20-permanent->controller->hp);
    else if (card == cd.StaffoftheDeathMagus || card == cd.StaffoftheSunMagus)
        AppendToList(castSpellSubscribers, permanent);
    else if (card == cd.DivineFavor || card == cd.PillarofLight)
        selectTarget(permanent,"creature");
    else if (card == cd.SolemnOffering)
        selectTarget(permanent, "artifact or enchantment");
    else if (card->subtypes.is_enchantment || card->subtypes.is_sorcery || card->subtypes.is_instant)
        return true;
    
    return false;

}

void Event_onResolve(Permanent* permanent) {
    char buffer[64];
    unsigned int index;
    MTGPlayer* targetPlayer;

    if (permanent->source == cd.MassCalcify) {
        for (unsigned int i=0;i<player1->battlefield->size;i++) {
            Permanent* p = player1->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_white) {
                MTGPlayer_discardFromBattlefield(p->owner, i--,false);
            }
        }
        for (unsigned int i=0;i<player2->battlefield->size;i++) {
            Permanent* p = player2->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_white) {
                MTGPlayer_discardFromBattlefield(p->owner, i--,false);
            }
        }
        message("All non-white creatures are destroyed");
    } else if (permanent->source == cd.DivineFavor) {
        Event_gainLife(permanent, permanent->controller, 3);
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            permanent->target->power++;
            permanent->target->toughness+=3;
            permanent->target->bonusPower++;
            permanent->target->bonusToughness+=3;
            sprintf(buffer,"%s gets +1/+3",permanent->target->source->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->source->name);
            message(buffer);
        }
    } else if (permanent->source == cd.PillarofLight) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature && permanent->target->toughness >= 4) {
            MTGPlayer_discardFromBattlefield(permanent->target->controller,index,true);
            sprintf(buffer,"%s is exiled",permanent->target->source->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->source->name);
            message(buffer);
        }
    } else if (permanent->source == cd.SolemnOffering) {
        Event_gainLife(permanent, permanent->controller, 4);
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && (permanent->target->subtypes.is_artifact || permanent->target->subtypes.is_enchantment)) {
            MTGPlayer_discardFromBattlefield(permanent->target->controller,index,false);
            sprintf(buffer,"%s is destroyed",permanent->target->source->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->source->name);
            message(buffer);
        }
    }
}

void Event_tapAbility(Permanent* permanent,int index) {
    if (permanent->source == cd.Soulmender)
        Event_gainLife(permanent,permanent->controller, 1);
}

void Event_onDestroy(Permanent* p) {
    MTGCard* card = p->source;
    if (card == cd.AjanisPridemate || card == cd.WallofLimbs) {
        RemoveListObject(gainLifeSubscribers, p);
    } else if (card == cd.StaffoftheSunMagus || card == cd.StaffoftheDeathMagus) {
        RemoveListObject(castSpellSubscribers, p);
    } else if (card == cd.DivineFavor) {
        p->target->bonusPower -= 1;
        p->target->bonusToughness -= 3;
        p->target->power -= 1;
        p->target->toughness -= 3;
    }
}

void initEvents() {
    gainLifeSubscribers = InitList();
    castSpellSubscribers = InitList();
}

void DeleteEvents() {
    if (gainLifeSubscribers)
        DeleteList(gainLifeSubscribers);
    if (castSpellSubscribers)
        DeleteList(castSpellSubscribers);
}

