#include "./Drivers/parallelPipsNlp_C_Callback.h"

#include "mpi.h"
#include "global_var.h"
#include <iostream>
#include <cassert>
#include <math.h>


//# min (x1 + x2)^2 + x1 + x2 + ( x1 + x2)x3 + x3 + ( x1 + x2)x4 + x4
//# st.
//#     x1 + x2 + x1x2 = 100
//#     0< x2^2 + x3*x1 + x3^2 + x3 + x1 + x2 < 600
//#     0< x1^2 + x4*x2 + x4^2 + x4 + x1 + x2 < 600
//# x1, x2 , x3, x4 free variables

int str_init_x0(double* x0, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	MESSAGE("str_init_x0 -- row " << row <<" col "<<col);
	assert(row == col);
	if(row == 0)
	{
		x0[0] = 1.0;
		x0[1] = 1.0;
	}
	else if(row == 1)
	{
		x0[0] = 1.0;
	}
	else if(row == 2)
	{
		x0[0] = 1.0;
	}
	else
		assert(false);


	return 1;
}

int str_prob_info(int* n, double* col_lb, double* col_ub, int* m,
		double* row_lb, double* row_ub, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	MESSAGE("str_prob_info -- row " << row <<" col "<<col);
	assert(row == col);
	if(col_lb == NULL)
	{
		assert(row_lb == NULL);
		assert(row_ub == NULL);
		assert(col_ub == NULL);
		if(row==0)
		{
			*n = 2;
			*m = 1;
		}
		else if(row ==1 || row == 2)
		{
			*n = 1;
			*m = 1;
		}
		else
			assert(false);
	}
	else
	{
		if(row==0)
		{
			assert(*n==2 && *m == 1);
			col_lb[0] = -INFINITY;
			col_lb[1] = -INFINITY;
			col_ub[0] = INFINITY;
			col_ub[1] = INFINITY;
			row_lb[0] = 100;
			row_ub[0] = 100;
		}
		else if(row ==1 || row == 2)
		{
			assert(*n==1 && *m ==1 );
			col_lb[0] = -INFINITY;
			col_ub[0] = INFINITY;
			row_lb[0] = 0;
			row_ub[0] = 600;
		}
		else
			assert(false);
	}

	return 1;
}

int str_eval_f(double* x0, double* x1, double* obj, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	MESSAGE("str_prob_info -- row " << row <<" col "<<col );
	assert(row == col);
	if(row == 0 )
	{   // (x0 + x1) ^ 2 + x0 + x1
		*obj =  ( x0[0] + x0[1] ) * ( x0[0] + x0[1] ) + x0[0] + x0[1];
	}
	else if(row == 1)
	{   // (x0 + x1)x3 + x3
		*obj = ( x0[0] + x0[1] ) * x1[0] + x1[0];
	}
	else if(row == 2)
	{  // (x0 + x1)x4 + x4
		*obj = ( x0[0] + x0[1] ) * x1[0] + x1[0];
	}
	else
		assert(false);
	return 1;
}

int str_eval_g(double* x0, double* x1, double* eq_g, double* inq_g,
		CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	MESSAGE("str_eval_g  -- row " << row <<" col "<<col);
	assert(row == col);
	if(row == 0)
	{	//x1 + x2 + x1x2 = 100
		eq_g[0] = x0[0] + x0[1] + x0[0]*x0[1];
	}
	else if(row == 1)
	{   //0< x2^2 + x3x1 + x1 + x2 + x3^2 + x3<600
		inq_g[0] = x0[1]*x0[1] + x1[0] * x0[0] + x0[0] + x0[1] + x1[0]*x1[0] + x1[0];
	}
	else if(row == 2)
	{  //0< x1^2 + x4x2 + x1 + x2 + x4^2 + x4< 600
		inq_g[0] = x0[0]*x0[0] + x1[0] * x0[1] + x0[0] + x0[1] + x1[0]*x1[0] + x1[0];
	}
	else
		assert(false);

	return 1;
}

