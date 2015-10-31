#include "MTGCard.h"

MTGCard* NewMTGCard(const char* s,int cost) {
	MTGCard* m = (MTGCard*) calloc(1,sizeof(MTGCard));
	m->name = s;
	m->cmc = cost;
	m->manaCost = InitList();
    m->abilities = InitList();
	return m;
}

Ability* NewAbility() {
    Ability* ab = (Ability*) calloc(1,sizeof(Ability));
    ab->manaCost = InitList();
    return ab;
}

int MTGCard_compare(const void* c1,const void* c2) {
    MTGCard* m1 = *(MTGCard**)c1;
    MTGCard* m2 = *(MTGCard**)c2;
    if (m1 == m2) return 0;
    if (m1->subtypes.is_land && !m2->subtypes.is_land) return 1;
    if (m2->subtypes.is_land && !m1->subtypes.is_land) return -1;
    
    int m1_color = m1->subtypes.is_white<<4 | m1->subtypes.is_blue<<3 | m1->subtypes.is_black<<2 | m1->subtypes.is_red<<1 | m1->subtypes.is_green;
    int m2_color = m2->subtypes.is_white<<4 | m2->subtypes.is_blue<<3 | m2->subtypes.is_black<<2 | m2->subtypes.is_red<<1 | m2->subtypes.is_green;
    if (m1_color != m2_color) return m2_color - m1_color;
    
    int m1_type = m1->subtypes.is_planeswalker<<5 | m1->subtypes.is_creature<<4| m1->subtypes.is_enchantment<<3 | m1->subtypes.is_artifact << 2 | m1->subtypes.is_sorcery << 1 | m1->subtypes.is_instant;
    int m2_type = m2->subtypes.is_planeswalker<<5 | m2->subtypes.is_creature<<4| m2->subtypes.is_enchantment<<3 | m2->subtypes.is_artifact << 2 | m2->subtypes.is_sorcery << 1 | m2->subtypes.is_instant;
    if (m1_type != m2_type) return m2_type - m1_type;
    
    if (m1->cmc != m2->cmc) return m1->cmc - m2->cmc;
    
    const char* r1 = m1->name, *r2 = m2->name;
    if (*r1 != *r2) return *r1 - *r2;
    while (1) {
        r1++;
        r2++;
        if (*r1 != *r2) return *r1 - *r2;
        if (! *r1 || ! *r2) break;
    }
    return 0;
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

void DeleteAbility(Ability* a) {
    for (unsigned int j=0;j<a->manaCost->size;j++) {
        free(a->manaCost->entries[j]);
    }
    DeleteList(a->manaCost);
    free(a);
}

void DeleteMTGCard(MTGCard* card) {
    for (unsigned int i=0;i<card->abilities->size;i++) {
        Ability* a = card->abilities->entries[i];
        DeleteAbility(a);
    }
    DeleteList(card->abilities);
	for (unsigned int i=0;i<card->manaCost->size;i++)
		free(card->manaCost->entries[i]);
	DeleteList(card->manaCost);
	free(card);
}
