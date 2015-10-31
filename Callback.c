#include "Callback.h"

extern CardData cd;
extern MTGPlayer* player1;
extern MTGPlayer* player2;
MTGPlayer* findTarget(Permanent* p,int* index);
bool Event_damage(Permanent* defender,int num);

bool MercurialPretender_callback(Permanent* permanent) {
    char buf[128];
    sprintf(buf,"%s is returned to hand",permanent->name);
    message(buf);
    AppendToList(permanent->owner->hand,cd.MercurialPretender);
    for (unsigned int i=0;i<permanent->abilities->size;i++) {
        Ability* a = permanent->abilities->entries[i];
        if (a->callback == MercurialPretender_callback) {
            DeleteAbility(a);
            break;
        }
    }
    Event_onDestroy(permanent,HAND);
    if (permanent->equipment) {
        for (unsigned int j=0;j<permanent->equipment->size;j++) {
            Permanent* q = permanent->equipment->entries[j];
            if (q->subtypes.is_equipment) {
                q->target = NULL;
                AppendToList(q->owner->battlefield, q);
            } else {
                Event_onDestroy(q,GRAVEYARD);
                AppendToList(q->owner->graveyard, q->source);
                DeletePermanent(q);
            }
        }
        DeleteList(permanent->equipment);
    }
    if (permanent->abilities)
        DeleteList(permanent->abilities);
    RemoveListObject(permanent->controller->battlefield, permanent);
    free(permanent);
    return false;
}

bool BurningAnger_callback(Permanent* permanent) {
    int index;
    char buffer[128];
    if (permanent->target == player1->marker) {
        Event_loseLife(permanent, player1, permanent->power);
    } else if (permanent->target == player2->marker) {
        Event_loseLife(permanent, player2, permanent->power);
    } else if (permanent->target) {
        if (findTarget(permanent->target,&index) && permanent->target->subtypes.is_creature) {
            sprintf(buffer,"%s is dealt %d damage (%s)",permanent->target->name,permanent->power,permanent->name);
            message(buffer);
            Event_damage(permanent->target, permanent->power);
        } else {
            sprintf(buffer,"Invalid target for %s",permanent->name);
            message(buffer);
        }
    } else {
        Event_loseLife(permanent, player2, permanent->power);
    }
    return true;
}

bool CausticTar_callback(Permanent* permanent) {
    MTGPlayer* p = permanent->target == player1->marker ? player1 : player2;
    Event_loseLife(permanent, p, 3);
    return true;
}