int str_eval_grad_f(double* x0, double* x1, double* grad, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	MESSAGE("str_eval_grad_f -- row " << row <<" col "<<col );

	if(row == 0 && col == 0)
	{
		grad[0] = 2.0 * (x0[0] + x0[1]) + 1.0;
		grad[1] = 2.0 * (x0[0] + x0[1]) + 1.0;
	}
	else if(row == 1 && col == 1)
	{
		grad[0] = (x0[0] + x0[1]) + 1.0;
	}
	else if(row == 2 && col == 2)
	{
		grad[0] = (x0[0] + x0[1]) + 1.0;
	}
	else if(row == 1 && col == 0)
	{
		grad[0] = x1[0];
		grad[1] = x1[0];
	}
	else if(row == 2 && col == 0)
	{
		grad[0] = x1[0];
		grad[1] = x1[0];
	}
	else
		assert(false);

	return 1;
}

int str_eval_jac_g(double* x0, double* x1, int* e_nz, double* e_elts,
		int* e_rowidx, int* e_colptr, int* i_nz, double* i_elts, int* i_rowidx,
		int* i_colptr, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	MESSAGE("str_eval_jac_g  -- row " << row <<" col "<<col);
	if(e_elts==NULL && i_elts == NULL)
	{
		assert(e_elts == NULL && e_rowidx == NULL && e_colptr == NULL);
		assert(i_elts == NULL && i_rowidx == NULL && i_colptr == NULL);
		if (row == 0 && col == 0) {
			*e_nz = 2;
			*i_nz = 0;
		} else if (row == 1 && col == 1) {
			*e_nz = 0;
			*i_nz = 1;
		} else if (row == 2 && col == 2) {
			*e_nz = 0;
			*i_nz = 1;
		} else if (row == 1 && col == 0) {
			*e_nz = 0;
			*i_nz = 2;
		} else if (row == 2 && col == 0) {
			*e_nz = 0;
			*i_nz = 2;
		} else
			assert(false);
	}
	else
	{
		if (row == 0 && col == 0) {
			assert(*i_nz == 0 && *e_nz == 2);
			e_rowidx[0] = 0;
			e_rowidx[1] = 0;
			e_colptr[0] = 0;
			e_colptr[1] = 1;
			e_colptr[2] = 2;
			e_elts[0] = x0[1] + 1.0;
			e_elts[1] = x0[0] + 1.0;
		} else if (row == 1 && col == 1) {
			assert(*i_nz == 1 && *e_nz == 0);
			i_rowidx[0] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_elts[0] = x0[0] + 2.0*x1[0] + 1.0;
		} else if (row == 2 && col == 2) {
			assert(*i_nz == 1 && *e_nz == 0);
			i_rowidx[0] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_elts[0] = x0[1] + 2.0*x1[0] + 1.0;
		} else if (row == 1 && col == 0) {
			assert(*i_nz == 2 && *e_nz == 0);
			i_rowidx[0] = 0;
			i_rowidx[1] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_colptr[2] = 2;
			i_elts[0] = x1[0] + 1.0;
			i_elts[1] = 2.0*x0[1] + 1.0;
		} else if (row == 2 && col == 0) {
			assert(*i_nz == 2 && *e_nz == 0);
			i_rowidx[0] = 0;
			i_rowidx[1] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_colptr[2] = 2;
			i_elts[0] = 2.0*x0[0] + 1.0;
			i_elts[1] = x1[0] + 1.0;
		} else
			assert(false);
	}


	return 1;
}

