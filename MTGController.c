#include "MTGController.h"

extern CardData cd;
extern HashTable* cdt;
extern List* categories;
extern bool DEBUG_AI;
MTGPlayer* player1;
MTGPlayer* player2;
MTGPlayer* currentPlayer;
List* stack;

List* blockersList = NULL;
List* attackerList = NULL;
int block_index;

void loadCardDataTable() {
    categories = InitList();
    for (int i=0;i<14;i++) {
        List* l = InitList();
        AppendToList(categories, l);
    }
    
	cdt = InitHashTable();
	int numCards = sizeof(CardData) / sizeof(MTGCard*);
	MTGCard** cardArray = & cd.AbbotofKeralKeep;
	for (int i=0;i<numCards;i++) {
        MTGCard* card = *cardArray;
		HashTable_insertVar(cdt,(void*)card->name, (int) strlen(card->name), card);
        List* l;
        int k=0;
        if (card->subtypes.is_plains||card->subtypes.is_island||card->subtypes.is_swamp||card->subtypes.is_mountain||card->subtypes.is_forest)
            k = 13;
        else if (card->subtypes.is_land) k = 12;
        else if (card->subtypes.is_white + card->subtypes.is_blue + card->subtypes.is_black + card->subtypes.is_red + card->subtypes.is_green == 1) {
            if (card->subtypes.is_white) k += 0;
            else if (card->subtypes.is_blue) k += 2;
            else if (card->subtypes.is_black) k += 4;
            else if (card->subtypes.is_red) k += 6;
            else if (card->subtypes.is_green) k += 8;
            if (!card->subtypes.is_creature) k++;
        } else {
            k += 10;
            if (!card->subtypes.is_creature) k++;
        }
        l = categories->entries[k];
        AppendToList(l, card);
		cardArray++;
	}
    
    for (int i=0;i<14;i++) {
        List* l = categories->entries[i];
        qsort(l->entries,l->size,sizeof(MTGCard*),MTGCard_compare);
    }
}

void loadAbilities() {
    cd.ShadowcloakVampire->subtypes.has_instant = true;
}

