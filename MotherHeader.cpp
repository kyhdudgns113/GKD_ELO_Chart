#include "MotherHeader.h"
#include <math.h>
#include <stdio.h>

bool cmp_row_elo(NODE_PRINTED_ROW a, NODE_PRINTED_ROW b) {
	return a.elo > b.elo;
}

void flush_input_buffer() {
	char cbuf = 'a';

	while (cbuf != -1 && cbuf != '\n') {
		cbuf = getc(stdin);
	}
}