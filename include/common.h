﻿
#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <iostream>
#include <assert.h>
#include "const.h"
#include <iostream>
#include <math.h>
#include <float.h>

static double C_lbDom = 0., C_rbDom = 1.; //   -  Left and right boundaries of rectangular domain.
static double *masOX; //   -  Massive of OX points. Dimension = C_numOfOXSt +1.
static double C_bbDom = 0., C_ubDom = 1.; //   -  Botton and upper boundaries of rectangular domain.
static double *masOY; //   -  Massive of OY points. Dimension = C_numOfOYSt +1.
static double C_tau; //   -  Time step. Will be computed by relation "C_StepsRel" with small correction.
static int C_numOfTSt; //   -  A number of time steps.

inline void initCompOfGlVar()
{
	double maxOfGrSt = 0.;
	int j;

	//   Massive of OX steps. Dimension = C_numOfOXSt +1.
	masOX = new double[ C_numOfOXSt +1];
	for( j=0; j< C_numOfOXSt +1; j++ )
	{
		//   Regular grid for simplification.
		masOX[j] = C_lbDom + (C_rbDom - C_lbDom) * ( (double)j / C_numOfOXSt );
	}

	//   Let's find out maximum of OX step.
	for( j=0; j< C_numOfOXSt; j++ )
	{
		if( maxOfGrSt < (masOX[j+1] - masOX[j]) )
		{
			maxOfGrSt = (masOX[j+1] - masOX[j]);
		}
	}

	//   Massive of OY steps. Dimension = C_numOfOYSt +1.
	masOY = new double[ C_numOfOYSt +1];
	for( j=0; j< C_numOfOYSt +1; j++ )
	{
		//   Regular grid for simplification.
		masOY[j] = C_bbDom + (C_ubDom - C_bbDom) * ( (double)j / C_numOfOYSt );
	}

	//   Let's find out maximum of OY step.
	for( j=0; j< C_numOfOYSt; j++ )
	{
		if( maxOfGrSt < (masOY[j+1] - masOY[j]) )
		{
			maxOfGrSt = (masOY[j+1] - masOY[j]);
		}
	}

	C_tau = C_StepsRel * maxOfGrSt;

	C_numOfTSt = (int)( C_timeEnd / C_tau );


	if( fabs(C_numOfTSt*C_tau - C_timeEnd) < (C_tau* 1.e-7) )
	{
		//   O.K. This error is error of computer arithmetic.

		return;
	}

	if ( fabs(C_numOfTSt*C_tau - C_timeEnd) >= (C_tau* 1.e-7) )
	{
		//   We need to change number of time steps and time step.

		//cout<<"\nA number of time steps and time step itself have changed."<< flush;

		C_numOfTSt++;

		C_tau = C_timeEnd / C_numOfTSt;

		//cout<<"\nNow time step = "<< C_tau << " and a number of such time steps = " << C_numOfTSt << flush;

		//cout<<"\nSo finish time is "<< C_tau*C_numOfTSt <<flush;

		return;
	}
}

inline void memClean()
{
	delete[] masOX;
	delete[] masOY;
	return;
}

struct ComputeParameters
{
	private:
		int time_i;
		bool _initresult;
	public:
		double a;
		double b;
		double lb;
		double rb;
		double bb;
		double ub;
		double tau;

		int size;
		int t_count;
		int x_size;
		int y_size;
		int i;
		int j;
		double *result;
		double currentTimeLevel;
		double *diff;
		bool _initdiff;

		ComputeParameters(int level, bool initresult, bool initdiff = false) : currentTimeLevel(1), t_count(0)
	{
		_initresult = initresult;
		_initdiff = initdiff;

		a = C_par_a;
		b = C_par_b;
		lb = C_lbDom;
		rb = C_rbDom;
		bb = C_bbDom;
		ub = C_ubDom;

		double value = pow(2., level);
		int n = C_numOfOXSt * value;
		tau = C_tau / value;
		t_count = C_numOfTSt * value;

		x_size = n;

		int m = C_numOfOYSt * value;

		y_size = m;
		size = (n + 1) * (m + 1);
		result = NULL;
		if (initresult)
		{
			result = new double[size];
		}
		if (_initdiff)
		{
			diff = new double[size];
		} 

	}

