#ifndef _CNS_CELL_OP_H_
#define _CNS_CELL_OP_H_

#include "cns_cell.h"

#ifdef __cplusplus
extern "C" {
#endif

	void cns_cell_op_mul_int8(cns_cell_data *data);
	void cns_cell_op_add_int8(cns_cell_data *data);
	void cns_cell_op_relu_int8(cns_cell_data *data);
	void cns_cell_op_add_many_int8(cns_cell_data *data);
	void cns_cell_op_assign_int8(cns_cell_data *data);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_CELL_OP_H_ */
