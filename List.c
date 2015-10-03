#include "List.h"

List* InitList() {
	List* ls = (List*)malloc(sizeof(List));
	//printf("malloc List: %d\n",sizeof(List)+INITIAL_LIST_SIZE*sizeof(void*));
	if (!ls) return NULL;
	ls->entries = (void**)malloc(INITIAL_LIST_SIZE*sizeof(void*));
	if (!ls->entries) {
		free(ls);
		return NULL;
	}
	ls->maxSize = INITIAL_LIST_SIZE;
	ls->size = 0;
	// unsigned int i;
	// for (i = 0; i < ls->maxSize; i++) {
	// 	ls->entries[i] = NULL;
	// }
	memset(ls->entries,'\0',INITIAL_LIST_SIZE*sizeof(void*));
	return ls;
}

void DeleteList(List* ls) {
	if (ls) {
		if (ls->entries) free(ls->entries);
		free(ls);
	}
}

List* ListCopy(List* ls) {
    if (ls) {
        List* res = (List*)malloc(sizeof(List));
        if (!res) return NULL;
        res->maxSize = ls->maxSize;
        res->size = ls->size;
        res->entries = (void**)malloc(res->maxSize*sizeof(void*));
        memcpy(res->entries,ls->entries,ls->size*sizeof(void*));
        return res;
    } else return NULL;
}

bool AppendToList(List* ls, void* entry) {
	if (!ls) return false;
	if (ls->maxSize==ls->size) {
		//printf("Now doubling size of list\n");
		void** newEntries = (void**)realloc(ls->entries,ls->maxSize*2*sizeof(void*));
		//printf("malloc double List: %d\n",ls->maxSize*2*sizeof(void*));
		if (!newEntries) {
			printf("Warning(List): Unable to resize table!\n");
			return 0;
		}
		ls->entries = newEntries;
		ls->maxSize *= 2;
	}
	ls->entries[ls->size]=entry;
	ls->size++;
	return true;
}

void* GetListItem(List* ls, unsigned int index) {
	if (!ls||index>=ls->size) return NULL;
	return ls->entries[index];
}

void* RemoveListIndex(List* ls,unsigned int index) {
    void* obj = ls->entries[index];
    memmove(ls->entries+index,ls->entries+index+1,(ls->size-1-index)*sizeof(void*));
    ls->size--;
    return obj;
}

bool RemoveListObject(List* ls,void* obj) {
    for (unsigned int i=0;i<ls->size;i++) {
        if (ls->entries[i] == obj) {
            RemoveListIndex(ls, i);
            return true;
        }
    }
    return false;
}

bool ListContains(List* ls,void* obj) {
    for (unsigned int i=0;i<ls->size;i++) {
        if (ls->entries[i] == obj) {
            return true;
        }
    }
    return false;
}


List* IntersectList(List* lsa, List* lsb) {
	if (!lsa||!lsb) {
		return NULL;
	}
	List* combined = InitList();
	//IMPROVE algorithm later
	//TODO intersect lists with repeats, color highlight correct word
	unsigned int i,j;
	for (i = 0; i < lsa->size; i++) {
		printf("lsa: %p\n",GetListItem(lsa,i));
	}
	for (i = 0; i < lsb->size; i++) {
		printf("lsb: %p\n",GetListItem(lsb,i));
	}
	for (i = 0; i < lsa->size; i++) {
		for (j = 0; j < lsb->size; j++) {
			void* p = GetListItem(lsa, i);
			if (GetListItem(lsb, j) == p) {
				AppendToList(combined, p);
			}
		}
	}
	for (i = 0; i < combined->size; i++) {
		printf("combined: %p\n",GetListItem(combined,i));
	}
	return combined;
}
