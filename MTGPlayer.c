#include "MTGPlayer.h"

MTGPlayer* InitMTGPlayer() {
	MTGPlayer* p = (MTGPlayer*)malloc(sizeof(MTGPlayer));
	p->deck=InitList();
	p->hand=InitList();
	return p;
}

void MTGPlayer_drawCards(MTGPlayer* p,int num) {
	for (int i=0;i<num;i++) {
		AppendToList(p->hand,p->deck->entries[p->deck->size-1-i]);
	}
	p->deck->size -=num;
}

void DeleteMTGPlayer(MTGPlayer* p) {
	DeleteList(p->deck);
	DeleteList(p->hand);
	free(p);
}
