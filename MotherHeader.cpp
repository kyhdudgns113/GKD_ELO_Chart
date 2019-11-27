#include "MotherHeader.h"

bool cmp_row_elo(NODE_PRINTED_ROW a, NODE_PRINTED_ROW b) {
	return a.elo > b.elo;
}