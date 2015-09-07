all: main.c List.c List.h Types.h
	gcc -ggdb3 -o main main.c List.c
