﻿
#include "timer.h"
#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include "model_data_provider.h"
#include <assert.h>     /* assert */
#ifndef COMMON_H_INCLUDED
#include "common.h"
#endif
#define FULL_TEST true

class TestBase : public testing::Test
{
protected:
	double _accuracy;


	ModelDataProvider _modelDataProvider;

	TestBase()
	{
		_accuracy = 1.0e-8;
		initCompOfGlVar();
		_modelDataProvider = ModelDataProvider();
	}

	virtual ~TestBase()
	{
		// You can do clean-up work that doesn't throw exceptions here.
		memClean();
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	virtual void TearDown()
	{
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	int GetSize()
	{
		return C_numOfOXSt * C_numOfOYSt;
	}
};

class CpuTest : public TestBase
{
protected:

	CpuTest()
	{
	}

	virtual ~CpuTest()
	{
	}

	double* GetCpuToLevel(int level)
	{
		return solve_cpu_test(C_par_a, C_par_b, C_lbDom, C_rbDom, C_bbDom,
			C_ubDom, C_tau, C_numOfTSt, masOX, C_numOfOXSt, masOY,
			C_numOfOYSt, level);
	}
};

TEST_F(CpuTest, CpuTestModel11)
{
	double* data = _modelDataProvider.GetModelData(Model11);
	double* result = GetCpuToLevel(0);

	for (int i = 0; i < GetSize(); i++)
	{
		ASSERT_TRUE(abs(data[i] - result[i]) <= _accuracy) << data[i] << " " << result[i];

	}
}

TEST_F(CpuTest, CpuTestModel21)
{
	double* data = _modelDataProvider.GetModelData(Model21);
	double* result = GetCpuToLevel(1);

	for (int i = 0; i < GetSize(); i++)
	{
		EXPECT_TRUE(data[i] - result[i] <= _accuracy);
	}
}

TEST_F(CpuTest, CpuTestModel41)
{
	if (FULL_TEST)
	{
		double* data = _modelDataProvider.GetModelData(Model41);
		double* result = GetCpuToLevel(2);

		for (int i = 0; i < GetSize(); i++)
		{
			EXPECT_TRUE(data[i] - result[i] <= _accuracy);
		}
	}
}

TEST_F(CpuTest, CpuTestModel81)
{
	if (FULL_TEST)
	{
		double* data = _modelDataProvider.GetModelData(Model81);
		double* result = GetCpuToLevel(3);

		for (int i = 0; i < GetSize(); i++)
		{
			ASSERT_TRUE(data[i] - result[i] <= _accuracy);
		}
	}
}

TEST_F(CpuTest, CpuTestModel161)
{
	if (FULL_TEST)
	{
		double* data = _modelDataProvider.GetModelData(Model161);
		double* result = GetCpuToLevel(4);

		for (int i = 0; i < GetSize(); i++)
		{
			ASSERT_TRUE(data[i] - result[i] <= _accuracy);
		}
	}
}

TEST_F(CpuTest, CpuTestModel321)
{
	if (FULL_TEST)
	{
		double* data = _modelDataProvider.GetModelData(Model321);
		double* result = GetCpuToLevel(5);

		for (int i = 0; i < GetSize(); i++)
		{
			ASSERT_TRUE(data[i] - result[i] <= _accuracy);
		}
	}
}

TEST_F(CpuTest, CpuTestModel641)
{
	if (FULL_TEST)
	{
		double* data = _modelDataProvider.GetModelData(Model641);
		double* result = GetCpuToLevel(6);

		for (int i = 0; i < GetSize(); i++)
		{
			ASSERT_TRUE(data[i] - result[i] <= _accuracy);
		}
	}
}

TEST_F(CpuTest, CpuTestModel1281)
{
	if (FULL_TEST)
	{
		double* data = _modelDataProvider.GetModelData(Model1281);
		double* result = GetCpuToLevel(7);

		for (int i = 0; i < GetSize(); i++)
		{
			ASSERT_TRUE(data[i] - result[i] <= _accuracy);
		}
	}
}

class GpuTest : public TestBase
{
protected:


	GpuTest()
	{
	}

	virtual ~GpuTest()
	{
	}
};



TEST_F(GpuTest, get_quad_coord)
{
	const int finishLevel = 7;
	const int startLevel = 0;
	const double error = 1.0e-15;

	for (int level = startLevel; level < finishLevel; ++level)
	{
		std::cout << "level = " << level << std::endl;
		ComputeParameters* p = new ComputeParameters(level, false);
		p->currentTimeLevel = 1;
		TriangleResult* gpu = new TriangleResult(p);
		get_quad_coord(gpu, p);
		double first_x1(0), second_x1(0), third_x1(0), first_x2(0), second_x2(0), third_x2(0);
		double first_y1(0), second_y1(0), third_y1(0), first_y2(0), second_y2(0), third_y2(0);

		for (int j = 1; j < p->y_size; j++)
		{
			for (int i = 1; i < p->x_size; i++)
			{
				p->i = i;
				p->j = j;

				h_quadrAngleType(p, 
					&first_x1, &second_x1, &third_x1, &first_x2, &second_x2, &third_x2,
					&first_y1, &second_y1, &third_y1, &first_y2, &second_y2, &third_y2);
				int c = (p->x_size - 1) * (j - 1) + (i - 1);

				bool b1 = (fabs(gpu->first1[2 * c] - first_x1) < error) && (fabs(gpu->first1[2 * c + 1] - first_y1) < error);
				ASSERT_TRUE(b1) << "c = " << c << std::endl;
				bool b2 = (fabs(gpu->second1[2 * c] - second_x1) < error) && (fabs(gpu->second1[2 * c + 1] - second_y1) < error);
				ASSERT_TRUE(b2) << "c = " << 2 * c << std::endl;
				bool b3 = (fabs(gpu->third1[2 * c] - third_x1) < error) && (fabs(gpu->third1[2 * c + 1] - third_y1) < error);
				ASSERT_TRUE(b3) << "c = " << 2 * c << std::endl;

				ASSERT_TRUE(b1&&b2&&b3) << "c = " << c << std::endl;

				bool b4 = (fabs(gpu->first2[2 * c] - first_x2) < error) && (fabs(gpu->first2[2 * c + 1] - first_y2) < error);
				ASSERT_TRUE(b4) << "c = " << 2 * c << std::endl;

				bool b5 = (fabs(gpu->second2[2 * c] - second_x2) < error) && (fabs(gpu->second2[2 * c + 1] - second_y2) < error);
				ASSERT_TRUE(b5) << "c = " << 2 * c << std::endl;

				bool b6 = (fabs(gpu->third2[2 * c] - third_x2) < error) && (fabs(gpu->third2[2 * c + 1] - third_y2) < error);
				ASSERT_TRUE(b6) << "c = " << 2 * c << std::endl;

				ASSERT_TRUE(b3&&b5&&b6) << "c = " << 2 * c << std::endl;
			}
		}
		delete p;
		delete gpu;
	}
	std::cout << "Done!" << std::endl;
	std::cin.get();
}

TEST_F(GpuTest, get_quad_coord_te)
{
	double time_cpu (-1), time_gpu(0);
	double first_x1(0), second_x1(0), third_x1(0), first_x2(0), second_x2(0), third_x2(0);
	double first_y1(0), second_y1(0), third_y1(0), first_y2(0), second_y2(0), third_y2(0);
	int finish_level = 9;
	int start_level = 8;


	for (int level = start_level; level < finish_level; level++)
	{
		ComputeParameters* p = new ComputeParameters(level, false);
		p->currentTimeLevel = 1;

		std::cout << *p << std::endl;

		printf("Start GPU\n");
		TriangleResult* gpu = new TriangleResult(p);

		time_gpu = get_quad_coord(gpu, p);

		printf("End GPU\n");

		printf("Start CPU\n");

		int y_size;
		int x_size;
		y_size = p->y_size;
		x_size = p->x_size;
		StartTimer();

		for (int j = 1; j < y_size; j++)
		{
			for (int i = 1; i < x_size; i++)
			{
				p->i = i;
				p->j = j;
				h_quadrAngleType(p,
					&first_x1, &second_x1, &third_x1, &first_x2, &second_x2, &third_x2,
					&first_y1, &second_y1, &third_y1, &first_y2, &second_y2, &third_y2);
			}
		}

		time_cpu = GetTimer();

		printf("End CPU\n");
		printf("CPU time is = %f\n", time_cpu);
		printf("GPU time is = %f\n", time_gpu);
		printf("CPU/GPU = %f\n", time_cpu / time_gpu);
		delete gpu;
		delete p;
	}
}

class CpuVersusGpuFunctionalFemTest : public TestBase
{
protected:

	CpuVersusGpuFunctionalFemTest()
	{
	}

	virtual ~CpuVersusGpuFunctionalFemTest()
	{
	}
};

TEST_F(CpuVersusGpuFunctionalFemTest, AnalyticSolutionEqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	double cpu = analytSolut(p->a, p->lb, p->rb, p->bb, p->ub, p->tau, p->x[1],
		p->y[1]);
	double gpu = h_analytSolut(p->tau, p->x[1], p->y[1]);
	ASSERT_EQ(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, AnalyticSolutionNotEqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	double cpu = analytSolut(p->a, p->lb, p->rb, p->bb, p->ub, p->tau, p->x[1],
		p->y[2]);
	double gpu = h_analytSolut(p->tau, p->x[1], p->y[1]);
	ASSERT_NE(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, DataInitializationAnalyticSolutionEqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	double cpu = initDataOfSol(p->a, p->lb, p->rb, p->bb, p->ub, 1, p->x, 2, p->y);
	double gpu = d_initDataOfSol(p, 1, 2);
	ASSERT_EQ(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, F_Function_EqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	int currentTimeLevel = 1;
	int i = 1;
	int j = 1;
	double cpu = f_function(p->a, p->b, p->lb, p->rb, p->bb, p->ub, p->tau,
		currentTimeLevel, i, p->x, p->x_size, j, p->y, p->y_size);
	double gpu = h_f_function(p, currentTimeLevel, i, j);
	ASSERT_EQ(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, F_Function_NotEqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	int currentTimeLevel = 1;
	int i = 1;
	int j = 1;
	double cpu = f_function(p->a, p->b, p->lb, p->rb, p->bb, p->ub, p->tau,
		currentTimeLevel, i, p->x, p->x_size, j + 2, p->y, p->y_size);
	double gpu = h_f_function(p, currentTimeLevel, i + 1, j);
	ASSERT_NE(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, d_bottomBound_EqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	int currentTimeLevel = 1;
	double t = p->tau * 1;
	int i = 1;

	double cpu = bottonBound(p->a, p->lb, p->rb, p->bb, p->ub, t, p->x[i]);

	p->currentTimeLevel = currentTimeLevel;
	p->i = i;
	double gpu = h_bottomBound(p);

	ASSERT_EQ(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, d_upperBound_EqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	int currentTimeLevel = 1;
	double t = p->tau * 1;
	int i = 1;

	double cpu = upperBound(p->a, p->lb, p->rb, p->bb, p->ub, t, p->x[i]);
	p->currentTimeLevel = currentTimeLevel;
	p->i = i;
	double gpu = h_upperBound(p);
	ASSERT_EQ(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, d_rightBound_EqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	int currentTimeLevel = 1;
	double t = p->tau * 1;
	int j = 1;

	double cpu = rightBound(p->a, p->lb, p->rb, p->bb, p->ub, t, p->y[j]);

	p->currentTimeLevel = currentTimeLevel;
	p->j = j;
	double gpu = h_rightBound(p);
	ASSERT_EQ(cpu, gpu);
	delete p;
}

TEST_F(CpuVersusGpuFunctionalFemTest, d_leftBound_EqualsTest)
{
	ComputeParameters* p = new ComputeParameters(0, false);
	int currentTimeLevel = 1;
	double t = p->tau * 1;
	int j = 1;

	double cpu = leftBound(p->a, p->lb, p->rb, p->bb, p->ub, t, p->y[j]);

	p->currentTimeLevel = currentTimeLevel;
	p->j = j;
	double gpu = h_leftBound(p);

	ASSERT_EQ(cpu, gpu);
	delete p;
}