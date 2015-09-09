all: MTGmain.c MTGController.c MTGController.h CommandLine.c ViewInterface.h MTGPlayer.c MTGPlayer.h CardData.c CardData.h MTGCard.c MTGCard.h List.c List.h Subtypes.h
	gcc -std=c11 -ggdb3 -o MTGmain MTGmain.c MTGController.c CommandLine.c MTGPlayer.c MTGCard.c CardData.c List.c
