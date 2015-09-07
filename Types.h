typedef enum {
	COLORLESS, WHITE, BLUE, BLACK, RED, GREEN
} Color;

typedef struct {
	Color color;
	int num;
	bool isVariable;
} Manacost;
