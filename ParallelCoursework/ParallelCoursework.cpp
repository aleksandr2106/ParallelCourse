#include "pch.h"
#include <iostream>
#include "Calculations.h"
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

using namespace std;

static int ThreadsCount = 2;
static int ChunkSize = Calculations::ColumnsNumber / ThreadsCount;

static void Task1(int index, double** matrix)
{
	int indexFrom;
	int indexTo;

	if (index == ThreadsCount - 1)
	{
		indexFrom = index * ChunkSize;
		indexTo = Calculations::RowsNumber - 1;
	}
	else
	{
		indexFrom = index * ChunkSize;
		indexTo = (index + 1) * ChunkSize - 1;
	}

	for (int i = indexFrom; i <= indexTo; i++)
	{
		matrix[i][0] = Calculations::GetExactResult(Calculations::T * i, 0);
		matrix[i][Calculations::ColumnsNumber - 1] =
			Calculations::GetExactResult(Calculations::T * i, 1);
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

	//chrono::time_point<chrono::system_clock> pTimerStart = chrono::system_clock::now();

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


	ofstream fs;
	fs.open("output_aproximate.txt");

	for (int i = 0; i < Calculations::RowsNumber; i++)
	{
		for (int j = 0; j < Calculations::ColumnsNumber; j++)
		{
			fs << dataMatrixAproximate[i][j] << " ";
		}
		fs << endl;
	}


	for (int i = 0; i < Calculations::RowsNumber; i++)
	{
		for (int j = 0; j < Calculations::ColumnsNumber; j++)
		{
			dataMatrixExact[i][j] = Calculations::GetExactResult(Calculations::T * i, Calculations::H * j);
		}
	}
	fs.close();
	fs.open("output_exact.txt");

	for (int i = 0; i < Calculations::RowsNumber; i++)
	{
		for (int j = 0; j < Calculations::ColumnsNumber; j++)
		{
			fs << dataMatrixExact[i][j] << " ";
		}
		fs << endl;
	}


}