		~ComputeParameters()
		{
			if (_initresult)
				delete[] result;
			if (_initdiff)
				delete[] diff;
		}


		void reset_time_counter()
		{
			time_i = 1;
		}

		bool can_iterate_over_time_level()
		{
			return time_i <= t_count;
		}

		void inc_time_level()
		{
			time_i++;
		}

		int get_real_x_size()
		{
			return x_size + 1;
		}


		int get_real_y_size()
		{
			return y_size + 1;
		}

		// получает размер внутренней матрицы
		int get_inner_matrix_size()
		{
			return (get_real_x_size() - 2) * (get_real_y_size() - 2);
		}

		int get_real_matrix_size()
		{
			return get_real_x_size() * get_real_y_size();
		}

		// получает размер внутренней матрицы
		int get_inner_x_size()
		{
			return get_real_x_size() - 2;
		}


		void print_info()
		{
			std::cout << "current time level " << currentTimeLevel << std::endl;
		}


		friend std::ostream &operator<<( std::ostream &output,
				const ComputeParameters &tr )
		{
			output << "a = " << tr.a << std::endl;
			output << "b = " << tr.b << std::endl;
			output << "lb = " << tr.lb << std::endl;
			output << "rb = " << tr.rb << std::endl;
			output << "bb = " << tr.bb << std::endl;
			output << "ub = " << tr.ub << std::endl;
			output << "size = " << tr.size << std::endl;
			output << "tau = " << tr.tau << std::endl;
			output << "Time levels = " << tr.t_count << std::endl;
			output << "x size = " << tr.x_size << std::endl;
			output << "y size = " << tr.y_size << std::endl;
			return output;
		}
};

struct VertexData
{
	int* types;
	int length;
	int x_length;
	int y_length;
	int currentTimeLevel;
	int part_number;
	int offset;
	int chunk;
};

template<typename CharT>
class DecimalSeparator : public std::numpunct<CharT>
{
	public:

		DecimalSeparator(CharT Separator)
			: m_Separator(Separator)
		{
		}

	protected:

		CharT do_decimal_point()const
		{
			return m_Separator;
		}

	private:
		CharT m_Separator;
};

struct ComputeResults
{
	int timeStepCount;
	int xSize;
	int ySize;
	double cpuTime;
	double openmpTime;
	double gpuTime;
};


void print_vector(const int, const double*);

const std::string currentDateTime();

void print_matrix(const int, const int, const double*);

void print_matrix(const int, const int, const int*);

void write_csv(double, double, double, const int, const int, const double, const std::string&, const std::string&);

void write_info(double, double, double, const int, const int, double);

void write_openmp_stress_test_info(std::string &filename, int threadNumber, int n, int m, int tl_number, double time_ms, int ht_on, bool append);


extern double h_analytSolut(double t, double x, double y );
extern double h_f_function(ComputeParameters p, const int currentTimeLevel, const int i, const int j);
extern double h_f_function(ComputeParameters* p, const int currentTimeLevel, const int i, const int j);

extern double h_rightBound(ComputeParameters& p);
extern double h_rightBound(ComputeParameters* p);


extern double u_function(double par_b, double t, double x, double y);
extern double v_function(double lbDom, double rbDom, double bbDom, double ubDom, double t, double x, double y );

extern double h_leftBound(ComputeParameters& p);
extern double h_leftBound(ComputeParameters* p);

extern double h_upperBound(ComputeParameters& p);
extern double h_upperBound(ComputeParameters* p);

