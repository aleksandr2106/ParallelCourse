#pragma once
static class Calculations
{
public:
	static int ColumnsNumber;
	static int RowsNumber;

	static double H;
	static double T;

	static double GetExactResult(double t, double x);

	static void CalcAndSetApproximateResult(double** matrix, int i, int j);
};

