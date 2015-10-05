#include "EventHandler.h"

extern CardData cd;
extern MTGPlayer* player1;
extern MTGPlayer* player2;
extern MTGPlayer* currentPlayer;
extern List* stack;

List* gainLifeSubscribers = NULL;
List* loseLifeSubscribers = NULL;
List* castSpellSubscribers = NULL;
List* upkeepSubscribers = NULL;
List* bufferList = NULL;

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
    char buffer[128];
    sprintf(buffer,"%s gain %d life (%s)",player==player1?"You":"Opponent",num,source ? source->name : "lifelink");
    message(buffer);
    
    for (unsigned int i=0;i<gainLifeSubscribers->size;i++) {
        Permanent* p = gainLifeSubscribers->entries[i];
        if ((p->source == cd.AjanisPridemate || p->source == cd.WallofLimbs) && player == p->controller) {
            p->bonusPower++; p->bonusToughness++; p->power++; p->toughness++;
            sprintf(buffer,"%s gets +1/+1 counter",p->name);
            message(buffer);
            displayBattlefield(player->battlefield, player==player1);
        }
    }
    
    displayStats(player->hp,player->library->size,player->hand->size,player->graveyard->size,player->exile->size,player->mana,player==player1);
}

void Event_loseLife(Permanent* source, MTGPlayer* player,int num) {
    player->hp -= num;
    char buffer[128];
    sprintf(buffer,"%s lose %d life (%s)",player==player1?"You":"Opponent",num,source->name);
    message(buffer);
    
    for (unsigned int i=0;i<loseLifeSubscribers->size;i++) {
        Permanent* p = loseLifeSubscribers->entries[i];
        if (p->source == cd.FirstResponse) {
            p->is_activated = true;
        }
    }
    
    displayStats(player->hp,player->library->size,player->hand->size,player->graveyard->size,player->exile->size,player->mana,player==player1);
    if (player->hp <= 0 ) {
        if (player == player1)
            displayWinner(player2);
        else
            displayWinner(player1);
    }
}

void Event_damage(Permanent* attacker, Permanent* defender,int num) {
    char buffer[128];
    if (attacker && attacker->subtypes.is_deathtouch)
        defender->toughness = 0;
    else
        defender->toughness -= num;
    if (attacker && defender->source == cd.WallofEssence) {
        Event_gainLife(defender,defender->controller, num);
    }
    if (defender->toughness <= 0 && !defender->subtypes.is_indestructible) {
        sprintf(buffer,"%s is destroyed",defender->name);
        message(buffer);
        unsigned int index;
        MTGPlayer* p = findTarget(defender, &index);
        MTGPlayer_discardFromBattlefield(p, index, GRAVEYARD);
    }
}

bool Event_onPlay(Permanent* permanent) {
    if (permanent->selectedAbility)
        return Event_onPlayAbility(permanent);
    
    unsigned int index;
    MTGPlayer* targetPlayer;
    MTGCard* card = permanent->source;
    
    permanent->controller->hasCastSpell = true;
    void (*chooseTarget)(Permanent* source,char* allowedTargets);
    void (*choosePlayer)(Permanent* source);
    void (*chooseCards)(Permanent* source,List* cards,char* allowedTargets);
    chooseTarget = permanent->controller==player1 ? selectTarget : AI_selectTarget;
    choosePlayer = permanent->controller==player1 ? selectPlayer : AI_selectPlayer;
    chooseCards = permanent->controller==player1 ? selectCards : AI_selectCards;
    
    for (unsigned int i=0;i<castSpellSubscribers->size;i++) {
        Permanent* p = castSpellSubscribers->entries[i];
        if (p->source == cd.StaffoftheDeathMagus && currentPlayer == p->controller && (permanent->subtypes.is_black || permanent->subtypes.is_swamp)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.StaffoftheSunMagus && currentPlayer == p->controller && (permanent->subtypes.is_white || permanent->subtypes.is_plains)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.KapshoKitefins && currentPlayer == p->controller && permanent->subtypes.is_creature) {
            targetPlayer = findTarget(p, &index);
            RemoveListObject(targetPlayer->battlefield,p);
            AppendToList(stack, p);
            chooseTarget(p,"creature (-ve)");
        }
    }
    
    displayStack(stack);
    displayBattlefield(player1->battlefield, true);
    displayBattlefield(player2->battlefield, false);
    
    
    if (card->subtypes.is_land)
        return false;
    
    if (card == cd.KinsbaileSkirmisher || card == cd.DivineFavor || card == cd.MercurialPretender || card == cd.InvasiveSpecies)
        chooseTarget(permanent,"creature (+ve)");
    else if (card == cd.PillarofLight || card == cd.CripplingBlight || card == cd.Ulcerate || card==cd.FrostLynx ||
             card == cd.KapshoKitefins || card == cd.Encrust || card == cd.TurntoFrog)
        chooseTarget(permanent,"creature (-ve)");
    else if (card == cd.SolemnOffering || card == cd.ReclamationSage)
        chooseTarget(permanent, "artifact or enchantment (-ve)");
    
    else if (card == cd.PeelfromReality) {
        chooseTarget(permanent,"2nd creature (-ve)");
        chooseTarget(permanent,"1st creature (+ve)");
    } else if (card == cd.IntotheVoid) {
        chooseTarget(permanent,"2nd creature (-ve)");
        chooseTarget(permanent,"1st creature (-ve)");
    } else if (card == cd.SatyrWayfinder) {
        int n = permanent->controller->library->size < 4 ? permanent->controller->library->size : 4;
        memcpy(bufferList->entries,permanent->controller->library->entries+permanent->controller->library->size-n,n*sizeof(MTGCard*));
        permanent->controller->library->size -= 4;
        bufferList->size = n;
        chooseCards(permanent,bufferList,"a Land card");
    } else if (card == cd.SigninBlood)
        choosePlayer(permanent);
    else
        return true;

    return false;
}

