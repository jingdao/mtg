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
List* refreshSubscribers = NULL;
List* restoreSubscribers = NULL;
List* combatSubscribers = NULL;
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

bool Event_damage(Permanent* defender,int num) {
    char buffer[128];
    defender->toughness -= num;
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

bool Event_combat(Permanent* attacker, Permanent* defender,int num) {
    char buffer[128];
    for (unsigned int i=0;i<combatSubscribers->size;i++) {
        Permanent* p = combatSubscribers->entries[i];
        if (p->source == cd.RoguesGloves && attacker==p->target && (defender==player1->marker||defender==player2->marker)) {
            MTGPlayer_drawCards(p->controller, 1);
            sprintf(buffer,"%s draw 1 card (%s)",p->controller->marker->name,p->name);
            message(buffer);
        }
    }
    if (defender == player1->marker) {
        Event_loseLife(attacker, player1, num);
        return false;
    }
    if (defender == player2->marker) {
        Event_loseLife(attacker, player2, num);
        return false;
    }
    if (attacker && attacker->subtypes.is_deathtouch)
        defender->toughness = 0;
    else
        defender->toughness -= num;
    if (defender->source == cd.WallofEssence) {
        Event_gainLife(defender,defender->controller, num);
    } else if (defender->source == cd.WallofFrost) {
        attacker->is_untap_blocked = true;
    }
    if (defender->toughness <= 0 && !defender->subtypes.is_indestructible) {
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
    char buffer[128];
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
        if (p->source == cd.StaffoftheDeathMagus && permanent->controller == p->controller && (permanent->subtypes.is_black || permanent->subtypes.is_swamp)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.StaffoftheSunMagus && permanent->controller == p->controller && (permanent->subtypes.is_white || permanent->subtypes.is_plains)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.StaffoftheFlameMagus && permanent->controller == p->controller && (permanent->subtypes.is_red || permanent->subtypes.is_mountain)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.StaffoftheMindMagus && permanent->controller == p->controller && (permanent->subtypes.is_blue || permanent->subtypes.is_island)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.StaffoftheWildMagus && permanent->controller == p->controller && (permanent->subtypes.is_green || permanent->subtypes.is_forest)) {
            Event_gainLife(p,currentPlayer, 1);
        } else if (p->source == cd.KapshoKitefins && permanent->controller == p->controller && permanent->subtypes.is_creature) {
            targetPlayer = findTarget(p, &index);
            RemoveListObject(targetPlayer->battlefield,p);
            AppendToList(stack, p);
            p->selectedAbility = 1;
            chooseTarget(p,"creature (-ve)");
        } else if (p->source == cd.NightfireGiant && permanent->controller == p->controller && permanent->subtypes.is_mountain &&!p->is_activated) {
            p->is_activated = true;
            p->power++;
            p->toughness++;
            p->bonusPower++;
            p->bonusToughness++;
        } else if (p->source == cd.SunbladeElf && permanent->controller == p->controller && permanent->subtypes.is_plains &&!p->is_activated) {
            p->is_activated = true;
            p->power++;
            p->toughness++;
            p->bonusPower++;
            p->bonusToughness++;
        } else if (p->source == cd.MidnightGuard && permanent->subtypes.is_creature) {
            p->is_untap_blocked = false;
            p->is_tapped = false;
        } else if (p->source == cd.SeraphoftheMasses && permanent->controller == p->controller && permanent->subtypes.is_creature) {
            p->power++;
            p->toughness++;
            p->bonusPower++;
            p->bonusToughness++;
        } else if (p->source == cd.AeronautTinkerer && permanent->controller == p->controller && permanent->subtypes.is_artifact && !p->is_activated) {
            p->is_activated = true;
            p->subtypes.is_flying = true;
            sprintf(buffer,"%s gains flying",p->name);
            message(buffer);
        } else if (p->source == cd.ScrapyardMongrel && permanent->controller == p->controller && permanent->subtypes.is_artifact && !p->is_activated) {
            p->is_activated = true;
            p->power+=2;
            p->bonusPower+=2;
            p->subtypes.is_trample = true;
            sprintf(buffer,"%s gains trample",p->name);
            message(buffer);
        }
    }
    
    displayStack(stack);
    displayBattlefield(player1->battlefield, true);
    displayBattlefield(player2->battlefield, false);
    
    
    if (!card || card->subtypes.is_land)
        return false;
    else
        permanent->controller->hasCastSpell = true;
    
    if (card == cd.KinsbaileSkirmisher || card == cd.DivineFavor || card == cd.MercurialPretender || card == cd.InvasiveSpecies || card == cd.LivingTotem || card == cd.GatherCourage || card == cd.TitanicGrowth)
        chooseTarget(permanent,"creature (+ve)");
    else if (card == cd.PillarofLight || card == cd.CripplingBlight || card == cd.Ulcerate || card==cd.FrostLynx ||
             card == cd.KapshoKitefins || card == cd.TurntoFrog || card == cd.Plummet || card == cd.StabWound || card == cd.FleshtoDust ||
             card == cd.ClearaPath || card == cd.BlastfireBolt || card == cd.HeatRay)
        chooseTarget(permanent,"creature (-ve)");
    else if (card == cd.Encrust)
        chooseTarget(permanent,"creature or enchantment (-ve)");
    else if (card == cd.SolemnOffering || card == cd.ReclamationSage || card == cd.Naturalize)
        chooseTarget(permanent, "artifact or enchantment (-ve)");
    else if (card == cd.EnsoulArtifact)
        chooseTarget(permanent,"artifact (+ve)");
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
    } else if (card == cd.ShrapnelBlast) {
        chooseTarget(permanent,"artifact (+ve)");
        chooseTarget(permanent,"2nd creature or player (-ve)");
    } else if (card == cd.Meteorite || card == cd.LightningStrike) {
        chooseTarget(permanent,"creature or player (-ve)");
    } else if (card == cd.SatyrWayfinder) {
        int n = permanent->controller->library->size < 4 ? permanent->controller->library->size : 4;
        memcpy(bufferList->entries,permanent->controller->library->entries+permanent->controller->library->size-n,n*sizeof(MTGCard*));
        permanent->controller->library->size -= 4;
        bufferList->size = n;
        chooseCards(permanent,bufferList,"a land card");
    } else if (card == cd.Restock)
        chooseCards(permanent,permanent->controller->graveyard,"2 cards from graveyard");
    else if (card == cd.Gravedigger)
        chooseCards(permanent,permanent->controller->graveyard,"creature card from graveryard");
    else if (card == cd.NissasExpedition)
        chooseCards(permanent,permanent->controller->library,"2 land card from library");
    else if (card == cd.HoardingDragon)
        chooseCards(permanent,permanent->controller->library,"artifact card from library");
    else if (card == cd.Demolish)
        chooseTarget(permanent,"artifact or land (-ve)");
    else if (card == cd.SigninBlood || card == cd.LavaAxe)
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
    
    if (permanent->subtypes.is_equipment)
        chooseTarget(permanent,"creature (+ve)");
    else if (card == cd.BloodHost || card == cd.WallofMulch || card == cd.SacredArmory ||
             (card==cd.AjaniSteadfast && permanent->selectedAbility==1))
        chooseTarget(permanent,"creature (+ve)");
    else if (card == cd.TyrantsMachine)
        chooseTarget(permanent,"creature (-ve)");
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
    else if (card == cd.StaffoftheDeathMagus || card == cd.StaffoftheSunMagus || card == cd.StaffoftheWildMagus ||
             card == cd.StaffoftheFlameMagus || card == cd.StaffoftheMindMagus|| card == cd.MidnightGuard)
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
            if (p->subtypes.is_creature && !p->subtypes.is_white && !p->subtypes.is_indestructible) {
                MTGPlayer_discardFromBattlefield(p->owner, i--,GRAVEYARD);
            }
        }
        for (unsigned int i=0;i<player2->battlefield->size;i++) {
            Permanent* p = player2->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_white && !p->subtypes.is_indestructible) {
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
            if (permanent->target)
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
            Event_damage(permanent->target, 1);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
            if (permanent->target)
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
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && (permanent->target->subtypes.is_artifact || permanent->target->subtypes.is_enchantment) && !permanent->subtypes.is_indestructible) {
            sprintf(buffer,"%s is destroyed (%s)",permanent->target->name,permanent->name);
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
            Event_damage(permanent->target, 3);
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
        Event_onPlay(token);
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
            AppendToList(refreshSubscribers, permanent);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
            if (permanent->target)
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
            permanent->target->subtypes.is_creature && permanent->target->controller==permanent->controller &&
            permanent->target2 && findTarget(permanent->target2, &index2) &&
            permanent->target2->subtypes.is_creature && permanent->target2->controller!=permanent->controller) {
            sprintf(buffer,"%s, %s are returned to hand (%s)",permanent->target->name,permanent->target2->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(permanent->controller, index, HAND);
            MTGPlayer_discardFromBattlefield(permanent->controller==player1?player2:player1, index2, HAND);
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
    } else if (permanent->source == cd.ReclamationSage || permanent->source == cd.Naturalize) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && (permanent->target->subtypes.is_artifact || permanent->target->subtypes.is_enchantment) && !permanent->subtypes.is_indestructible) {
            sprintf(buffer,"%s is destroyed (%s)",permanent->target->name,permanent->name);
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
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature && permanent->target->subtypes.is_flying && !permanent->subtypes.is_indestructible) {
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
                Event_damage(permanent->target, 2);
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
            Event_damage(permanent->target, 2);
            AppendToList(upkeepSubscribers, permanent);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
            if (permanent->target)
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
    } else if (card == cd.SunbladeElf) {
        AppendToList(castSpellSubscribers, permanent);
        AppendToList(destroySubscribers, permanent);
        for (unsigned int i=0;i<permanent->controller->lands->size;i++) {
            Permanent* q = permanent->controller->lands->entries[i];
            if (q->subtypes.is_plains) {
                permanent->is_activated = true;
                permanent->power++;
                permanent->toughness++;
                permanent->bonusPower++;
                permanent->bonusToughness++;
                break;
            }
        }
    } else if (card == cd.Demolish) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && (permanent->target->subtypes.is_artifact || permanent->target->subtypes.is_land) && !permanent->subtypes.is_indestructible) {
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
                if (Event_damage(p, 1))
                    i--;
            }
        }
        for (unsigned int i=0;i<player2->battlefield->size;i++) {
            Permanent* p = player2->battlefield->entries[i];
            if (p->subtypes.is_creature && !p->subtypes.is_black) {
                p->power--;
                if (Event_damage(p, 1))
                    i--;
            }
        }
        sprintf(buffer,"All non-black creatures get -1/-1 (%s)",permanent->name);
        message(buffer);
    } else if (card == cd.FleshtoDust) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature && !permanent->subtypes.is_indestructible) {
            sprintf(buffer,"%s is destroyed",permanent->target->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer,index,GRAVEYARD);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.ClearaPath) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target,&index)) && permanent->target->subtypes.is_creature && permanent->target->subtypes.is_defender && !permanent->subtypes.is_indestructible) {
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
                    Event_damage(targets[i], i+1);
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
                Event_damage(permanent->target, 3);
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
            for (unsigned int j=0;j<permanent->target->equipment->size;j++) {
                Permanent* q = permanent->target->equipment->entries[j];
                if (q->subtypes.is_equipment && !q->subtypes.is_indestructible) {
                    sprintf(buffer,"%s is destroyed (%s)",q->name,permanent->name);
                    message(buffer);
                    RemoveListIndex(permanent->target->equipment, j--);
                    Event_onDestroy(q,GRAVEYARD);
                    AppendToList(q->owner->graveyard, q->source);
                    DeletePermanent(q);
                }
            }
            Event_damage(permanent->target, 5);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.HeatRay) {
        if (permanent->target && findTarget(permanent->target,&index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is dealt %d damage (%s)",permanent->target->name,permanent->X,permanent->name);
            message(buffer);
            Event_damage(permanent->target, permanent->X);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.SeraphoftheMasses) {
        AppendToList(castSpellSubscribers, permanent);
        AppendToList(destroySubscribers, permanent);
        for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
            Permanent* q = permanent->controller->battlefield->entries[i];
            if (q->subtypes.is_creature) {
                permanent->power++;
                permanent->toughness++;
                permanent->bonusPower++;
                permanent->bonusToughness++;
            }
        }
    } else if (card == cd.TriplicateSpirits) {
            for (int i=0;i<3;i++) {
            Permanent* token = NewCreatureToken(permanent->controller,1,1,"Spirit Token");
            token->subtypes.is_white = true;
            token->subtypes.is_spirit = true;
            token->subtypes.is_flying = true;
            AppendToList(permanent->controller->battlefield,token);
            Event_onPlay(token);
        }
        sprintf(buffer,"3 white Spirits are summoned (%s)",permanent->name);
        message(buffer);
    } else if (card == cd.RaisetheAlarm) {
        for (int i=0;i<2;i++) {
            Permanent* token = NewCreatureToken(permanent->controller,1,1,"Soldier Token");
            token->subtypes.is_white = true;
            token->subtypes.is_soldier = true;
            AppendToList(permanent->controller->battlefield,token);
            Event_onPlay(token);
        }
        sprintf(buffer,"2 white Soldiers are summoned (%s)",permanent->name);
        message(buffer);
    } else if (card == cd.MeditationPuzzle) {
        Event_gainLife(permanent,permanent->controller, 8);
    } else if (card == cd.SanctifiedCharge) {
        for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
            Permanent* p = permanent->controller->battlefield->entries[i];
            if (p->subtypes.is_creature) {
                p->power+=2;
                p->toughness++;
                if (p->subtypes.is_white)
                    p->subtypes.is_first_strike = true;
            }
        }
        sprintf(buffer,"All %s creatures get +2/+1 (%s)",permanent->controller==player1?"your":"opponents",permanent->name);
        message(buffer);
        sprintf(buffer,"White creatures get first strike (%s)",permanent->name);
        message(buffer);
    } else if (card == cd.HornetQueen) {
        for (int i=0;i<4;i++) {
            Permanent* token = NewCreatureToken(permanent->controller,1,1,"Insect Token");
            token->subtypes.is_green = true;
            token->subtypes.is_insect = true;
            token->subtypes.is_flying = true;
            token->subtypes.is_deathtouch = true;
            AppendToList(permanent->controller->battlefield,token);
            Event_onPlay(token);
        }
        sprintf(buffer,"4 green Insects are summoned (%s)",permanent->name);
        message(buffer);
    } else if (permanent->source == cd.LivingTotem) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            permanent->target->bonusPower++;
            permanent->target->bonusToughness++;
            permanent->target->power++;
            permanent->target->toughness++;
            sprintf(buffer,"%s gets +1/+1",permanent->target->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.FeralIncarnation) {
        for (int i=0;i<3;i++) {
            Permanent* token = NewCreatureToken(permanent->controller,3,3,"Beast Token");
            token->subtypes.is_green = true;
            token->subtypes.is_beast = true;
            AppendToList(permanent->controller->battlefield,token);
            Event_onPlay(token);
        }
        sprintf(buffer,"3 green Beasts are summoned (%s)",permanent->name);
        message(buffer);
    } else if (permanent->source == cd.Overwhelm) {
        for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
            Permanent* p = permanent->controller->battlefield->entries[i];
            if (p->subtypes.is_creature) {
                p->power+=3;
                p->toughness+=3;
            }
        }
        sprintf(buffer,"All %s creatures get +3/+3 (%s)",permanent->controller==player1?"your":"opponents",permanent->name);
        message(buffer);
    } else if (permanent->source == cd.NissasExpedition) {
        unsigned long j = permanent->target - permanent;
        unsigned long k = permanent->target2 - permanent;
        if (permanent->target) {
            MTGCard* card = permanent->controller->library->entries[j];
            if (card->subtypes.is_land) {
                RemoveListIndex(permanent->controller->library, (unsigned int)j);
                Permanent* p = NewPermanent(card,permanent->controller);
                p->is_tapped = true;
                AppendToList(permanent->controller->lands,p);
                Event_onPlay(p);
                sprintf(buffer,"%s is added to battlefield (%s)",card->name,permanent->name);
                message(buffer);
                k--;
            }
        }
        if (permanent->target2) {
            MTGCard* card = permanent->controller->library->entries[k];
            if (card->subtypes.is_land) {
                RemoveListIndex(permanent->controller->library, (unsigned int)k);
                Permanent* p = NewPermanent(card,permanent->controller);
                p->is_tapped = true;
                AppendToList(permanent->controller->lands,p);
                Event_onPlay(p);
                sprintf(buffer,"%s is added to battlefield (%s)",card->name,permanent->name);
                message(buffer);
            }
        }
        shuffleDeck(permanent->controller->library);
    } else if (permanent->source == cd.GatherCourage) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            permanent->target->power+=2;
            permanent->target->toughness+=2;
            sprintf(buffer,"%s gets +2/+2",permanent->target->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.TitanicGrowth) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
            permanent->target->power+=4;
            permanent->target->toughness+=4;
            sprintf(buffer,"%s gets +4/+4",permanent->target->name);
            message(buffer);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (card == cd.AeronautTinkerer) {
        AppendToList(castSpellSubscribers, permanent);
        AppendToList(destroySubscribers, permanent);
        AppendToList(restoreSubscribers, permanent);
        Permanent* q;
        int k=0;
        while ((q = MTGPlayer_getBattlefieldPermanent(permanent->controller->battlefield, k++))) {
            if (q->subtypes.is_artifact) {
                permanent->is_activated = true;
                permanent->subtypes.is_flying = true;
                sprintf(buffer,"%s gains flying",permanent->name);
                message(buffer);
                break;
            }
        }
    } else if (card == cd.ScrapyardMongrel) {
        AppendToList(castSpellSubscribers, permanent);
        AppendToList(destroySubscribers, permanent);
        AppendToList(restoreSubscribers, permanent);
        Permanent* q;
        int k=0;
        while ((q = MTGPlayer_getBattlefieldPermanent(permanent->controller->battlefield, k++))) {
            if (q->subtypes.is_artifact) {
                permanent->is_activated = true;
                permanent->power+=2;
                permanent->bonusPower+=2;
                permanent->subtypes.is_trample = true;
                sprintf(buffer,"%s gains trample",permanent->name);
                message(buffer);
                break;
            }
        }
    } else if (permanent->source == cd.LavaAxe) {
        MTGPlayer* p = permanent->target == player1->marker ? player1 : player2;
        Event_loseLife(permanent, p, 5);
    } else if (permanent->source == cd.ShrapnelBlast) {
        if (permanent->target && (targetPlayer=findTarget(permanent->target, &index)) && permanent->target->subtypes.is_artifact) {
            sprintf(buffer,"%s is sacrificed (%s)",permanent->target->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(targetPlayer, index, GRAVEYARD);
            if (permanent->target2 == player1->marker) {
                Event_loseLife(permanent, player1, 5);
            } else if (permanent->target2 == player2->marker) {
                Event_loseLife(permanent, player2, 5);
            } else if (permanent->target2) {
                if (findTarget(permanent->target2,&index) && permanent->target2->subtypes.is_creature) {
                    sprintf(buffer,"%s is dealt 2 damage (%s)",permanent->target2->name,permanent->name);
                    message(buffer);
                    Event_damage(permanent->target2, 5);
                } else {
                    sprintf(buffer,"Invalid target for %s",permanent->name);
                    message(buffer);
                }
            } else {
                Event_loseLife(permanent, player2, 5);
            }
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.SiegeDragon) {
        targetPlayer = permanent->controller == player1 ? player2 : player1;
        for (unsigned int i=0;i<targetPlayer->battlefield->size;i++) {
            Permanent* p = targetPlayer->battlefield->entries[i];
            if (p->subtypes.is_creature && p->subtypes.is_wall && !p->subtypes.is_indestructible) {
                MTGPlayer_discardFromBattlefield(p->owner, i--,GRAVEYARD);
            }
        }
        sprintf(buffer, "All %s Walls are destroyed",targetPlayer==player1?"your":"opponents");
        message(buffer);
    } else if (permanent->source == cd.HoardingDragon) {
        unsigned long j = permanent->target - permanent;
        if (permanent->target) {
            MTGCard* card = permanent->controller->library->entries[j];
            if (card->subtypes.is_artifact) {
                RemoveListIndex(permanent->controller->library, (unsigned int)j);
                shuffleDeck(permanent->controller->library);
                Permanent* p = NewPermanent(card,permanent->controller);
                permanent->target = p;
                sprintf(buffer,"%s is exiled (%s)",card->name,permanent->name);
                message(buffer);
            } else {
                sprintf(buffer,"Invalid target for %s",permanent->name);
                message(buffer);
                permanent->target = NULL;
            }
        }
        return true;
    } else if (permanent->source == cd.EnsoulArtifact) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_artifact) {
            permanent->target->subtypes.is_creature = true;
            permanent->target->power=5;
            permanent->target->toughness=5;
            sprintf(buffer,"%s becomes a 5/5 creature (%s)",permanent->target->name,permanent->name);
            message(buffer);
            AppendToList(restoreSubscribers, permanent);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
            if (permanent->target)
                RemoveListObject(permanent->target->equipment, permanent);
            permanent->target = NULL;
        }
    }
    if (!permanent->subtypes.is_aura && !permanent->subtypes.is_equipment) {
        permanent->target = NULL;
    }
    permanent->target2 = NULL;
    permanent->target3 = NULL;
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
    } else if (permanent->source == cd.ResearchAssistant || permanent->source == cd.RummagingGoblin) {
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
            if ((player = findTarget(permanent->target2, &index)) && permanent->controller != permanent->target2->controller  && permanent->target2->subtypes.is_creature && !permanent->target2->subtypes.is_indestructible) {
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
                Event_damage(permanent->target, 2);
            } else {
                sprintf(buffer,"Invalid target for %s",permanent->name);
                message(buffer);
            }
        } else {
            Event_loseLife(permanent, player2, 2);
        }
    } else if (permanent->source == cd.TorchFiend) {
        if (permanent->target && (player=findTarget(permanent->target, &index)) && permanent->target->subtypes.is_artifact && !permanent->subtypes.is_indestructible) {
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
    }  else if (permanent->source == cd.SelflessCathar) {
        for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
            Permanent* p = permanent->controller->battlefield->entries[i];
            if (p->subtypes.is_creature) {
                p->power++;
                p->toughness++;
            }
        }
        sprintf(buffer,"All %s creatures get +1/+1 (%s)",permanent->controller==player1?"your":"opponents",permanent->name);
        message(buffer);
        sprintf(buffer,"%s is sacrificed",permanent->name);
        message(buffer);
        if ((player = findTarget(permanent, &index)))
            MTGPlayer_discardFromBattlefield(player, index, GRAVEYARD);
        return false;
    } else if (permanent->source == cd.SunbladeElf) {
        for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
            Permanent* p = permanent->controller->battlefield->entries[i];
            if (p->subtypes.is_creature) {
                p->power++;
                p->toughness++;
            }
        }
        sprintf(buffer,"All %s creatures get +1/+1 (%s)",permanent->controller==player1?"your":"opponents",permanent->name);
        message(buffer);
    } else if (permanent->source == cd.WallofMulch) {
        Permanent* p = NULL;
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature && permanent->target->controller==permanent->controller && permanent->target->subtypes.is_wall)
            p = permanent->target;
        else
            p = permanent;
        if ((player = findTarget(p, &index))) {
            MTGPlayer_drawCards(permanent->controller, 1);
            sprintf(buffer,"%s draw 1 card. %s is sacrificed (%s)",permanent->controller==player1?"You":"Opponent",p->name,permanent->name);
            message(buffer);
            MTGPlayer_discardFromBattlefield(player, index, GRAVEYARD);
        }
    } else if (permanent->source == cd.SacredArmory) {
        if (permanent->target && (player=findTarget(permanent->target, &index)) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s gets +1/+0 (%s)",permanent->target->name,permanent->name);
            message(buffer);
            permanent->target->power++;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.TyrantsMachine) {
        if (permanent->target && (player=findTarget(permanent->target, &index)) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is tapped (%s)",permanent->target->name,permanent->name);
            message(buffer);
            permanent->target->is_tapped = true;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else if (permanent->source == cd.BrawlersPlate) {
        if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature && permanent->target->controller == permanent->controller) {
            sprintf(buffer,"%s gets +2/+2 and has trample",permanent->target->name);
            message(buffer);
            permanent->target->power+=2;
            permanent->target->bonusPower+=2;
            permanent->target->bonusToughness+=2;
            permanent->target->toughness+=2;
            permanent->target->subtypes.is_trample=true;
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
            if (permanent->target) {
                RemoveListObject(permanent->target->equipment, permanent);
                AppendToList(permanent->owner->battlefield,permanent);
            }
            permanent->target = NULL;
        }
    } else if (permanent->source == cd.RoguesGloves) {
        AppendToList(combatSubscribers, permanent);
    } else if (permanent->source == cd.AjaniSteadfast) {
        if (permanent->selectedAbility == 1) {
            if (permanent->target && findTarget(permanent->target, &index) && permanent->target->subtypes.is_creature) {
                sprintf(buffer,"%s gets +1/+1 and has first_strike,vigilance,lifelink (%s)",permanent->target->name,permanent->name);
                message(buffer);
                permanent->target->power++;
                permanent->target->toughness++;
                permanent->target->subtypes.is_first_strike = true;
                permanent->target->subtypes.is_vigilance = true;
                permanent->target->subtypes.is_lifelink = true;
            } else {
                sprintf(buffer,"Invalid target for %s",permanent->name);
                message(buffer);
            }
        } else if (permanent->selectedAbility == 2) {
            for (unsigned int i=0;i<permanent->controller->battlefield->size;i++) {
                Permanent* p = permanent->controller->battlefield->entries[i];
                if (p->subtypes.is_creature) {
                    p->power++;
                    p->toughness++;
                    p->bonusPower++;
                    p->bonusToughness++;
                } else if (p->subtypes.is_planeswalker) {
                    p->loyalty++;
                }
            }
            sprintf(buffer,"All %s creatures/planeswalkers get +1/+1 (%s)",permanent->controller==player1?"your":"opponents",permanent->name);
            message(buffer);
        }
    }
    permanent->selectedAbility = 0;
    if (!permanent->subtypes.is_aura && !permanent->subtypes.is_equipment) {
        permanent->target = NULL;
    }
    permanent->target2 = NULL;
    permanent->target3 = NULL;
    return true;
}

void Event_onDestroy(Permanent* p,Destination dest) {
    MTGCard* card = p->source;
    char buffer[128];
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
        } else if (q->source == cd.SunbladeElf && q->is_activated && p->subtypes.is_plains && p->controller == q->controller) {
            q->is_activated = false;
            for (unsigned int j=0;j<p->controller->lands->size;j++) {
                Permanent* r = p->controller->lands->entries[j];
                if (r->subtypes.is_plains && r!=p) {
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
        } else if (q->source == cd.AeronautTinkerer && q->is_activated && p->subtypes.is_artifact && p->controller == q->controller) {
            Permanent* r;
            int k=0;
            q->is_activated = false;
            while ((r = MTGPlayer_getBattlefieldPermanent(q->controller->battlefield, k++))) {
                if (r->subtypes.is_artifact) {
                    q->is_activated = true;
                    break;
                }
            }
            if (!q->is_activated) {
                q->subtypes.is_flying = false;
                sprintf(buffer,"%s loses flying",q->name);
                message(buffer);
            }
        } else if (q->source == cd.ScrapyardMongrel && q->is_activated && p->subtypes.is_artifact && p->controller == q->controller) {
            Permanent* r;
            int k=0;
            q->is_activated = false;
            while ((r = MTGPlayer_getBattlefieldPermanent(q->controller->battlefield, k++))) {
                if (r->subtypes.is_artifact) {
                    q->is_activated = true;
                    break;
                }
            }
            if (!q->is_activated) {
                q->power-=2;
                q->bonusPower-=2;
                q->subtypes.is_trample = false;
                sprintf(buffer,"%s loses trample",q->name);
                message(buffer);
            }
        } else if (q->source == cd.ProfaneMemento && p->controller!=q->controller && p->subtypes.is_creature && dest==GRAVEYARD) {
            Event_gainLife(q, q->controller, 1);
        } else if (q->source == cd.SeraphoftheMasses && p->controller==q->controller && p->subtypes.is_creature) {
            q->power--;
            q->toughness--;
            q->bonusPower--;
            q->bonusToughness--;
        }
    }
    
    if (card == cd.AjanisPridemate || card == cd.WallofLimbs) {
        RemoveListObject(gainLifeSubscribers, p);
    } else if (card == cd.StaffoftheSunMagus || card == cd.StaffoftheDeathMagus || card == cd.StaffoftheWildMagus ||
               card == cd.StaffoftheFlameMagus || card == cd.StaffoftheMindMagus|| card == cd.KapshoKitefins || card == cd.MidnightGuard) {
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
    } else if (card == cd.NightfireGiant || card == cd.SeraphoftheMasses || card == cd.SunbladeElf) {
        RemoveListObject(castSpellSubscribers, p);
        RemoveListObject(destroySubscribers, p);
    } else if (card == cd.AeronautTinkerer || card == cd.ScrapyardMongrel) {
        RemoveListObject(castSpellSubscribers, p);
        RemoveListObject(destroySubscribers, p);
        RemoveListObject(restoreSubscribers, p);
    } else if (card == cd.Phytotitan) {
        Permanent* q = NewPermanent(card, p->owner);
        AppendToList(upkeepSubscribers, q);
    } else if (card == cd.Encrust)
        RemoveListObject(refreshSubscribers, p);
    else if (card == cd.RoguesGloves)
        RemoveListObject(combatSubscribers, p);
    else if (card == cd.HoardingDragon) {
        AppendToList(p->controller->battlefield,p->target);
        Event_onPlay(p->target);
        sprintf(buffer,"%s is added to battlefield (%s)",p->target->name,p->name);
        message(buffer);
    } else if (card == cd.EnsoulArtifact) {
        p->target->subtypes = p->target->source->subtypes;
        RemoveListObject(restoreSubscribers, p);
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
                    Event_onPlay(token);
                    sprintf(buffer,"A white Soldier is summoned (%s)",p->name);
                    message(buffer);
                }
                p->is_activated = false;
            } else if (p->source == cd.RoaringPrimadox) {
                RemoveListObject(p->controller->battlefield, p);
                AppendToList(stack, p);
                p->selectedAbility = 1;
                if (p->controller == player1)
                    selectTarget(p, "creature (+ve)");
                else
                    AI_selectTarget(p,"creature (+ve)");
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
            } else if (p->source == cd.Phytotitan) {
                RemoveListIndex(upkeepSubscribers, i--);
                if (RemoveListObject(p->controller->graveyard, p->source)) {
                    AppendToList(p->controller->battlefield, p);
                    p->is_tapped = true;
                    sprintf(buffer,"Phytotitan is returned to the battlefield");
                    message(buffer);
                }
            }
        }
        if (p->source == cd.StabWound && p->target->controller == player) {
            Event_loseLife(p, player, 2);
        }
    }
}

