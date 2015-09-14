#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
		int* dummy;
		int* keys;
		void** entries;
		int size,load;
} HashTable;

HashTable* InitHashTable();
void DeleteHashTable(HashTable* ht);
bool HashTable_insert(HashTable* ht,int intKey,void* entry);
void* HashTable_find(HashTable* ht,int intKey);
void* HashTable_remove(HashTable* ht,int intKey);
bool HashTable_insertVar(HashTable* ht,void* key, int keySize, void* entry);
void* HashTable_findVar(HashTable* ht,void* key, int keySize);
void* HashTable_removeVar(HashTable* ht,void* key, int keySize);