void buildDeck(List* cards,int index) {
	if (index == 0) {
        for (int i=0;i<10;i++) AppendToList(cards,cd.SatyrWayfinder);
        for (int i=0;i<10;i++) AppendToList(cards,cd.InvasiveSpecies);
        for (int i=0;i<10;i++) AppendToList(cards,cd.ReclamationSage);
		for (int i=0;i<15;i++) AppendToList(cards,cd.Island);
        for (int i=0;i<15;i++) AppendToList(cards,cd.Forest);
	} else if (index == 1) {
		for (int i=0;i<2;i++) AppendToList(cards,cd.Ornithopter);
		for (int i=0;i<3;i++) AppendToList(cards,cd.BronzeSable);
		for (int i=0;i<2;i++) AppendToList(cards,cd.WelkinTern);
		for (int i=0;i<1;i++) AppendToList(cards,cd.RummagingGoblin);
		for (int i=0;i<3;i++) AppendToList(cards,cd.AeronautTinkerer);
		for (int i=0;i<2;i++) AppendToList(cards,cd.KrenkosEnforcer);
		for (int i=0;i<2;i++) AppendToList(cards,cd.WallofFrost);
		for (int i=0;i<2;i++) AppendToList(cards,cd.Juggernaut);
		for (int i=0;i<3;i++) AppendToList(cards,cd.ScrapyardMongrel);
		for (int i=0;i<1;i++) AppendToList(cards,cd.HoardingDragon);
		for (int i=0;i<2;i++) AppendToList(cards,cd.GlacialCrasher);
		for (int i=0;i<1;i++) AppendToList(cards,cd.SiegeDragon);
		for (int i=0;i<1;i++) AppendToList(cards,cd.RoguesGloves);
		for (int i=0;i<1;i++) AppendToList(cards,cd.SacredArmory);
		for (int i=0;i<1;i++) AppendToList(cards,cd.TyrantsMachine);
		for (int i=0;i<1;i++) AppendToList(cards,cd.ShrapnelBlast);
		for (int i=0;i<2;i++) AppendToList(cards,cd.EnsoulArtifact);
		for (int i=0;i<1;i++) AppendToList(cards,cd.BrawlersPlate);
		for (int i=0;i<1;i++) AppendToList(cards,cd.StaffoftheFlameMagus);
		for (int i=0;i<1;i++) AppendToList(cards,cd.StaffoftheMindMagus);
		for (int i=0;i<2;i++) AppendToList(cards,cd.LavaAxe);
		for (int i=0;i<1;i++) AppendToList(cards,cd.DarksteelCitadel);
		for (int i=0;i<12;i++) AppendToList(cards,cd.Mountain);
		for (int i=0;i<12;i++) AppendToList(cards,cd.Island);
	} else if (index == 2) {
		for (int i=0;i<1;i++) AppendToList(cards,cd.FugitiveWizard);
		for (int i=0;i<1;i++) AppendToList(cards,cd.ResearchAssistant);
		for (int i=0;i<2;i++) AppendToList(cards,cd.RuneclawBear);
		for (int i=0;i<2;i++) AppendToList(cards,cd.SatyrWayfinder);
		for (int i=0;i<2;i++) AppendToList(cards,cd.CoralBarrier);
		for (int i=0;i<2;i++) AppendToList(cards,cd.FrostLynx);
		for (int i=0;i<1;i++) AppendToList(cards,cd.IllusoryAngel);
		for (int i=0;i<2;i++) AppendToList(cards,cd.InvasiveSpecies);
		for (int i=0;i<2;i++) AppendToList(cards,cd.ReclamationSage);
		for (int i=0;i<2;i++) AppendToList(cards,cd.RoaringPrimadox);
		for (int i=0;i<3;i++) AppendToList(cards,cd.ShamanofSpring);
		for (int i=0;i<1;i++) AppendToList(cards,cd.MercurialPretender);
		for (int i=0;i<2;i++) AppendToList(cards,cd.KapshoKitefins);
		for (int i=0;i<1;i++) AppendToList(cards,cd.CarnivorousMossBeast);
		for (int i=0;i<1;i++) AppendToList(cards,cd.StormtideLeviathan);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Negate);
		for (int i=0;i<2;i++) AppendToList(cards,cd.PeelfromReality);
		for (int i=0;i<1;i++) AppendToList(cards,cd.TurntoFrog);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Plummet);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Encrust);
		for (int i=0;i<2;i++) AppendToList(cards,cd.IntotheVoid);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Meteorite);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Restock);
		for (int i=0;i<13;i++) AppendToList(cards,cd.Island);
		for (int i=0;i<12;i++) AppendToList(cards,cd.Forest);
	} else if (index == 3) {
		for (int i=0;i<3;i++) AppendToList(cards,cd.TyphoidRats);
		for (int i=0;i<2;i++) AppendToList(cards,cd.TorchFiend);
		for (int i=0;i<2;i++) AppendToList(cards,cd.GargoyleSentinel);
		for (int i=0;i<2;i++) AppendToList(cards,cd.CarrionCrow);
		for (int i=0;i<2;i++) AppendToList(cards,cd.GoblinRoughrider);
		for (int i=0;i<1;i++) AppendToList(cards,cd.WallofFire);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Gravedigger);
		for (int i=0;i<1;i++) AppendToList(cards,cd.ZofShade);
		for (int i=0;i<2;i++) AppendToList(cards,cd.NightfireGiant);
		for (int i=0;i<1;i++) AppendToList(cards,cd.IndulgentTormentor);
		for (int i=0;i<1;i++) AppendToList(cards,cd.ThunderingGiant);
		for (int i=0;i<2;i++) AppendToList(cards,cd.ProfaneMemento);
		for (int i=0;i<1;i++) AppendToList(cards,cd.ClearaPath);
		for (int i=0;i<2;i++) AppendToList(cards,cd.LightningStrike);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Festergloom);
		for (int i=0;i<1;i++) AppendToList(cards,cd.StabWound);
		for (int i=0;i<1;i++) AppendToList(cards,cd.BurningAnger);
		for (int i=0;i<2;i++) AppendToList(cards,cd.FleshtoDust);
		for (int i=0;i<1;i++) AppendToList(cards,cd.ConeofFlame);
		for (int i=0;i<2;i++) AppendToList(cards,cd.BlastfireBolt);
		for (int i=0;i<1;i++) AppendToList(cards,cd.CausticTar);
		for (int i=0;i<2;i++) AppendToList(cards,cd.HeatRay);
		for (int i=0;i<13;i++) AppendToList(cards,cd.Swamp);
		for (int i=0;i<13;i++) AppendToList(cards,cd.Mountain);
	} else if (index == 4) {
		for (int i=0;i<3;i++) AppendToList(cards,cd.Soulmender);
		for (int i=0;i<2;i++) AppendToList(cards,cd.AjanisPridemate);
		for (int i=0;i<1;i++) AppendToList(cards,cd.KinsbaileSkirmisher);
		for (int i=0;i<2;i++) AppendToList(cards,cd.SungracePegasus);
		for (int i=0;i<1;i++) AppendToList(cards,cd.WallofEssence);
		for (int i=0;i<2;i++) AppendToList(cards,cd.ChildofNight);
		for (int i=0;i<2;i++) AppendToList(cards,cd.WallofLimbs);
		for (int i=0;i<2;i++) AppendToList(cards,cd.WitchsFamiliar);
		for (int i=0;i<1;i++) AppendToList(cards,cd.GeistoftheMoors);
		for (int i=0;i<1;i++) AppendToList(cards,cd.AccursedSpirit);
		for (int i=0;i<1;i++) AppendToList(cards,cd.TirelessMissionaries);
		for (int i=0;i<2;i++) AppendToList(cards,cd.ShadowcloakVampire);
		for (int i=0;i<1;i++) AppendToList(cards,cd.BloodHost);
		for (int i=0;i<1;i++) AppendToList(cards,cd.ResoluteArchangel);
		for (int i=0;i<1;i++) AppendToList(cards,cd.CripplingBlight);
		for (int i=0;i<2;i++) AppendToList(cards,cd.Ulcerate);
		for (int i=0;i<1;i++) AppendToList(cards,cd.DivineFavor);
		for (int i=0;i<3;i++) AppendToList(cards,cd.SigninBlood);
		for (int i=0;i<1;i++) AppendToList(cards,cd.StaffoftheDeathMagus);
		for (int i=0;i<1;i++) AppendToList(cards,cd.StaffoftheSunMagus);
		for (int i=0;i<1;i++) AppendToList(cards,cd.PillarofLight);
		for (int i=0;i<1;i++) AppendToList(cards,cd.SolemnOffering);
		for (int i=0;i<1;i++) AppendToList(cards,cd.FirstResponse);
		for (int i=0;i<1;i++) AppendToList(cards,cd.MassCalcify);
		for (int i=0;i<12;i++) AppendToList(cards,cd.Plains);
		for (int i=0;i<13;i++) AppendToList(cards,cd.Swamp);
	} else if (index == 5) {
		for (int i=0;i<2;i++) AppendToList(cards,cd.SunbladeElf);
		for (int i=0;i<2;i++) AppendToList(cards,cd.SelflessCathar);
		for (int i=0;i<1;i++) AppendToList(cards,cd.WallofMulch);
		for (int i=0;i<2;i++) AppendToList(cards,cd.OreskosSwiftclaw);
		for (int i=0;i<2;i++) AppendToList(cards,cd.MidnightGuard);
		for (int i=0;i<2;i++) AppendToList(cards,cd.LivingTotem);
		for (int i=0;i<2;i++) AppendToList(cards,cd.WillForgedGolem);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Phytotitan);
		for (int i=0;i<2;i++) AppendToList(cards,cd.SiegeWurm);
		for (int i=0;i<2;i++) AppendToList(cards,cd.SeraphoftheMasses);
		for (int i=0;i<1;i++) AppendToList(cards,cd.HornetQueen);
		for (int i=0;i<1;i++) AppendToList(cards,cd.GatherCourage);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Naturalize);
		for (int i=0;i<1;i++) AppendToList(cards,cd.TitanicGrowth);
		for (int i=0;i<3;i++) AppendToList(cards,cd.RaisetheAlarm);
		for (int i=0;i<2;i++) AppendToList(cards,cd.DevouringLight);
		for (int i=0;i<1;i++) AppendToList(cards,cd.NissasExpedition);
		for (int i=0;i<1;i++) AppendToList(cards,cd.SanctifiedCharge);
		for (int i=0;i<1;i++) AppendToList(cards,cd.MeditationPuzzle);
		for (int i=0;i<2;i++) AppendToList(cards,cd.TriplicateSpirits);
		for (int i=0;i<1;i++) AppendToList(cards,cd.Overwhelm);
		for (int i=0;i<2;i++) AppendToList(cards,cd.FeralIncarnation);
		for (int i=0;i<13;i++) AppendToList(cards,cd.Forest);
		for (int i=0;i<12;i++) AppendToList(cards,cd.Plains);
	}
}