extern double h_bottomBound(ComputeParameters& p);
extern double h_bottomBound(ComputeParameters* p);

extern double d_initDataOfSol(ComputeParameters* p, int i, int j);

extern float solve_cuda_params(ComputeParameters p);

extern double d_solByEqualVolumes(ComputeParameters p);

extern void h_quadrAngleType(ComputeParameters* p, double* first_x1, double* second_x1, double* third_x1, double* first_x2, double* 
		second_x2, double* third_x2,
		double* first_y1, double* second_y1, double* third_y1, double* first_y2, double* second_y2, double* third_y2);



extern double d_integUnderUnunifTr(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		double par_b, //   -  Item of second parameter from "u_funcion".
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Botton and upper boundaries of rectangular domain.
		double ubDom,
		//
		double tau,
		int iCurrTL, //   -  Index of current time layer.
		//
		double * firVer, //   -  First vertex of triangle.
		double * secVer, //   -  Second vertex of triangle.
		double * thiVer, //   -  Third vertex of triangle.
		//
		const double * masOX, //   -  Massive of OX steps. Dimension = numOfOXSt +1.
		int numOfOXSt, //   -  Number of OX steps.
		//
		const double * masOY, //   -  Massive of OY steps. Dimension = numOfOYSt +1.
		int numOfOYSt, //   -  Number of OY steps.
		//
		double * rhoInPrevTL_asV,
		int ii, int jj );

extern double integUnderUnunifTr(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		double par_b, //   -  Item of second parameter from "u_funcion".
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Botton and upper boundaries of rectangular domain.
		double ubDom,
		//
		double tau,
		int iCurrTL, //   -  Index of current time layer.
		//
		double * firVer, //   -  First vertex of triangle.
		double * secVer, //   -  Second vertex of triangle.
		double * thiVer, //   -  Third vertex of triangle.
		//
		const double * masOX, //   -  Massive of OX steps. Dimension = numOfOXSt +1.
		int numOfOXSt, //   -  Number of OX steps.
		//
		const double * masOY, //   -  Massive of OY steps. Dimension = numOfOYSt +1.
		int numOfOYSt, //   -  Number of OY steps.
		//
		double * rhoInPrevTL_asV,
		int ii, int jj ); //!!!!!!!!!!!!!!!!!!!