int str_eval_h(double* x0, double* x1, double* lambda, int* nz, double* elts,
		int* rowidx, int* colptr, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	MESSAGE("str_eval_h  -- row " << row <<" col "<<col);
	if(elts==NULL)
	{
		assert(rowidx == NULL);
		assert(colptr == NULL);
		if (row == 0 && col == 0) {
			*nz = 3;
		} else if (row == 1 && col == 1) {
			*nz = 1;
		} else if (row == 2 && col == 2) {
			*nz = 1;
		} else if (row == 1 && col == 0) {
			*nz = 3;
		} else if (row == 2 && col == 0) {
			*nz = 3;
		} else if (row == 0 && col == 1) {
			*nz = 2;
		} else if (row == 0 && col == 2) {
			*nz = 2;
		} else
			assert(false);
	}
	else{
		if (row == 0 && col == 0) {
			rowidx[0] = 0;
			rowidx[1] = 1;
			rowidx[2] = 1;
			colptr[0] = 0;
			colptr[1] = 2;
			colptr[2] = 3;
			elts[0] = 2.0;
			elts[1] = 2.0 + 1.0 * lambda[0];
			elts[2] = 2.0;
		} else if (row == 1 && col == 1) {
			rowidx[0] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			elts[0] = 2.0 * lambda[0];
		} else if (row == 2 && col == 2) {
			rowidx[0] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			elts[0] = 2.0 * lambda[0];
		} else if (row == 1 && col == 0) { //parent contribution
			rowidx[0] = 0;
			rowidx[1] = 1;
			rowidx[2] = 1;
			colptr[0] = 0;
			colptr[1] = 2;
			colptr[2] = 3;
			elts[0] = 0.0;
			elts[1] = 0.0;
			elts[2] = 2.0 * lambda[0];
		} else if (row == 2 && col == 0) { //parent contribution
			rowidx[0] = 0;
			rowidx[1] = 1;
			rowidx[2] = 1;
			colptr[0] = 0;
			colptr[1] = 2;
			colptr[2] = 3;
			elts[0] = 2.0 * lambda[0];
			elts[1] = 0.0;
			elts[2] = 0.0;
		} else if (row == 0 && col == 1) {
			rowidx[0] = 0;
			rowidx[1] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			colptr[2] = 2;
			elts[0] = 1.0 + 1.0*lambda[0];
			elts[0] = 1.0;
		} else if (row == 0 && col == 2) {
			rowidx[0] = 0;
			rowidx[1] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			colptr[2] = 2;
			elts[0] = 1.0;
			elts[0] = 1.0 + 1.0*lambda[0];
		} else
			assert(false);
	}

	return 1;
}

int str_write_solution(double* x, double* lam_eq, double* lam_ieq,CallBackDataPtr cbd)
{
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	assert(row == col);
	MESSAGE("write_solution  -- row " << row <<" col "<<col);
	if(row == 0)
	{
		PRINT_ARRAY("node = 0 - x ", x, 2);
		PRINT_ARRAY("node = 0 - eq ", lam_eq, 1);
		PRINT_ARRAY("node = 0 - ieq ", lam_ieq , 0);
	}
	else if(row == 1 || row == 2)
	{
		PRINT_ARRAY("node = "<<row<<" - x ", x, 2);
		PRINT_ARRAY("node = "<<row<<" - eq ", lam_eq, 0);
		PRINT_ARRAY("node = "<<row<<" - ieq ", lam_ieq , 1);
	}
	else
	{
		assert(false);
	}
	return 1;
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	MESSAGE("start");
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_rank(comm, &gmyid);
	MPI_Comm_size(comm, &gnprocs);

	str_init_x0_cb init_x0 = &str_init_x0;
	str_prob_info_cb prob_info = &str_prob_info;
	str_eval_f_cb eval_f = &str_eval_f;
	str_eval_g_cb eval_g = &str_eval_g;
	str_eval_grad_f_cb eval_grad_f = &str_eval_grad_f;
	str_eval_jac_g_cb eval_jac_g = &str_eval_jac_g;
	str_eval_h_cb eval_h = &str_eval_h;
	str_write_solution_cb write_solution = &str_write_solution;

	PipsNlpProblemStructPtr prob = CreatePipsNlpProblemStruct(MPI_COMM_WORLD, 2,
			init_x0, prob_info, eval_f, eval_g, eval_grad_f, eval_jac_g,
			eval_h, str_write_solution, NULL);

	MESSAGE("problem created");

	PipsNlpSolveStruct(prob);

	MESSAGE("end solve ");
	MPI_Barrier(comm);
    MPI_Finalize();
}