void shuffleDeck(List* cards) {
    unsigned int n = cards->size;
    List* tmp;
    while (n > 1) {
        unsigned int i = rand() % n;
        tmp = cards->entries[i];
        cards->entries[i] = cards->entries[n-1];
        cards->entries[n-1] = tmp;
        n--;
    }
}

void apply_mulligan(MTGPlayer* p) {
    unsigned int sz = p->hand->size;
    if (sz == 0)
        return;
    for (unsigned int i=0;i<sz;i++) {
        AppendToList(p->library, p->hand->entries[i]);
    }
    p->hand->size = 0;
    shuffleDeck(p->library);
    MTGPlayer_drawCards(p, sz - 1);
    displayHand(p->hand);
    displayStats(p->hp,p->library->size,p->hand->size,p->graveyard->size,p->exile->size,p->mana,true);
}

void resolveAttack(MTGPlayer* attacker,List* permanentList) {
	blockersList = InitList();
    block_index = 0;
    attackerList = InitList();
	for (unsigned int i=0;i<permanentList->size;i++) {
        Permanent* p = permanentList->entries[i];
        AppendToList(attackerList, p);
		List* blockers = InitList();
		AppendToList(blockersList,blockers);
	}
	if (attacker==player1) {
		AI_getBlockers(attackerList,blockersList);
        message("You launched an attack");
	} else {
		selectBlockers(attackerList,blockersList);
        message("Opponent launched an attack");
	}
}

