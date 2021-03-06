#include "pch.h"
#include <iostream>
#include "Calculations.h"
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

using namespace std;

static int ThreadsCount = 4;
static int ChunkSize = Calculations::ColumnsNumber / ThreadsCount;

static void Task1(int index, double** matrix)
{
	int indexFrom;
	int indexTo;
	int chunkSize = Calculations::RowsNumber / ThreadsCount;

	if (index == ThreadsCount - 1)
	{
		indexFrom = index * chunkSize;
		indexTo = Calculations::RowsNumber - 1;
	}
	else
	{
		indexFrom = index * chunkSize;
		indexTo = (index + 1) * chunkSize - 1;
	}

	for (int i = indexFrom; i <= indexTo; i++)
	{
		matrix[i][0] = Calculations::GetExactResult(Calculations::T * i, 0);
		matrix[i][Calculations::ColumnsNumber - 1] =
			Calculations::GetExactResult(Calculations::T * i, Calculations::H * (Calculations::ColumnsNumber - 1));
	}
}

static void Task2(int index, double** matrix)
{
	int indexFrom;
	int indexTo;

	if (index == ThreadsCount - 1)
	{
		indexFrom = index * ChunkSize;
		indexTo = Calculations::ColumnsNumber - 1;
	}
	else
	{
		indexFrom = index * ChunkSize;
		indexTo = (index + 1) * ChunkSize - 1;
	}

	for (int i = indexFrom; i <= indexTo; i++)
	{
		matrix[0][i] = Calculations::GetExactResult(0, Calculations::H * i);
	}
}

static void Task3(int row, int index, double** matrix)
{
	int indexFrom;
	int indexTo;

	if (index == ThreadsCount - 1)
	{
		indexFrom = index * ChunkSize;
		indexTo = Calculations::ColumnsNumber - 1;
	}
	else
	{
		indexFrom = index * ChunkSize;
		indexTo = (index + 1) * ChunkSize;
	}

	if (indexFrom == 0)
		indexFrom = 1;

	for (int j = indexFrom; j < indexTo; j++)
	{
		Calculations::CalcAndSetApproximateResult(matrix, row, j);
	}
}

static void Deviation(double **array1, double **array2, int row, int column)
{
	double absolute = -1;
	int t_i;
	int t_j;
	double relative;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			double temp_val = abs(array1[i][j] - array2[i][j]);

			if (temp_val > absolute)
			{
				absolute = temp_val;

				t_i = i;
				t_j = j;

			}
		}
	}

	std::cout << "Deviation absolute: " << absolute << std::endl;

	relative = absolute / array2[t_i][t_j];
	std::cout << "Deviatione relative: " << relative << std::endl;
}


int main()
{
	vector<thread> threads;
	double** dataMatrixAproximate = new double*[Calculations::RowsNumber];
	double** dataMatrixExact = new double*[Calculations::RowsNumber];


	for (int i = 0; i < Calculations::RowsNumber; i++)
	{
		dataMatrixAproximate[i] = new double[Calculations::ColumnsNumber];
		dataMatrixExact[i] = new double[Calculations::ColumnsNumber];
	}

	chrono::time_point<chrono::system_clock> TimerStart = chrono::system_clock::now();

	if (ThreadsCount > Calculations::ColumnsNumber)
	{
		ThreadsCount = Calculations::ColumnsNumber;
		ChunkSize = 1;
	}

	int count = 0;
	for (int i = 0; i < ThreadsCount; i++)
	{
		threads.emplace_back(thread(Task2, count++, dataMatrixAproximate));
	}

	for (int i = 0; i < ThreadsCount; i++)
	{
		threads[i].join();
	}
	threads.clear();

	count = 0;
	for (int i = 0; i < ThreadsCount; i++)
	{
		threads.emplace_back(thread(Task1, count++, dataMatrixAproximate));
	}

	for (int i = 0; i < ThreadsCount; i++)
	{
		threads[i].join();
	}
	threads.clear();


	for (int i = 1; i < Calculations::RowsNumber; i++)
	{
		count = 0;

		for (int j = 0; j < ThreadsCount; j++)
		{
			threads.emplace_back(thread(Task3, i, count++, dataMatrixAproximate));
		}
		for (int j = 0; j < ThreadsCount; j++)
		{
			threads[j].join();
		}

		threads.clear();
	}

	cout << "Parallel time: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - TimerStart).count() << endl;

	ofstream fs;
	fs.precision(15);
	fs.open("output_aproximate.txt");

	for (int i = 0; i < Calculations::RowsNumber; i++)
	{
		fs << "{";
		for (int j = 0; j < Calculations::ColumnsNumber; j++)
		{
			fs << dataMatrixAproximate[i][j] << ",";
		}
		fs << "}," << endl;
	}
	fs.close();


	TimerStart = chrono::system_clock::now();

	for (int i = 0; i < Calculations::RowsNumber; i++)
	{
		for (int j = 0; j < Calculations::ColumnsNumber; j++)
		{
			dataMatrixExact[i][j] = Calculations::GetExactResult(Calculations::T * i, Calculations::H * j);
		}
	}

	cout << "One thread time: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - TimerStart).count() << endl;


	fs.open("output_exact.txt");
	for (int i = 0; i < Calculations::RowsNumber; i++)
	{
		fs << "{";
		for (int j = 0; j < Calculations::ColumnsNumber; j++)
		{
			fs << dataMatrixExact[i][j] << ",";
		}
		fs <<  "}," << endl;
	}
	fs.close();
	
	Deviation(dataMatrixAproximate, dataMatrixExact, Calculations::RowsNumber, Calculations::ColumnsNumber);


	getchar();
}
