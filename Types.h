typedef enum {
	COLORLESS, WHITE, BLUE, BLACK, RED, GREEN
} Color;

typedef struct {
	Color color1;
	Color color2;
	int num;
	bool isVariable;
	bool hasOption;
} Manacost;
