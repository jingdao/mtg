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
List* attackSubscribers = NULL;
List* destroySubscribers = NULL;
List* bufferList = NULL;

MTGPlayer* findTarget(Permanent* p,int* index) {
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
    *index = -1;
    for (unsigned int i=0;i<player1->lands->size;i++) {
        Permanent* q = player1->lands->entries[i];
        if (p == q) return player1;
        (*index)--;
    }
    *index = -1;
    for (unsigned int i=0;i<player2->lands->size;i++) {
        Permanent* q = player2->lands->entries[i];
        if (p == q) return player2;
        (*index)--;
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
    sprintf(buffer,"%s lose %d life (%s)",player==player1?"You":"Opponent",num,source? source->name : "trample");
    message(buffer);
    
    for (unsigned int i=0;i<loseLifeSubscribers->size;i++) {
        Permanent* p = loseLifeSubscribers->entries[i];
        if (p->source == cd.FirstResponse && p->controller==player) {
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

bool Event_damage(Permanent* attacker, Permanent* defender,int num) {
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
        int index;
        MTGPlayer* p = findTarget(defender, &index);
        MTGPlayer_discardFromBattlefield(p, index, GRAVEYARD);
        return true;
    }
    return false;
}

bool Event_onPlay(Permanent* permanent) {
    if (permanent->selectedAbility)
        return Event_onPlayAbility(permanent);
    
    int index;
    MTGPlayer* targetPlayer;
    MTGCard* card = permanent->source;
    
    permanent->controller->hasCastSpell = true;
    void (*chooseTarget)(Permanent* source,char* allowedTargets);
    void (*choosePlayer)(Permanent* source);
    void (*chooseCards)(Permanent* source,List* cards,char* allowedTargets);
    void (*chooseOption)(Permanent* source,List* options);
    chooseTarget = permanent->controller==player1 ? selectTarget : AI_selectTarget;
    choosePlayer = permanent->controller==player1 ? selectPlayer : AI_selectPlayer;
    chooseCards = permanent->controller==player1 ? selectCards : AI_selectCards;
    chooseOption = permanent->controller==player1 ? selectOption : AI_selectOption;
    
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
            p->selectedAbility = 1;
            chooseTarget(p,"creature (-ve)");
        } else if (p->source == cd.NightfireGiant && currentPlayer == p->controller && permanent->subtypes.is_mountain &&!p->is_activated) {
            p->is_activated = true;
            p->power++;
            p->toughness++;
            p->bonusPower++;
            p->bonusToughness++;
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
             card == cd.KapshoKitefins || card == cd.TurntoFrog || card == cd.Plummet || card == cd.StabWound || card == cd.FleshtoDust ||
             card == cd.ClearaPath || card == cd.BlastfireBolt || card == cd.HeatRay)
        chooseTarget(permanent,"creature (-ve)");
    else if (card == cd.Encrust)
        chooseTarget(permanent,"creature or enchantment (-ve)");
    else if (card == cd.SolemnOffering || card == cd.ReclamationSage)
        chooseTarget(permanent, "artifact or enchantment (-ve)");
    else if (card == cd.PeelfromReality) {
        chooseTarget(permanent,"1st creature (+ve)");
        chooseTarget(permanent,"2nd creature (-ve)");
    } else if (card == cd.IntotheVoid) {
        chooseTarget(permanent,"1st creature (-ve)");
        chooseTarget(permanent,"2nd creature (-ve)");
    } else if (card == cd.ConeofFlame) {
        chooseTarget(permanent,"1st creature or player (-ve)");
        chooseTarget(permanent,"2nd creature or player (-ve)");
        chooseTarget(permanent,"3rd creature or player (-ve)");
    } else if (card == cd.Meteorite || card == cd.LightningStrike) {
        chooseTarget(permanent,"creature or player (-ve)");
    } else if (card == cd.SatyrWayfinder) {
        int n = permanent->controller->library->size < 4 ? permanent->controller->library->size : 4;
        memcpy(bufferList->entries,permanent->controller->library->entries+permanent->controller->library->size-n,n*sizeof(MTGCard*));
        permanent->controller->library->size -= 4;
        bufferList->size = n;
        chooseCards(permanent,bufferList,"a Land card");
    } else if (card == cd.Restock)
        chooseCards(permanent,permanent->controller->graveyard,"2 cards from graveyard");
    else if (card == cd.Gravedigger)
        chooseCards(permanent,permanent->controller->graveyard,"creature card from graveryard");
    else if (card == cd.Demolish)
        chooseTarget(permanent,"artifact or land (-ve)");
    else if (card == cd.SigninBlood)
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
    void (*chooseCards)(Permanent* source,List* cards,char* allowedTargets);
    void (*chooseOption)(Permanent* source,List* options);
    chooseTarget = permanent->controller==player1 ? selectTarget : AI_selectTarget;
    choosePlayer = permanent->controller==player1 ? selectPlayer : AI_selectPlayer;
    chooseCards = permanent->controller==player1 ? selectCards : AI_selectCards;
    chooseOption = permanent->controller==player1 ? selectOption : AI_selectOption;
    
    if (card == cd.BloodHost)
        chooseTarget(permanent,"creature (+ve)");
    else if (card == cd.Meteorite) {
        bufferList->size = 0;
        AppendToList(bufferList, "white");
        AppendToList(bufferList, "blue");
        AppendToList(bufferList, "black");
        AppendToList(bufferList, "red");
        AppendToList(bufferList, "green");
        chooseOption(permanent,bufferList);
    } else if (card == cd.NightfireGiant)
        chooseTarget(permanent,"creature or player (-ve)");
    else if (card == cd.TorchFiend)
        chooseTarget(permanent,"artifact");
    else return true;
    return false;
}

bool Event_onResolve(Permanent* permanent) {
    if (permanent->selectedAbility) {
        return Event_onAbility(permanent);
    }
    
    char buffer[128];
    int index;
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
    else if (card == cd.RoaringPrimadox || card == cd.IndulgentTormentor)
        AppendToList(upkeepSubscribers, permanent);
    else if (card == cd.ProfaneMemento)
        AppendToList(destroySubscribers, permanent);
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
            RemoveListObject(permanent->target->equipment, permanent);
            permanent->target = NULL;
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
            RemoveListObject(permanent->target->equipment, permanent);
            permanent->target = NULL;
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
            RemoveListObject(permanent->target->equipment, permanent);
            permanent->target = NULL;
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
    } else if (card == cd.StormtideLeviathan) {
        AppendToList(attackSubscribers, permanent);
        for (unsigned int i=0;i<player1->lands->size;i++) {
            Permanent* q = player1->lands->entries[i];
            q->subtypes.is_island = true;
        }
        for (unsigned int i=0;i<player2->lands->size;i++) {
            Permanent* q = player2->lands->entries[i];
            q->subtypes.is_island = true;
        }
        message("All lands are now Islands");
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
        int index2;
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
        unsigned long j=0;
        if (permanent->target) {
            j= permanent->target - permanent;
            card = bufferList->entries[j];
        }
        if (card && card->subtypes.is_land) {
            RemoveListIndex(bufferList, (unsigned int)j);
            AppendToList(permanent->controller->hand, card);
            sprintf(buffer,"%s draws %s to hand (%s)",permanent->controller==player1?"You":"Opponent",card->name,permanent->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
        AppendListToList(permanent->controller->graveyard,bufferList);
    } else if (permanent->source == cd.ShamanofSpring) {
        sprintf(buffer,"%s draw one card (%s)",permanent->controller==player1?"You":"Opponent",permanent->name);
        message(buffer);
        MTGPlayer_drawCards(permanent->controller, 1);
    } else if (permanent->source == cd.Restock) {
        if (permanent->target) {
            unsigned long j = permanent->target - permanent;
            MTGCard* card = RemoveListIndex(permanent->controller->graveyard, (unsigned int)j);
            AppendToList(permanent->controller->hand,card);
            sprintf(buffer,"%s is returned to hand (%s)",card->name,permanent->name);
            message(buffer);
        }
        if (permanent->target2) {
            unsigned long j = permanent->target2 - permanent - 1;
            MTGCard* card = RemoveListIndex(permanent->controller->graveyard, (unsigned int)j);
            AppendToList(permanent->controller->hand,card);
            sprintf(buffer,"%s is returned to hand (%s)",card->name,permanent->name);
            message(buffer);
        }
        RemoveListIndex(permanent->controller->graveyard,permanent->controller->graveyard->size-1);
        AppendToList(permanent->controller->exile, permanent->source);
        sprintf(buffer,"%s is exiled",permanent->name);
        message(buffer);
    } else if (permanent->source == cd.Plummet) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature && permanent->target->subtypes.is_flying) {
            sprintf(buffer,"%s is destroyed",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.Meteorite) {
        if (permanent->target == player1->marker) {
            Event_loseLife(permanent, player1, 2);
        } else if (permanent->target == player2->marker) {
            Event_loseLife(permanent, player2, 2);
        } else if (permanent->target) {
            if (findTarget(permanent->target,&index) && permanent->target->subtypes.is_creature) {
                sprintf(buffer,"%s is dealt 2 damage (%s)",permanent->target->name,permanent->name);
                message(buffer);
                Event_damage(permanent, permanent->target, 2);
            } else {
                sprintf(buffer,"Invalid target for %s",permanent->name);
                message(buffer);
            }
        } else {
            Event_loseLife(permanent, player2, 2);
        }
    } else if (permanent->source == cd.CarrionCrow) {
        permanent->is_tapped = true;
    } else if (permanent->source == cd.Gravedigger) {
        if (permanent->target) {
            unsigned long j = permanent->target - permanent;
            MTGCard* card = permanent->controller->graveyard->entries[j];
            if (card->subtypes.is_creature) {
                RemoveListIndex(permanent->controller->graveyard, (unsigned int)j);
                AppendToList(permanent->controller->hand,card);
                sprintf(buffer,"%s is returned to hand (%s)",card->name,permanent->name);
                message(buffer);
            } else {
                sprintf(buffer,"Invalid target for %s",permanent->name);
                message(buffer);
            }
        }
    } else if (permanent->source == cd.StabWound) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s gets -2/-2",permanent->target->name);
            message(buffer);
            permanent->target->power-=2;
            permanent->target->bonusPower-=2;
            permanent->target->bonusToughness-=2;
            Event_damage(NULL, permanent->target, 2);
            AppendToList(upkeepSubscribers, permanent);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
            RemoveListObject(permanent->target->equipment, permanent);
            permanent->target = NULL;
        }
    } else if (card == cd.NightfireGiant) {
        AppendToList(castSpellSubscribers, permanent);
        AppendToList(destroySubscribers, permanent);
        for (unsigned int i=0;i<permanent->controller->lands->size;i++) {
            Permanent* q = permanent->controller->lands->entries[i];
            if (q->subtypes.is_mountain) {
                permanent->is_activated = true;
                permanent->power++;
                permanent->toughness++;
                permanent->bonusPower++;
                permanent->bonusToughness++;
                break;
            }
        }
    } else if (card == cd.Demolish) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && (permanent->target->subtypes.is_artifact || permanent->target->subtypes.is_land)) {
            sprintf(buffer,"%s is destroyed",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.Festergloom) {
        for (unsigned int i=0;i<player1->battlefield->size;i++) {
            Permanent* p = player1->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_black) {
                p->power--;
                if (Event_damage(permanent, p, 1))
                    i--;
            }
        }
        for (unsigned int i=0;i<player2->battlefield->size;i++) {
            Permanent* p = player2->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_white) {
                p->power--;
                if (Event_damage(permanent, p, 1))
                    i--;
            }
        }
        sprintf(buffer,"All non-black creatures get -1/-1 (%s)",permanent->name);
        message(buffer);
    } else if (card == cd.FleshtoDust) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is destroyed",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.ClearaPath) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature && permanent->target->subtypes.is_defender) {
            sprintf(buffer,"%s is destroyed",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.ConeofFlame) {
        Permanent* targets[3] = {permanent->target,permanent->target2,permanent->target3};
        for (int i=0;i<3;i++) {
            if (targets[i] == player1->marker) {
                Event_loseLife(permanent, player1, i+1);
            } else if (targets[i] == player2->marker) {
                Event_loseLife(permanent, player2, i+1);
            } else if (targets[i]) {
                if (findTarget(targets[i],&index) && targets[i]->subtypes.is_creature) {
                    sprintf(buffer,"%s is dealt %d damage (%s)",targets[i]->name,i+1,permanent->name);
                    message(buffer);
                    Event_damage(permanent, targets[i], i+1);
                } else {
                    sprintf(buffer,"Invalid target for %s",permanent->name);
                    message(buffer);
                }
            } else {
                Event_loseLife(permanent, player2, i+1);
            }
        }
    } else if (card == cd.LightningStrike) {
        if (permanent->target == player1->marker) {
            Event_loseLife(permanent, player1, 3);
        } else if (permanent->target == player2->marker) {
            Event_loseLife(permanent, player2, 3);
        } else if (permanent->target) {
            if (findTarget(permanent->target,&index) && permanent->target->subtypes.is_creature) {
                sprintf(buffer,"%s is dealt 3 damage (%s)",permanent->target->name,permanent->name);
                message(buffer);
                Event_damage(permanent, permanent->target, 3);
            } else {
                sprintf(buffer,"Invalid target for %s",permanent->name);
                message(buffer);
            }
        } else {
            Event_loseLife(permanent, player2, 3);
        }
    } else if (card == cd.BlastfireBolt) {
        if (permanent->target && findTarget(permanent->target,&index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is dealt 5 damage (%s)",permanent->target->name,permanent->name);
            message(buffer);
            Event_damage(permanent, permanent->target, 5);
            for (unsigned int j=0;j<permanent->target->equipment->size;j++) {
                Permanent* q = permanent->target->equipment->entries[j];
                if (q->subtypes.is_equipment) {
                    sprintf(buffer,"%s is destroyed (%s)",q->name,permanent->name);
                    message(buffer);
                    RemoveListIndex(permanent->target->equipment, j--);
                    Event_onDestroy(q,GRAVEYARD);
                    AppendToList(q->owner->graveyard, q->source);
                    DeletePermanent(q);
                }
            }
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.HeatRay) {
        if (permanent->target && findTarget(permanent->target,&index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is dealt %d damage (%s)",permanent->target->name,permanent->X,permanent->name);
            message(buffer);
            Event_damage(permanent, permanent->target, permanent->X);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
        
    }
    if (!permanent->subtypes.is_aura && !permanent->subtypes.is_equipment) {
        permanent->target = NULL;
    }
    permanent->target2 = NULL;
    return true;
}

bool Event_onAbility(Permanent* permanent) {
    char buffer[128];
    int index;
    MTGPlayer* player;
    if (permanent->source == cd.Soulmender)
        Event_gainLife(permanent,permanent->controller, 1);
    else if (permanent->source == cd.WallofLimbs) {
        sprintf(buffer,"WallofLimbs is sacrificed");
        message(buffer);
        Event_loseLife(permanent, permanent->controller==player1?player2:player1, permanent->power);
        if ((player = findTarget(permanent, &index)))
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
    } else if (permanent->source == cd.CarnivorousMossBeast) {
        sprintf(buffer,"%s gets +1/+1",permanent->name);
        message(buffer);
        permanent->power++;
        permanent->toughness++;
        permanent->bonusPower++;
        permanent->bonusToughness++;
    } else if (permanent->source == cd.RoaringPrimadox) {
        Permanent* p = NULL;
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature && permanent->target->controller==permanent->controller)
            p = permanent->target;
        else {
            for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
                Permanent* q = permanent->controller->battlefield->entries[i];
                if (q->subtypes.is_creature && q->controller == permanent->controller) {
                    p = q;
                    break;
                }
            }
        }
        if (p) {
            player = findTarget(p, &index);
            sprintf(buffer,"%s is returned to hand (%s)",p->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(player, index, HAND);
        }
    } else if (permanent->source == cd.Meteorite) {
        if (permanent->target) {
            unsigned long j = permanent->target - permanent;
            permanent->controller->mana[j]++;
            permanent->controller->mana[0]++;
            sprintf(buffer,"%s get 1 %s mana (%s)",permanent->controller==player1?"You":"Opponent",bufferList->entries[j-1],permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.ZofShade) {
        sprintf(buffer,"%s gets +2/+2",permanent->name);
        message(buffer);
        permanent->power+=2;
        permanent->toughness+=2;
    }  else if (permanent->source == cd.IndulgentTormentor) {
        if (permanent->target2) {
            if ((player = findTarget(permanent->target2, &index)) && permanent->controller != permanent->target2->controller) {
                sprintf(buffer,"%s is destroyed",permanent->target2->name);
                message(buffer);
                MTGPlayer_discardFromBattlefield(player,index,GRAVEYARD);
            } else {
                MTGPlayer_drawCards(permanent->controller==player1?player1:player2, 1);
                sprintf(buffer,"%s draw 1 card",permanent->controller==player1?"You":"Opponent");
                message(buffer);
            }
        } else {
            unsigned long j = permanent->target ? permanent->target - permanent : 0;
            switch (j) {
                case 0:
                    MTGPlayer_drawCards(permanent->controller==player1?player1:player2, 1);
                    sprintf(buffer,"%s draw 1 card",permanent->controller==player1?"You":"Opponent");
                    message(buffer);
                    permanent->target = NULL;
                    break;
                case 1:
                    if(permanent->controller==player1) {
                        AI_selectTarget(permanent, "2 creature (+ve)");
                        if (permanent->target2) {
                            RemoveListObject(permanent->controller->battlefield, permanent);
                            AppendToList(stack, permanent);
                            break;
                        }
                    } else {
                        selectTarget(permanent, "2 creature (+ve)");
                        RemoveListObject(permanent->controller->battlefield, permanent);
                        AppendToList(stack, permanent);
                        break;
                    }
                default:
                    Event_loseLife(permanent,permanent->controller==player1?player2:player1, 3);
                    permanent->target = NULL;
            }
            return true;
        }
    } else if (permanent->source == cd.NightfireGiant) {
        if (permanent->target == player1->marker) {
            Event_loseLife(permanent, player1, 2);
        } else if (permanent->target == player2->marker) {
            Event_loseLife(permanent, player2, 2);
        } else if (permanent->target) {
            if (findTarget(permanent->target,&index) && permanent->target->subtypes.is_creature) {
                sprintf(buffer,"%s is dealt 2 damage (%s)",permanent->target->name,permanent->name);
                message(buffer);
                Event_damage(permanent, permanent->target, 2);
            } else {
                sprintf(buffer,"Invalid target for %s",permanent->name);
                message(buffer);
            }
        } else {
            Event_loseLife(permanent, player2, 2);
        }
    } else if (permanent->source == cd.TorchFiend) {
        if (permanent->target && (player=findTarget(permanent->target, &index)) && permanent->target->subtypes.is_artifact) {
            sprintf(buffer,"%s is destroyed (%s)",permanent->target->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(player, index, GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
        sprintf(buffer,"%s is sacrificed",permanent->name);
        message(buffer);
        if ((player = findTarget(permanent, &index)))
            MTGPlayer_discardFromBattlefield(player, index, GRAVEYARD);
        return false;
    }  else if (permanent->source == cd.WallofFire) {
        sprintf(buffer,"%s gets +1/+0",permanent->name);
        message(buffer);
        permanent->power++;
    } else if (permanent->source == cd.GargoyleSentinel) {
        sprintf(buffer,"%s loses defender and gains flying",permanent->name);
        message(buffer);
        permanent->subtypes.is_flying=true;
        permanent->subtypes.is_defender=false;
    }
    permanent->selectedAbility = 0;
    permanent->target = NULL;
    permanent->target2 = NULL;
    return true;
}

void Event_onDestroy(Permanent* p,Destination dest) {
    MTGCard* card = p->source;
    
    for (unsigned int i=0;i<destroySubscribers->size;i++) {
        Permanent* q = destroySubscribers->entries[i];
        if (q->source == cd.NightfireGiant && q->is_activated && p->subtypes.is_mountain && p->controller == q->controller) {
            q->is_activated = false;
            for (unsigned int j=0;j<p->controller->lands->size;j++) {
                Permanent* r = p->controller->lands->entries[j];
                if (r->subtypes.is_mountain && r!=p) {
                    q->is_activated = true;
                    break;
                }
            }
            if (!q->is_activated) {
                q->power--;
                q->toughness--;
                q->bonusPower--;
                q->bonusToughness--;
            }
        } else if (q->source == cd.ProfaneMemento && p->controller!=q->controller && p->subtypes.is_creature && dest==GRAVEYARD) {
            Event_gainLife(q, q->controller, 1);
        }
    }
    
    if (card == cd.AjanisPridemate || card == cd.WallofLimbs) {
        RemoveListObject(gainLifeSubscribers, p);
    } else if (card == cd.StaffoftheSunMagus || card == cd.StaffoftheDeathMagus || card == cd.KapshoKitefins) {
        RemoveListObject(castSpellSubscribers, p);
    } else if (card == cd.RoaringPrimadox || card == cd.IndulgentTormentor) {
        RemoveListObject(upkeepSubscribers, p);
    } else if (card == cd.FirstResponse) {
        RemoveListObject(upkeepSubscribers, p);
        RemoveListObject(loseLifeSubscribers,p);
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
    } else if (card == cd.StabWound) {
        p->target->bonusPower+=2;
        p->target->bonusToughness+=2;
        p->target->power+=2;
        p->target->toughness+=2;
        RemoveListObject(upkeepSubscribers, p);
    } else if (card == cd.NightfireGiant) {
        RemoveListObject(castSpellSubscribers, p);
        RemoveListObject(destroySubscribers, p);
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
            } else if (p->source == cd.RoaringPrimadox) {
                RemoveListObject(p->controller->battlefield, p);
                AppendToList(stack, p);
                p->selectedAbility = 1;
                selectTarget(p, "creature (+ve)");
            } else if (p->source == cd.IndulgentTormentor) {
                RemoveListObject(p->controller->battlefield, p);
                AppendToList(stack, p);
                p->selectedAbility = 1;
                bufferList->size = 0;
                AppendToList(bufferList, "Opponent draws a card");
                AppendToList(bufferList, "Sacrifice a creature");
                AppendToList(bufferList, "Lose 3 life");
                if (p->controller==player1) {
                    AI_selectOption(p, bufferList);
                    triggerSelect(bufferList->entries[p->target-p]);
                } else
                    selectOption(p, bufferList);
            }
        }
        if (p->source == cd.StabWound && p->target->controller == player) {
            Event_loseLife(p, player, 2);
        }
    }
}

bool Event_attack(List* attackers,char* err) {
    for (unsigned int i=0;i<attackSubscribers->size;i++) {
        Permanent* p = attackSubscribers->entries[i];
        if (p->source == cd.StormtideLeviathan) {
            for (unsigned int j=0;j<attackers->size;j++) {
                Permanent* q = attackers->entries[j];
                if (!q->subtypes.is_flying && !q->subtypes.is_islandwalk) {
                    sprintf(err,"Only creatures with islandwalk or flying can attack");
                    return false;
                }
            }
        }
    }
    return true;
}

void initEvents() {
    gainLifeSubscribers = InitList();
    loseLifeSubscribers = InitList();
    castSpellSubscribers = InitList();
    upkeepSubscribers = InitList();
    attackSubscribers = InitList();
    destroySubscribers = InitList();
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
    if (attackSubscribers)
        DeleteList(attackSubscribers);
    if (destroySubscribers)
        DeleteList(destroySubscribers);
    if (bufferList)
        DeleteList(bufferList);
}

