#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "API.h"

int main() {
srand(time(0));

int **map;
generare_harta(&map);
afisare_harta(map);
return 0;
}