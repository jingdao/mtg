#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_LIST_SIZE 8

typedef struct {
	unsigned int maxSize;
	unsigned int size;
	void** entries;
} List;

List* InitList();
void DeleteList(List* ls);
bool AppendToList(List* ls, void* entry);
void* GetListItem(List* ls, unsigned int index);
void* RemoveListIndex(List* ls,unsigned int index);
bool RemoveListObject(List* ls,void* obj);
bool ListContains(List* ls,void* obj);

List* IntersectList(List* lsa, List* lsb);
