//
//  main.c
//  mtg
//
//  Created by Chen, Jing Dao on 9/6/15.
//  Copyright (c) 2015 Chen, Jing Dao. All rights reserved.
//

#include <stdio.h>
#include "MTGController.h"

CardData cd;
List* manaList;

int main(int argc, const char * argv[]) {
	cd = loadCardData();
	manaList = InitList();
	newGame();
	DeleteList(manaList);
    return 0;
}
