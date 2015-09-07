#include <stdbool.h>
#include <stdlib.h>
#include "List.h"
#include "Subtypes.h"
#pragma once

typedef enum {
	COLORLESS, WHITE, BLUE, BLACK, RED, GREEN
} Color;

typedef struct {
	Color color1;
	Color color2;
	int num;
	bool isVariable;
	bool hasOption;
} Manacost;

typedef struct  {
	const char* name;
	List* manaCost;
	int cmc;
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
	const char* text;
	int power;
	int toughness;
	int loyalty;
} MTGCard;

MTGCard* NewMTGCard(const char* s,int cost);
Manacost* colorlessMana(int n);
Manacost* W_Mana(int n);
Manacost* U_Mana(int n);
Manacost* B_Mana(int n);
Manacost* R_Mana(int n);
Manacost* G_Mana(int n);
Manacost* X_Mana(int n);
