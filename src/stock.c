#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/stock.h"


Stock new_stock (int c, int q) {
    Stock s = malloc(sizeof(Stock));
    s.codigoArt = c;
    s.quantidade = q;
    return s;
}
