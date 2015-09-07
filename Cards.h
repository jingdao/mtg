#include <stdbool.h>

typedef struct  {
	char* name;
	List* manaCost;
	int cmc;
	bool isLand;
	bool isSorcery;
	bool isInstant;
	bool isEnchantment;
	bool isArtifact;
	bool isCreature;
	bool isPlaneswalker;
	char* text;
	int power;
	int toughness;
	int loyalty;
} Cards;
