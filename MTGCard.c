#include "MTGCard.h"

MTGCard* NewMTGCard(const char* s,int cost) {
	MTGCard* m = (MTGCard*) calloc(1,sizeof(MTGCard));
	m->name = s;
	m->cmc = cost;
	m->manaCost = InitList();
	return m;
}

Permanent* NewPermanent(MTGCard* source) {
    Permanent* p = (Permanent*) malloc(sizeof(Permanent));
    p->is_tapped = false;
    p->power = source->power;
    p->toughness = source->toughness;
    p->source = source;
    return p;
}

Manacost* colorlessMana(int n) {
	Manacost* m = (Manacost*) calloc(1,sizeof(Manacost));
	m->color1 = COLORLESS;
	m->num = n;
	return m;
}

Manacost* W_Mana(int n) {
	Manacost* m = (Manacost*) calloc(1,sizeof(Manacost));
	m->color1 = WHITE;
	m->num = n;
	return m;
}

Manacost* U_Mana(int n) {
	Manacost* m = (Manacost*) calloc(1,sizeof(Manacost));
	m->color1 = BLUE;
	m->num = n;
	return m;
}

Manacost* B_Mana(int n) {
	Manacost* m = (Manacost*) calloc(1,sizeof(Manacost));
	m->color1 = BLACK;
	m->num = n;
	return m;
}

Manacost* R_Mana(int n) {
	Manacost* m = (Manacost*) calloc(1,sizeof(Manacost));
	m->color1 = RED;
	m->num = n;
	return m;
}

Manacost* G_Mana(int n) {
	Manacost* m = (Manacost*) calloc(1,sizeof(Manacost));
	m->color1 = GREEN;
	m->num = n;
	return m;
}

Manacost* X_Mana(int n) {
	Manacost* m = (Manacost*) calloc(1,sizeof(Manacost));
	m->isVariable=true;
	m->num = n;
	return m;
}

void DeleteMTGCard(MTGCard* card) {
	for (unsigned int i=0;i<card->manaCost->size;i++)
		free(card->manaCost->entries[i]);
	DeleteList(card->manaCost);
	free(card);
}
