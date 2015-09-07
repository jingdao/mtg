#include "MTGCard.h"

MTGCard* NewMTGCard(const char* s) {
	MTGCard* m = (MTGCard*) malloc(sizeof(MTGCard));
	m->name = s;
	return m;
}
