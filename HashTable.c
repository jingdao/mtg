#include "HashTable.h"

int STRING_HASH_CONSTANT = 5381;
double BASE_HASH_CONSTANT = 0.618033988;
double STEP_HASH_CONSTANT = 0.707106781;
int INITIAL_TABLE_SIZE = 8;

int baseHash(int size, int hashKey) {
	return (int)(size*((BASE_HASH_CONSTANT*hashKey) - (int)(BASE_HASH_CONSTANT*hashKey)));
}

int stepHash(int size, int hashKey) {
	int res = (int)(size*((STEP_HASH_CONSTANT*hashKey) - (int)(STEP_HASH_CONSTANT*hashKey)));
	//make step size odd since table size is power of 2
	return res % 2 ? res : res + 1; 
}

int getIntKey(char* inputString, int len) {
	//djb2 algorithm http://www.cse.yorku.ca/~oz/hash.html
	int h = STRING_HASH_CONSTANT;
	for (int i=0;i<len;i++) {
		unsigned int c = *inputString++;
		h = (h << 5) + h + c;
	}
	if (h<0) h=-h;
	return h;
}

void doubleSize(HashTable* this) {
	//printf("Now doubling size of hash table\n");
	int newSize = this->size*2;
	int* newKeys = (int*)malloc(newSize * sizeof(int));
	void** newEntries = (void**)malloc(newSize*sizeof(void*));
	//printf("malloc double HashTable: %d\n",newSize * sizeof(int)+sizeof(void*));
	int* oldKeys = this->keys;
	void** oldEntries = this->entries;
	int oldSize = this->size;
	if (!newEntries||!newKeys) {
		printf("Warning(HashTable): Unable to resize table!\n");
		return;
	}
	this->keys = newKeys;
	this->entries = newEntries;
	this->size =newSize;
	this->load = 0;
	int i;
	for (i = 0; i < this->size; i++) {
		this->keys[i] = -1;
	}
	memset(this->entries,'\0',this->size*sizeof(void*));
	for (i = 0; i < oldSize; i++) {
		if (oldEntries[i] && oldEntries[i] != this->dummy)
			HashTable_insert(this,oldKeys[i], oldEntries[i]);
	}
	free(oldKeys);
	free(oldEntries);
}

HashTable* InitHashTable() {
	HashTable* this = (HashTable*) malloc(sizeof(HashTable));
	//claims a section of the heap to be used as dummy variable
	this->dummy = (int*)malloc(sizeof(int));
	this->keys = (int*)malloc(INITIAL_TABLE_SIZE*sizeof(int));
	this->entries = (void**)malloc(INITIAL_TABLE_SIZE*sizeof(void*));
	this->size = INITIAL_TABLE_SIZE;
	this->load = 0;
	int i;
	for (i = 0; i < this->size; i++) {
		this->keys[i] = -1;
	}
	memset(this->entries,'\0',this->size*sizeof(void*));
	return this;
}

void DeleteHashTable(HashTable* this) {
	free(this->dummy);
	free(this->keys);
	free(this->entries);
	free(this);
}

bool HashTable_insert(HashTable* this,int intKey, void* entry) {
	int h = baseHash(this->size, intKey);
	int step = stepHash(this->size, intKey);
	int i;
	for (i = 0; i < this->size; i++) {
		//printf("key: %d hash: %d\n", intKey, h);
		if (!this->entries[h] || this->entries[h] == this->dummy) {
			this->keys[h] = intKey;
			this->entries[h] = entry;
			this->load++;
			if (this->load>this->size / 4) {
				//printf("current load: %u/%u\n", this->load, this->size);
				doubleSize(this);
			}
			return true;
		}
		else if (this->keys[h] == intKey){
			return false;
		}
		else {
			h += step;
			h %= this->size;
		}
	}
	printf("Table overload (HashTable)!\n");
	return false;
}

void* HashTable_find(HashTable* this,int intKey) {
	int h = baseHash(this->size, intKey);
	int step = stepHash(this->size, intKey);
	int i;
	for (i = 0; i < this->size; i++) {
		//printf("key: %d hash: %d\n", intKey, h);
		if (!this->entries[h]) {
			return NULL;
		}
		else if (this->keys[h] == intKey){
			return this->entries[h];
		}
		else {
			h += step;
			h %= this->size;
		}
	}
	printf("Table overload (HashTable)!\n");
	return NULL;
}

void* HashTable_remove(HashTable* this,int intKey) {
	int h = baseHash(this->size, intKey);
	int step = stepHash(this->size, intKey);
	int i;
	void* removedItem;
	for (i = 0; i < this->size; i++) {
		//printf("key: %d hash: %d\n", intKey, h);
		if (!this->entries[h]) {
			return NULL;
		}
		else if (this->entries[h]!=this->dummy && this->keys[h] == intKey ){
			this->keys[h] = -1;
			removedItem = this->entries[h];
			this->entries[h] = this->dummy;
			this->load--;
			return removedItem;
		}
		else {
			h += step;
			h %= this->size;
		}
	}
	printf("Table overload (HashTable)!\n");
	return NULL;
}

bool HashTable_insertVar(HashTable* this,void* key, int keySize, void* entry) {
	int intKey = getIntKey((char*) key, keySize);
	return HashTable_insert(this,intKey,entry);
}

void* HashTable_findVar(HashTable* this,void* key, int keySize) {
	int intKey = getIntKey((char*) key, keySize);
	return HashTable_find(this,intKey);
}

void* HashTable_removeVar(HashTable* this,void* key, int keySize) {
	int intKey = getIntKey((char*) key, keySize);
	return HashTable_remove(this,intKey);
}

