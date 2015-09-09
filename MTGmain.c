//
//  main.c
//  mtg
//
//  Created by Chen, Jing Dao on 9/6/15.
//  Copyright (c) 2015 Chen, Jing Dao. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include "MTGController.h"

CardData cd;
int main(int argc, const char * argv[]) {
	srand(time(NULL));
	cd = loadCardData();
	newGame();
	endGame();
	freeCardData(&cd);
    return 0;
}