extern double solByEqualVolumes(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		double par_b, //   -  Item of second parameter from "u_funcion".
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Bottom and upper boundaries of rectangular domain.
		double ubDom,
		//
		double tau, //   -  Time step.
		int numOfTSt, //   -  A number of time steps.
		//
		double *masOX, //   -  Massive of OX points. Dimension = numOfOXSt +1.
		int numOfOXSt, //   -  Number of OX steps.
		//
		double *masOY, //   -  Massive of OY points. Dimension = numOfOYSt +1.
		int numOfOYSt, //   -  Number of OY steps.
		//
		int numOfSolOrd, //   -  For print only. Solution order which we want to get.
		//
		double *rhoInCurrTL_asV ); //   -  Rho (solution) in Last Time Level which we will compute.

		extern double* solve_cpu_test(
				double par_a, //   -  Item of left and right setback (parameter "a" in test).
				double par_b, //   -  Item of second parameter from "u_funcion".
				//
				double lbDom, //   -  Left and right boundaries of rectangular domain.
				double rbDom,
				//
				double bbDom, //   -  Botton and upper boundaries of rectangular domain.
				double ubDom,
				//
				double tau, //   -  Time step.
				int numOfTSt, //   -  A number of time steps.
				//
				double * masOX, //   -  Massive of OX nodes. Dimension = numOfOXSt +1.
				int numOfOXSt, //   -  Number of OX steps.
				//
				double * masOY, //   -  Massive of OY nodes. Dimension = numOfOYSt +1.
				int numOfOYSt, //   -  Number of OY steps.
				int gridStep,
				bool isComputeDiff);

		extern double spaceVolumeInPrevTL(
				double par_a, //   -  Item of left and right setback (parameter "a" in test).
				double par_b, //   -  Item of second parameter from "u_funcion".
				//
				double lbDom, //   -  Left and right boundaries of rectangular domain.
				double rbDom,
				//
				double bbDom, //   -  Botton and upper boundaries of rectangular domain.
				double ubDom,
				//
				double tau,
				double iCurrTL, //   -  Index of current time layer. Necessary for velocity.
				//
				int iOfOXN, //   -  Index of current OX node.
				const double * masOX, //   -  Massive of OX steps. Dimension = numOfOXSt +1.
				int numOfOXSt, //   -  Number of OX steps.
				//
				int iOfOYN, //   -  Index of current OY node.
				const double * masOY, //   -  Massive of OY steps. Dimension = numOfOYSt +1.
				int numOfOYSt, //   -  Number of OY steps.	

				double * rhoInPrevTL_asV );

				extern double f_function( //   -  It's item of right part of differential equation.
						double par_a, //   -  Item of left and right setback (parameter "a" in test).
						double par_b, //   -  Item of second parameter from "u_funcion".
						//
						double lbDom, //   -  Left and right boundaries of rectangular domain.
						double rbDom,
						//
						double bbDom, //   -  Bottom and upper boundaries of rectangular domain.
						double ubDom,
						//
						double tau,
						int iCurrTL, //   -  Index of current time layer.
						//
						int iOfOXN, //   -  Index of current OX node.
						const double *masOX, //   -  Massive of OX steps. Dimension = numOfOXSt +1.
						int numOfOXSt, //   -  Number of OX steps (segments).
						//
						int iOfOYN, //   -  Index of current OY node.
						const double *masOY, //   -  Massive of OY steps. Dimension = numOfOYSt +1.
						int numOfOYSt ); //   -  Number of OY steps (segments).

extern double rightBound(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Bottom and upper boundaries of rectangular domain.
		double ubDom,
		//
		double t,
		double y );

extern double bottonBound(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Botton and upper boundaries of rectangular domain.
		double ubDom,
		//
		double t,
		double x );

extern double upperBound(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Botton and upper boundaries of rectangular domain.
		double ubDom,
		//
		double t,
		double x );

extern double leftBound(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Botton and upper boundaries of rectangular domain.
		double ubDom,
		//
		double t,
		double y );

extern double integUnderUnunifTr(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		double par_b, //   -  Item of second parameter from "u_funcion".
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Bottom and upper boundaries of rectangular domain.
		double ubDom,
		//
		double tau,
		int iCurrTL, //   -  Index of current time layer.
		//
		double * firVer, //   -  First vertex of triangle.
		double * secVer, //   -  Second vertex of triangle.
		double * thiVer, //   -  Third vertex of triangle.
		//
		const double * masOX, //   -  Massive of OX steps. Dimension = numOfOXSt +1.
		int numOfOXSt, //   -  Number of OX steps.
		//
		const double * masOY, //   -  Massive of OY steps. Dimension = numOfOYSt +1.
		int numOfOYSt, //   -  Number of OY steps.
		//
		double * rhoInPrevTL );

		extern int quadrAngleType(
				double par_a, //   -  Item of left and right setback (parameter "a" in test).
				double par_b, //   -  Item of second parameter from "u_funcion".
				//
				double lbDom, //   -  Left and right boundaries of rectangular domain.
				double rbDom,
				//
				double bbDom, //   -  bottom and upper boundaries of rectangular domain.
				double ubDom,
				//
				double tau,
				double iCurrTL, //   -  Index of current time layer. Necessary for velocity.
				//
				int iOfOXN, //   -  Index of current OX node.
				const double * masOX, //   -  Massive of OX steps. Dimension = numOfOXSt +1.
				int numOfOXSt, //   -  Number of OX steps.
				//
				int iOfOYN, //   -  Index of current OY node.
				const double * masOY, //   -  Massive of OY steps. Dimension = numOfOYSt +1.
				int numOfOYSt, //   -  Number of OY steps.
				//
				double * firVfirT, //   -  First vertex of first triangle.
				double * secVfirT, //   -  Second vertex of first triangle.
				double * thiVfirT, //   -  Third vertex of first triangle.
				//
				double * firVsecT, //   -  First vertex of second triangle.
				double * secVsecT, //   -  Second vertex of second triangle.
				double * thiVsecT ); //   -  Third vertex of second triangle.