bool resolveBlock() {
    if (block_index >= attackerList->size) {
        displayStats(player1->hp,player1->library->size,player1->hand->size,player1->graveyard->size,player1->exile->size,player1->mana,true);
        displayBattlefield(player1->battlefield, true);
        displayStats(player2->hp,player2->library->size,player2->hand->size,player2->graveyard->size,player2->exile->size, player2->mana,false);
        displayBattlefield(player2->battlefield, false);
        endAttack();
        return true;
    }
    MTGPlayer* defender;
    if (currentPlayer==player1) {
        defender = player2;
    } else {
        defender = player1;
    }
    Permanent* p = attackerList->entries[block_index];
    List* blockers = blockersList->entries[block_index];
    char buffer[128];
    int k;
    if (blockers->size == 0) {
        Event_loseLife(p, defender, p->power);
        if (p->subtypes.is_lifelink) {
            Event_gainLife(p, currentPlayer, p->power);
        }
    } else {
        k = sprintf(buffer,"%s is blocked by ",p->name);
        for (unsigned int j=0;j<blockers->size;j++) {
            Permanent* q = blockers->entries[j];
            k += sprintf(buffer+k,"%s,",q->name);
        }
        message(buffer);
        int p_lifelink = 0;
        int q_lifelink = 0;
        for (unsigned int j=0;j<blockers->size;j++) {
            Permanent* q = blockers->entries[j];
            int initialToughness = q->toughness;
            if ((p->subtypes.is_first_strike||p->subtypes.is_double_strike) && p->power > 0 && p->toughness > 0) {
                Event_damage(p, q, p->power);
                if (p->subtypes.is_lifelink) p_lifelink += p->power;
            }
            if ((q->subtypes.is_first_strike||q->subtypes.is_double_strike) && q->power > 0) {
                Event_damage(q, p, q->power);
                if (q->subtypes.is_lifelink) q_lifelink += q->power;
            }
            int currentToughness = q->toughness;
            if (!p->subtypes.is_first_strike && p->power > 0 && p->toughness > 0) {
                Event_damage(p, q, p->power);
                if (p->subtypes.is_lifelink) p_lifelink += p->power;
            }
            if (!q->subtypes.is_first_strike && q->power > 0 && currentToughness > 0) {
                Event_damage(q, p, q->power);
                if (q->subtypes.is_lifelink) q_lifelink += q->power;
            }
            p->power -= initialToughness;
            
        }
        if (p->subtypes.is_trample && p->power > 0) {
            Event_loseLife(p,defender, p->power);
        }
        if (p_lifelink > 0) {
            Event_gainLife(p,currentPlayer,p_lifelink);
        }
        if (q_lifelink) {
            Event_gainLife(NULL,defender,q_lifelink);
        }
    }
    displayStats(player1->hp,player1->library->size,player1->hand->size,player1->graveyard->size,player1->exile->size,player1->mana,true);
    displayBattlefield(player1->battlefield, true);
    displayStats(player2->hp,player2->library->size,player2->hand->size,player2->graveyard->size, player2->exile->size,player2->mana,false);
    displayBattlefield(player2->battlefield, false);
    block_index++;
    return false;
}

