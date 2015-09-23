#include "Callback.h"

void KinsbaileSkirmisher_onPlay(Permanent* p) {
	p->power++;
	p->toughness++;
	char buffer[64];
	sprintf(buffer,"%s gets +1/+1",p->source->name);
	message(buffer);
}

