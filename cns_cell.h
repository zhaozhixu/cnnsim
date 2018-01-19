#ifndef _CNS_CELL_H_
#define _CNS_CELL_H_

#include <stdint.h>
#include <stdio.h>

typedef enum {
	CNS_FLASE = 0,
	CNS_TRUE = 1
} cns_bool_t;

typedef enum {
	CNS_BOOL,
	CNS_FLOAT,
	CNS_INT32,
	CNS_INT16,
	CNS_INT8,
	CNS_UINT32,
	CNS_UINT16,
	CNS_UINT8
} cns_dtype;

typedef struct cns_cell_data {
	uint8_t        input_width;
	uint8_t        output_width;
	uint8_t        weight_width;

	int            input_dtype;
	int            output_dtype;
	int            weight_dtype;

	void           *input;
	void           *output;
	void           *weight;
} cns_cell_data;

typedef void (* cns_cell_op) (cns_cell_data *data);

typedef struct cns_cell {
	cns_cell_data  data;
	cns_cell_op    op;
} cns_cell;

void cns_cell_run(cns_cell *cell);
void cns_cell_set_cell_data(cns_cell *cell, cns_cell_data *cell_data);
void cns_cell_set_data(cns_cell *cell, void *input, void *output, void *weight);
void cns_cell_set_width(cns_cell *cell, uint8_t input_width,
			uint8_t output_width, uint8_t weight_width);
void cns_cell_set_dtype(cns_cell *cell, int input_dtype, int output_dtype, int weight_dtype);
void cns_cell_set_op(cns_cell *cell, cns_cell_op op);
cns_cell *cns_cells_create(uint32_t size);
void cns_cells_free(cns_cell *cells);
void cns_cells_run(cns_cell *cells, uint32_t size);
void fprint_cell_data(FILE *fp, cns_cell *cell);
void print_cell_data(cns_cell *cell);

#endif	/* _CNS_CELL_H_ */
