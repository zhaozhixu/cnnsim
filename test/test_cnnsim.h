#ifndef _TEST_CNNSIM_H_
#define _TEST_CNNSIM_H_

#include <stdio.h>
#include <check.h>

#ifdef _cplusplus
extern "C" {
#endif

	Suite *make_block_suite(void);
	Suite *make_tensor_suite(void);
	Suite *make_list_suite(void);
	Suite *make_queue_suite(void);
	Suite *make_graph_suite(void);
	Suite *make_buf_suite(void);
	Suite *make_step_suite(void);
	Suite *make_sch_suite(void);

#ifdef _cplusplus
}
#endif

#endif	/* _TEST_CNNSIM_H_ */