extern double integUnderUnunifTr(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		double par_b, //   -  Item of second parameter from "u_funcion".
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  Bottom and upper boundaries of rectangular domain.
		double ubDom,
		//
		double tau,
		int iCurrTL, //   -  Index of current time layer.
		//
		double * firVer, //   -  First vertex of triangle.
		double * secVer, //   -  Second vertex of triangle.
		double * thiVer, //   -  Third vertex of triangle.
		//
		const double * masOX, //   -  Massive of OX steps. Dimension = numOfOXSt +1.
		int numOfOXSt, //   -  Number of OX steps.
		//
		const double * masOY, //   -  Massive of OY steps. Dimension = numOfOYSt +1.
		int numOfOYSt, //   -  Number of OY steps.
		//
		double * rhoInPrevTL );

		extern
		double itemOfInteg_2SpecType_optimized(
				double Py,
				double Qy,
				//
				double alpha,
				//
				double a,
				double b,
				double betta );

extern double analytSolut(
		double par_a,
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  bottom and upper boundaries of rectangular domain.
		double ubDom,
		//
		double t, double x, double y );
extern float solve_at_gpu(ComputeParameters* p, bool tl1, bool compute_diff = false);


extern void cuda_solve(
		double par_a, //   -  Item of left and right setback (parameter "a" in test).
		double par_b, //   -  Item of second parameter from "u_funcion".
		//
		double lbDom, //   -  Left and right boundaries of rectangular domain.
		double rbDom,
		//
		double bbDom, //   -  bottom and upper boundaries of rectangular domain.
		double ubDom,
		//
		double tau, //   -  Time step.
		int numOfTSt, //   -  A number of time steps.
		//
		double * masOX, //   -  Massive of OX nodes. Dimension = numOfOXSt +1.
		int numOfOXSt, //   -  Number of OX steps.
		//
		double * masOY, //   -  Massive of OY nodes. Dimension = numOfOYSt +1.
		int numOfOYSt, //   -  Number of OY steps.
		//
		bool isTimeStShBeChan, //   -  Is time step should be change?
		bool isGridStShBeChan, //   -  Is grid step should be change?
		//
		int numOfGrStepLayer ); //   -  How many computations with different grid steps we want to make.

		extern double initDataOfSol(
				double par_a, //   -  Item of left and right setback (parameter "a" in test).
				//
				double lbDom, //   -  Left and right boundaries of rectangular domain.
				double rbDom,
				//
				double bbDom, //   -  Botton and upper boundaries of rectangular domain.
				double ubDom,
				//
				int iOfOXN, //   -  Index of current OX node.
				const double *masOX, //   -  Massive of abscissa grid steps. Dimension = numOfOxSt +1.
				//
				int iOfOYN, //   -  Index of current OY node.
				const double *masOY ); //   -  Massive of ordinate grid steps. Dimension = numOfOySt +1.

extern double normOfMatrAtL1_asV(
		const double *masOX, //   -  Massive of OX grid nodes. Dimension = dimOX.
		int dimOX,
		//
		const double *masOY, //   -  Massive of OY grid nodes. Dimension = dimOY.
		int dimOY,
		//
		double * mat_asV );

extern double u_function(double par_b, double t, double x, double y);
extern double v_function(double lbDom, double rbDom, double bbDom, double ubDom, double t, double x, double y );
#endif
