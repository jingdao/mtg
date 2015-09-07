all: MTGmain.c CardData.c CardData.h MTGCard.c MTGCard.h List.c List.h Types.h Subtypes.h
	gcc -ggdb3 -o MTGmain MTGmain.c MTGCard.c CardData.c List.c