void resolveAI() {
    if (stack->size > 0) {
        if (resolveStack())
            AI_getAction();
    } else if (attackerList) {
        if (resolveBlock())
            AI_getAction();
    } else
        AI_getAction();
}

bool resolveStack() {
    Permanent* permanent = stack->entries[stack->size-1];
    stack->size--;
    bool destroyed = false;
    
    if ((permanent->subtypes.is_aura || permanent->subtypes.is_equipment)) {
        if (permanent->target)
            AppendToList(permanent->target->equipment,permanent);
        else {
            AppendToList(permanent->owner->graveyard,permanent->source);
            destroyed = true;
        }
    } else if (permanent->subtypes.is_enchantment || permanent->subtypes.is_creature || permanent->subtypes.is_planeswalker || permanent->subtypes.is_artifact) {
        AppendToList(permanent->owner->battlefield,permanent);
    } else {
        AppendToList(permanent->owner->graveyard,permanent->source);
        destroyed = true;
    }
    bool isValid = Event_onResolve(permanent);
    
    displayStack(stack);
    if (DEBUG_AI)
        displayHand(currentPlayer->hand);
    else
        displayHand(player1->hand);
    displayStats(player1->hp,player1->library->size,player1->hand->size,player1->graveyard->size,player1->exile->size,  player1->mana,true);
    displayLands(player1->lands, true);
    displayBattlefield(player1->battlefield, true);
    displayStats(player2->hp,player2->library->size,player2->hand->size,player2->graveyard->size,player2->exile->size, player2->mana ,false);
    displayLands(player2->lands, false);
    displayBattlefield(player2->battlefield, false);
    if (isValid && destroyed)
        DeletePermanent(permanent);
    if (stack->size == 0)
        return true;
    else
        return false;
}

