#include "pch.h"
#include "Calculations.h"
#include <math.h>

int Calculations::ColumnsNumber = 10;
int Calculations::RowsNumber = 10000;

double Calculations::H = (double) 1 / Calculations::ColumnsNumber;
double Calculations::T = (double) 1 / Calculations::RowsNumber;

double Calculations::GetExactResult(double t, double x)
{
	return
		pow(
			100 * exp(-2 * (x + t)) - 1
			, -0.5);
}

void Calculations::CalcAndSetApproximateResult(double** matrix, int i, int j)
{
	matrix[i][j] =
		T / H * ((matrix[i - 1][j - 1] - 2 * matrix[i - 1][j] + matrix[i - 1][j + 1]) / H +
			3 / 2 * pow(matrix[i - 1][j], 2) * (matrix[i - 1][j + 1] - matrix[i - 1][j - 1]))
		+ matrix[i - 1][j];
}