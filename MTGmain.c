//
//  main.c
//  mtg
//
//  Created by Chen, Jing Dao on 9/6/15.
//  Copyright (c) 2015 Chen, Jing Dao. All rights reserved.
//

#include <stdio.h>
#include "CardData.h"

int main(int argc, const char * argv[]) {
	CardData cd = loadCardData();
    printf("%s: %d %d\n",cd.BoonweaverGiant->name,cd.BoonweaverGiant->power,cd.BoonweaverGiant->toughness);
    return 0;
}
