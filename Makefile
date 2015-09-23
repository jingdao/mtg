all: MTGmain.c MTGController.c MTGController.h CommandLine.c ViewInterface.h \
	EventHandler.h EventHandler.c Callback.h Callback.c \
	MTGPlayer.c MTGPlayer.h AIPlayer.c AIPlayer.h CardData.c CardData.h MTGCard.c MTGCard.h \
	HashTable.c HashTable.h List.c List.h Subtypes.h
	gcc -std=c11 -ggdb3 -o MTGmain MTGmain.c MTGController.c \
		Callback.c EventHandler.c CommandLine.c \
		MTGPlayer.c AIPlayer.c MTGCard.c CardData.c HashTable.c List.c
