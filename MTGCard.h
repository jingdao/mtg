#include <stdbool.h>
#include <stdlib.h>
#include "List.h"
#include "Subtypes.h"

typedef struct  {
	const char* name;
	List* manaCost;
	int cmc;
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

MTGCard* NewMTGCard(const char* s);
