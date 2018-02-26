#ifndef _CNS_CELL_H_
#define _CNS_CELL_H_

#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#include "cns_util.h"
#include "cns_list.h"
#include "cns_graph.h"

#define CNS_MAX_CELLS INT_MAX

typedef struct cns_cell_data cns_cell_data;
struct cns_cell_data {
	uint8_t         width;
	cns_dtype       dtype;
	void           *input;
	void           *weight;
	void           *output;
};

typedef void (* cns_cell_op) (cns_cell_data *data);

typedef struct cns_cell cns_cell;
struct cns_cell {
	/* The data of a cell. For the convenience of implementation,
	   this is not implemented as a pointer. */
	cns_cell_data   data;
	cns_cell_op     op;	/* operation on the data, function pointer */
	size_t          index;	/* index in a cell array */
	cns_bool_t      en;	/* whether this cell is enabled */

	/* They are the indexes of cells in the same cell array
	   on which this cell depends. It starts running after
	   those cells all stop running. -1 is a special index.
	   Every cell running in the first place should depend on
	   and only on -1.
	   NOTICE: Avoid dead locks */
	cns_list       *deps;	/* data type ssize_t */
};

#ifdef __cplusplus
extern "C" {
#endif

	void cns_cell_run(cns_cell *cell);
	void cns_cell_set_cell_data(cns_cell *cell, cns_cell_data *cell_data);
	void cns_cell_set_data(cns_cell *cell,
			void *input, void *weight, void *output);
	void cns_cell_set_width(cns_cell *cell, uint8_t width);
	void cns_cell_set_dtype(cns_cell *cell, cns_dtype dtype);
	void cns_cell_set_op(cns_cell *cell, cns_cell_op op);
	void cns_cell_set_en(cns_cell *cell, cns_bool_t en);
	void cns_cell_add_dep(cns_cell *cell, ssize_t dep);
	void cns_cell_fprint_data(FILE *fp, cns_cell *cell);
	void cns_cell_print_data(cns_cell *cell);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_CELL_H_ */
