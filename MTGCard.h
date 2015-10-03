#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "List.h"
#include "Subtypes.h"

typedef enum {
	COLORLESS=0, WHITE=1, BLUE=2, BLACK=3, RED=4, GREEN=5
} Color;

typedef struct {
	Color color1;
	Color color2;
	int num;
	bool isVariable;
	bool hasOption;
} Manacost;

typedef struct {
    List* manaCost;
    bool needs_tap;
    int lifeCost;
} Ability;

typedef struct {
    bool is_white;
    bool is_blue;
    bool is_black;
    bool is_red;
    bool is_green;
    bool is_land;
    bool is_sorcery;
    bool is_instant;
    bool is_enchantment;
    bool is_artifact;
    bool is_creature;
    bool is_planeswalker;
    bool is_conspiracy;
    bool is_phenomenon;
    bool is_scheme;
    bool is_tribal;
    bool is_vanguard;
    SUBTYPE_PLACEHOLDER
} Subtypes;

typedef struct  {
	const char* name;
	List* manaCost;
    List* abilities;
	int cmc;
    Subtypes subtypes;
	const char* text;
	int power;
	int toughness;
	int loyalty;
} MTGCard;


Ability* NewAbility();
MTGCard* NewMTGCard(const char* s,int cost);

int MTGCard_compare(const void* c1,const void* c2);
Manacost* colorlessMana(int n);
Manacost* W_Mana(int n);
Manacost* U_Mana(int n);
Manacost* B_Mana(int n);
Manacost* R_Mana(int n);
Manacost* G_Mana(int n);
Manacost* X_Mana(int n);
void DeleteMTGCard(MTGCard* card);