bool Event_attack(MTGPlayer *player,List* attackers,char* err) {
    char buffer[128];
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
    for (unsigned int i=0;i<attackers->size;i++) {
        Permanent* p = attackers->entries[i];
        if (p->source == cd.SiegeDragon) {
            bool hasWall = false;
            MTGPlayer* targetPlayer = p->controller==player1?player2:player1;
            for (unsigned int j=0;j<targetPlayer->battlefield->size;j++) {
                Permanent* q = targetPlayer->battlefield->entries[j];
                if (q->subtypes.is_wall) {
                    hasWall = true;
                    break;
                }
            }
            if (!hasWall) {
                sprintf(buffer,"%s dealt 2 damage to creatures without flying",p->name);
                message(buffer);
                for (unsigned int j=0;j<targetPlayer->battlefield->size;j++) {
                    Permanent* q = targetPlayer->battlefield->entries[j];
                    if (!q->subtypes.is_flying) {
                        Event_damage(q, 2);
                    }
                }
            }
        } else if (p->source == cd.GlacialCrasher) {
            bool hasMountain = false;
            for (unsigned int j=0;j<player1->lands->size;j++) {
                Permanent* q = player1->lands->entries[j];
                if (q->subtypes.is_mountain) {
                    hasMountain = true;
                    break;
                }
            }
            for (unsigned int j=0;j<player2->lands->size;j++) {
                Permanent* q = player2->lands->entries[j];
                if (q->subtypes.is_mountain) {
                    hasMountain = true;
                    break;
                }
            }
            if (!hasMountain) {
                sprintf(err,"%s can only attack with Mountain on the battlefield",p->name);
                return false;
            }
        }
    }
    for (unsigned int i=0;i<player->battlefield->size;i++) {
        Permanent* p = player->battlefield->entries[i];
        if (p->source==cd.Juggernaut && (p->subtypes.is_haste||!p->has_summoning_sickness) && !p->is_tapped && !p->subtypes.is_defender && !p->has_attacked) {
            sprintf(err,"%s must attack",p->name);
            return false;
        }
    }
    return true;
}