bool Event_onPlayAbility(Permanent* permanent) {
    MTGCard* card = permanent->source;
    displayStack(stack);
    void (*chooseTarget)(Permanent* source,char* allowedTargets);
    void (*choosePlayer)(Permanent* source);
    chooseTarget = permanent->controller==player1 ? selectTarget : AI_selectTarget;
    choosePlayer = permanent->controller==player1 ? selectPlayer : AI_selectPlayer;
    
    if (card == cd.BloodHost)
        chooseTarget(permanent,"creature (+ve)");
    else return true;
    return false;
}

bool Event_onResolve(Permanent* permanent) {
    if (permanent->selectedAbility) {
        return Event_onAbility(permanent);
    }
    
    char buffer[128];
    unsigned int index;
    MTGPlayer* targetPlayer;
    MTGCard* card = permanent->source;

    if (card == cd.AjanisPridemate || card == cd.WallofLimbs)
        AppendToList(gainLifeSubscribers,permanent);
    else if (card == cd.TirelessMissionaries)
        Event_gainLife(permanent,permanent->controller,3);
    else if (card == cd.ResoluteArchangel && permanent->controller->hp < 20)
        Event_gainLife(permanent,permanent->controller,20-permanent->controller->hp);
    else if (card == cd.StaffoftheDeathMagus || card == cd.StaffoftheSunMagus)
        AppendToList(castSpellSubscribers, permanent);
    else if (card == cd.FirstResponse) {
        AppendToList(upkeepSubscribers,permanent);
        AppendToList(loseLifeSubscribers, permanent);
    } else if (permanent->source == cd.MassCalcify) {
        for (unsigned int i=0;i<player1->battlefield->size;i++) {
            Permanent* p = player1->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_white) {
                MTGPlayer_discardFromBattlefield(p->owner, i--,GRAVEYARD);
            }
        }
        for (unsigned int i=0;i<player2->battlefield->size;i++) {
            Permanent* p = player2->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_white) {
                MTGPlayer_discardFromBattlefield(p->owner, i--,GRAVEYARD);
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
            sprintf(buffer,"%s gets +1/+3",permanent->target->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.CripplingBlight) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s gets -1/-1 and cannot block",permanent->target->name);
            message(buffer);
            permanent->target->power--;
            permanent->target->bonusPower--;
            permanent->target->bonusToughness--;
            Event_damage(NULL, permanent->target, 1);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.PillarofLight) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature && permanent->target->toughness >= 4) {
            sprintf(buffer,"%s is exiled",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,EXILE);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.SolemnOffering) {
        Event_gainLife(permanent, permanent->controller, 4);
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && (permanent->target->subtypes.is_artifact || permanent->target->subtypes.is_enchantment)) {
            sprintf(buffer,"%s is destroyed",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.SigninBlood) {
        MTGPlayer* p = permanent->target == player1->marker ? player1 : player2;
        MTGPlayer_drawCards(p, 2);
        Event_loseLife(permanent, p, 2);
        sprintf(buffer,"%s draw 2 cards",p==player1?"You":"Opponent");
        message(buffer);
    } else if (permanent->source == cd.KinsbaileSkirmisher) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            permanent->target->power++;
            permanent->target->toughness++;
            sprintf(buffer,"%s gets +1/+1",permanent->target->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.Ulcerate) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s gets -3/-3",permanent->target->name);
            message(buffer);
            Event_loseLife(permanent, permanent->controller, 3);
            permanent->target->power-=3;
            Event_damage(NULL, permanent->target, 3);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.CoralBarrier) {
        Permanent* token = NewCreatureToken(permanent->controller,1,1,"Squid Token");
        token->subtypes.is_blue = true;
        token->subtypes.is_squid = true;
        token->subtypes.is_islandwalk = true;
        AppendToList(permanent->controller->battlefield,token);
        sprintf(buffer,"A blue Squid is summoned (%s)",permanent->name);
        message(buffer);
    } else if (permanent->source == cd.FrostLynx) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is tapped (%s)",permanent->target->name,permanent->name);
            message(buffer);
            permanent->target->is_tapped=true;
            permanent->target->is_untap_blocked=true;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.MercurialPretender) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature && permanent->target->controller==permanent->controller) {
            permanent->source = permanent->target->source;
            permanent->power = permanent->sourcePower = permanent->source->power;
            permanent->toughness = permanent->sourceToughness = permanent->source->toughness;
            permanent->subtypes = permanent->target->subtypes;
            sprintf(buffer,"%s entered the battlefield as %s",permanent->name,permanent->target->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
            targetPlayer = findTarget(permanent, &index);
            MTGPlayer_discardFromBattlefield(targetPlayer, index, GRAVEYARD);
            return false;
        }
    } else if (permanent->source == cd.Encrust) {
        if (permanent->target && findTarget(permanent->target, &index) && (permanent->target->subtypes.is_creature||permanent->target->subtypes.is_artifact)) {
            sprintf(buffer,"%s is tapped and cannot use abilities",permanent->target->name);
            message(buffer);
            permanent->target->is_tapped = true;
            permanent->target->is_untap_blocked = true;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.KapshoKitefins) {
        AppendToList(castSpellSubscribers, permanent);
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is tapped (%s)",permanent->target->name,permanent->name);
            message(buffer);
            permanent->target->is_tapped=true;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.IntotheVoid) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target, &index)) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is returned to hand (%s)",permanent->target->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer, index, HAND);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
        if (permanent->target2 && (targetPlayer=findTarget(permanent->target2, &index)) && permanent->target2->subtypes.is_creature) {
            sprintf(buffer,"%s is returned to hand (%s)",permanent->target2->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer, index, HAND);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.Negate) {
        Permanent* q = stack->size>0 ? stack->entries[stack->size-1] : NULL;
        if (q && !q->subtypes.is_creature) {
            sprintf(buffer,"%s is countered by %s",q->name,permanent->name);
            message(buffer);
            stack->size--;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.PeelfromReality) {
        unsigned int index2;
        if (permanent->target && findTarget(permanent->target,&index) &&
            permanent->target->subtypes.is_creature && permanent->target->controller==player1 &&
            permanent->target2 && findTarget(permanent->target2, &index2) &&
            permanent->target2->subtypes.is_creature && permanent->target2->controller==player2) {
            sprintf(buffer,"%s, %s are returned to hand (%s)",permanent->target->name,permanent->target2->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(player1, index, HAND);
            MTGPlayer_discardFromBattlefield(player2, index2, HAND);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.TurntoFrog) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            memset(&permanent->target->subtypes,0,sizeof(Subtypes));
            permanent->target->subtypes.is_blue = true;
            permanent->target->subtypes.is_frog = true;
            permanent->target->subtypes.is_creature = true;
            permanent->target->power = 1 + permanent->target->bonusPower;
            permanent->target->toughness = 1 + permanent->target->bonusToughness;
            sprintf(buffer,"%s becomes a blue Frog with 1/1 (%s)",permanent->target->name,permanent->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.InvasiveSpecies) {
        Permanent* p = NULL;
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature && permanent->target->controller==permanent->controller)
            p = permanent->target;
        else {
            for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
                Permanent* q = permanent->controller->battlefield->entries[i];
                if (q!=permanent && q->subtypes.is_creature && q->controller == permanent->controller) {
                    p = q;
                    break;
                }
            }
        }
        if (p) {
            targetPlayer = findTarget(p, &index);
            sprintf(buffer,"%s is returned to hand (%s)",p->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer, index, HAND);
        }
    } else if (permanent->source == cd.ReclamationSage) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && (permanent->target->subtypes.is_artifact || permanent->target->subtypes.is_enchantment)) {
            sprintf(buffer,"%s is destroyed",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.SatyrWayfinder) {
        MTGCard * card = NULL;
        if (permanent->target) {
            unsigned long index = permanent->target - permanent - 1;
            card = bufferList->entries[index];
        }
        if (card && card->subtypes.is_land) {
            RemoveListIndex(bufferList, (unsigned int)index);
            AppendToList(permanent->controller->hand, card);
            sprintf(buffer,"%s draws %s to hand (%s)",permanent->controller==player1?"You":"Opponent",card->name,permanent->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
        AppendListToList(permanent->controller->graveyard,bufferList);
        bufferList->size = 0;
    }
    return true;
}

bool Event_onAbility(Permanent* permanent) {
    char buffer[128];
    unsigned int index;
    MTGPlayer* player;
    if (permanent->source == cd.Soulmender)
        Event_gainLife(permanent,permanent->controller, 1);
    else if (permanent->source == cd.WallofLimbs) {
        sprintf(buffer,"WallofLimbs is sacrificed");
        message(buffer);
        Event_loseLife(permanent, permanent->controller==player1?player2:player1, permanent->power);
        player = findTarget(permanent, &index);
        MTGPlayer_discardFromBattlefield(player, index, GRAVEYARD);
        return false;
    } else if (permanent->source == cd.BloodHost) {
        if (permanent->target && (player=findTarget(permanent->target, &index)) && permanent->target->subtypes.is_creature && permanent->controller==permanent->target->controller) {
            sprintf(buffer,"%s is sacrificed,%s gets +1/+1",permanent->target->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(player, index, GRAVEYARD);
            Event_gainLife(permanent, permanent->controller, 2);
            permanent->toughness++;
            permanent->power++;
            permanent->bonusPower++;
            permanent->bonusToughness++;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.ShadowcloakVampire) {
        sprintf(buffer,"%s gains flying",permanent->name);
        message(buffer);
        permanent->subtypes.is_flying=true;
    } else if (permanent->source == cd.ResearchAssistant) {
        sprintf(buffer,"%s draw and discard one card",permanent->controller==player1?"You":"Opponent");
        message(buffer);
        MTGPlayer_drawCards(permanent->controller, 1);
        if (permanent->controller == player1)
            discardCards(player1,1);
        else
            AI_discard(1);
    } else if (permanent->source == cd.KapshoKitefins) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is tapped (%s)",permanent->target->name,permanent->name);
            message(buffer);
            permanent->target->is_tapped=true;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    }
    permanent->selectedAbility = 0;
    return true;
}

void Event_onDestroy(Permanent* p) {
    MTGCard* card = p->source;
    if (card == cd.AjanisPridemate || card == cd.WallofLimbs) {
        RemoveListObject(gainLifeSubscribers, p);
    } else if (card == cd.StaffoftheSunMagus || card == cd.StaffoftheDeathMagus || card == cd.KapshoKitefins) {
        RemoveListObject(castSpellSubscribers, p);
    } else if (card == cd.FirstResponse) {
        RemoveListObject(upkeepSubscribers, p);
    } else if (card == cd.DivineFavor) {
        p->target->bonusPower -= 1;
        p->target->bonusToughness -= 3;
        p->target->power -= 1;
        p->target->toughness -= 3;
    } else if (card == cd.CripplingBlight) {
        p->target->bonusPower++;
        p->target->bonusToughness++;
        p->target->power++;
        p->target->toughness++;
        p->target->canAttack=true;
    } else if (card == cd.Ulcerate) {
        p->target->bonusPower+=3;
        p->target->bonusToughness+=3;
        p->target->power+=3;
        p->target->toughness+=3;
    }
}

void Event_onUpkeep(MTGPlayer* player) {
    char buffer[128];
    for (unsigned int i=0;i<upkeepSubscribers->size;i++) {
        Permanent* p = upkeepSubscribers->entries[i];
        if (p->controller == player) {
            if (p->source == cd.FirstResponse){
                if (p->is_activated) {
                    Permanent* token = NewCreatureToken(player,1,1,"Soldier Token");
                    token->subtypes.is_white = true;
                    token->subtypes.is_soldier = true;
                    AppendToList(player->battlefield,token);
                    sprintf(buffer,"A white Soldier is summoned (%s)",p->name);
                    message(buffer);
                }
                p->is_activated = false;
            }
        }
    }
}

void initEvents() {
    gainLifeSubscribers = InitList();
    loseLifeSubscribers = InitList();
    castSpellSubscribers = InitList();
    upkeepSubscribers = InitList();
    bufferList = InitList();
}

void DeleteEvents() {
    if (gainLifeSubscribers)
        DeleteList(gainLifeSubscribers);
    if (loseLifeSubscribers)
        DeleteList(loseLifeSubscribers);
    if (castSpellSubscribers)
        DeleteList(castSpellSubscribers);
    if (upkeepSubscribers)
        DeleteList(upkeepSubscribers);
    if (bufferList)
        DeleteList(bufferList);
}