void endAttack() {
    if (attackerList)
        DeleteList(attackerList);
    if (blockersList) {
        for (unsigned int i=0;i<blockersList->size;i++) {
            List* l = blockersList->entries[i];
            DeleteList(l);
        }
        DeleteList(blockersList);
    }
    attackerList = NULL;
    blockersList = NULL;

}

void newTurn() {

	if (currentPlayer == player1) {
        MTGPlayer_restore(player1);
        MTGPlayer_restore(player2);
        displayStats(player1->hp,player1->library->size,player1->hand->size,player1->graveyard->size,player1->exile->size, player1->mana,true);
        displayBattlefield(player1->battlefield, true);
        
		currentPlayer = player2;
        MTGPlayer_refresh(player2);
        Event_onUpkeep(player2);
        MTGPlayer_drawCards(player2, 1);
        startTurn(player2);
        displayStats(player2->hp,player2->library->size,player2->hand->size,player2->graveyard->size, player2->exile->size, player2->mana,false);
        displayLands(player2->lands, false);
        displayBattlefield(player2->battlefield, false);
        AI_getAction(player2);
	} else {
        MTGPlayer_restore(player1);
        MTGPlayer_restore(player2);
        displayStats(player2->hp,player2->library->size,player2->hand->size,player2->graveyard->size,player2->exile->size,  player2->mana,false);
        displayBattlefield(player2->battlefield, false);
        
		currentPlayer = player1;
		MTGPlayer_refresh(player1);
        Event_onUpkeep(player1);
		MTGPlayer_drawCards(player1, 1);
        startTurn(player1);
		displayHand(player1->hand);
		displayStats(player1->hp,player1->library->size,player1->hand->size,player1->graveyard->size,player1->exile->size,  player1->mana,true);
        displayLands(player1->lands, true);
        displayBattlefield(player1->battlefield, true);
	}

}

MTGPlayer* newGame(int deck_index) {
    initEvents();
    stack = InitList();
    
    player1 = InitMTGPlayer();
	//loadDeck("deck.txt",player1->library);
    //if (player1->library->size <= 0)
    //    buildDeck(player1->library,rand() % 6);
    if (deck_index < 0)
        buildDeck(player1->library, rand() % 6);
    else
        buildDeck(player1->library, deck_index);
	shuffleDeck(player1->library);
	//saveDeck("deck.txt",player1->library);

	player2 = InitMTGPlayer();
    AI_init(player2);
	buildDeck(player2->library,4);
	shuffleDeck(player2->library);

	MTGPlayer_drawCards(player1,7);
	MTGPlayer_drawCards(player2,7);

	displayHand(player1->hand);
    displayStats(player1->hp,player1->library->size,player1->hand->size,player1->graveyard->size, player1->exile->size, player1->mana,true);
    displayLands(player1->lands, true);
    displayBattlefield(player1->battlefield, true);
    displayStats(player2->hp,player2->library->size,player2->hand->size,player2->graveyard->size,player2->exile->size, player2->mana ,false);
    displayLands(player2->lands, false);
    displayBattlefield(player2->battlefield, false);
    displayStack(stack);
  
 	if (rand() % 2) {
		currentPlayer = player1;
		message("You start first");
	} else {
		currentPlayer = player2;
		message("Opponent starts first");
	}
    
    mulligan();
    return player1;
}

void startGame() {
    startTurn(currentPlayer); //starting player does not draw 1 card
}

void endGame() {
    endAttack();
    DeleteEvents();
    DeleteList(stack);
    DeleteMTGPlayer(player1);
    DeleteMTGPlayer(player2);
}
