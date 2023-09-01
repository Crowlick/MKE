#include <iostream>
#include <stdlib.h>
#include "matrix.h"

Matrix::Matrix(int row, int col): _row(row), _col(col)
{
	if (row <= 0 || col <= 0)
	{
		std::cout << "No\n";
		exit(1);
	}
	_matrix = new double* [_row];
	for (int i = 0 ; i < _row; i++)
		_matrix[i] = new double [_col];
	for (int i = 0 ; i < _row; i++)
		for (int j = 0; j < _col; j++)
			_matrix[i][j] = 0;
}

Matrix::Matrix(int row): _row(row), _col(row)
{
	if (row <= 0)
	{
		std::cout << "No\n";
		exit(1);
	}
	_matrix = new double* [_row];
	for (int i = 0 ; i < _row; i++)
		_matrix[i] = new double [_col];
	for (int i = 0 ; i < _row; i++)
		for (int j = 0; j < _col; j++)
			_matrix[i][j] = 0;
}

Matrix::Matrix(): _row(1), _col(1)
{
	_matrix = new double* [1];
	_matrix[0] = new double [1];
	_matrix[0][0] = 0;
}

Matrix::Matrix(const Matrix& m)
{
	_row = m._row;
	_col = m._col;
	_matrix = new double* [m._row];
	for (int i = 0 ; i < _row; i++)
		_matrix[i] = new double [_col];
	for (int i = 0 ; i < _row; i++)
		for (int j = 0; j < _col; j++)
			_matrix[i][j] = m._matrix[i][j];
}

Matrix& Matrix::operator=(const Matrix& mat)
{
	for (int i = 0; i < _row; i++)
		delete[] _matrix[i];
	delete[] _matrix;
	_row = mat._row;
	_col = mat._col;
	_matrix = new double* [_row];
	for (int i = 0 ; i < _row; i++)
		_matrix[i] = new double [_col];
	for (int i = 0 ; i < _row; i++)
		for (int j = 0; j < _col; j++)
			_matrix[i][j] = mat._matrix[i][j];
	return *this;
}

std::ostream& operator << (std::ostream& out, const Matrix& m)
{
	for (int i = 0 ; i < m._row; i++)
	{
		for (int j = 0; j < m._col; j++)
			printf("%9.3f ", m._matrix[i][j]);
		out << std::endl;
	}
	return out;
}

std::istream& operator >> (std::istream& in, Matrix& m)
{
	for (int i = 0 ; i < m._row; i++)
	{
		for (int j = 0; j < m._col; j++)
			in >> m._matrix[i][j];
	}
	return in;
}

Matrix Matrix::operator * (const Matrix& mat) const
{
	if (_col != mat._row)
	{
		std::cout << "No\n";
		exit(1);
	}
	Matrix res(_row, mat._col); 
	for (int i = 0; i < _row; i++)
	{
		for (int j = 0; j < mat._col; j++)
		{
			double sum = 0;
			for (int n = 0; n < _col; n++)
				sum += _matrix[i][n]*mat._matrix[n][j];
			res._matrix[i][j] = sum;
		}
	}
	return res;
}

Matrix Matrix::operator + (const Matrix& mat) const
{
	if (_row != mat._row || _col != mat._col)
	{
		std::cout << "No\n";
		exit(1);
	}
	Matrix result(_col,_row);
	for (int i = 0; i < _row; i++)
		for (int j = 0; j < _col; j++)
			result._matrix[i][j] = _matrix[i][j] + mat._matrix[i][j];
	return result;
}

Matrix Matrix::transp() const
{
	Matrix mat(_col, _row);
	for (int i = 0; i < _row; i++)
		for (int j = 0; j < _col; j++)
			mat._matrix[j][i] = _matrix[i][j];
	return mat;
}

double* Matrix::operator [](int ind)
{
	if (ind >= _row || ind < 0)
	{
		std::cout << "Out of row\n";
		exit(1);
	}
	return _matrix[ind];
}

void Matrix::linecp(const Matrix& mat, int a, int b)
{
	if (_col != mat._col)
	{
		std::cout << "Matrix are too different\n";
		exit(3);
	}
	if (a >= _row || b >= mat._row || a < 0 || b < 0)
	{
		std::cout << "Out of range\n";
		exit(3);
	}
	for (int i = 0; i < _col; i++)
		_matrix[a][i] = mat._matrix[b][i];
}

Matrix::~Matrix()
{
	for (int i = 0; i < _row; i++)
		delete[] _matrix[i];
	delete[] _matrix;
}

HardMat::HardMat(double par) : Matrix(4)
{
	_matrix[0][0] = par;
	_matrix[0][2] = -par;
	_matrix[2][0] = -par;
	_matrix[2][2] = par;
}

TurnMat::TurnMat() : Matrix(2)
{
	_matrix[0][1] = -1;
	_matrix[1][0] = 1;
}