void Event_onRefresh(MTGPlayer* player) {
    for (unsigned int i=0;i<refreshSubscribers->size;i++) {
        Permanent* p = refreshSubscribers->entries[i];
        if (p->source == cd.Encrust)
            p->target->is_tapped = true;
    }
}

void Event_onRestore(MTGPlayer* player) {
    for (unsigned int i=0;i<restoreSubscribers->size;i++) {
        Permanent* p = restoreSubscribers->entries[i];
        if (p->source == cd.AeronautTinkerer) {
            if (p->is_activated)
                p->subtypes.is_flying = true;
        } else if (p->source == cd.ScrapyardMongrel) {
            if (p->is_activated)
                p->subtypes.is_trample = true;
        } else if (p->source == cd.EnsoulArtifact) {
            p->target->subtypes.is_creature = true;
            p->target->power = 5 + p->target->bonusPower;
            p->target->toughness = 5 + p->target->bonusToughness;
        }
    }
}

void initEvents() {
    gainLifeSubscribers = InitList();
    loseLifeSubscribers = InitList();
    castSpellSubscribers = InitList();
    upkeepSubscribers = InitList();
    attackSubscribers = InitList();
    destroySubscribers = InitList();
    refreshSubscribers = InitList();
    restoreSubscribers = InitList();
    combatSubscribers = InitList();
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
    if (refreshSubscribers)
        DeleteList(refreshSubscribers);
    if (restoreSubscribers)
        DeleteList(restoreSubscribers);
    if (combatSubscribers)
        DeleteList(combatSubscribers);
    if (bufferList)
        DeleteList(bufferList);
}

