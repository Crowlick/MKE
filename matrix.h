#ifndef MATRIX_HPP
#define MATRIX_HPP

class Matrix
{
protected:
	int _row;
	int _col;
	double** _matrix;
public:
	Matrix(int, int);
	Matrix(int);
	Matrix();
	Matrix(const Matrix&);
	Matrix& operator = (const Matrix&);
	Matrix operator * (const Matrix&) const;
	Matrix operator + (const Matrix&) const;
	Matrix transp() const;
	double* operator [] (int);
	void swapline(int a, int b) {std::swap(_matrix[a], _matrix[b]);}
	void linecp(const Matrix&, int, int);
	int getRow() const {return _row;}
	friend std::ostream& operator << (std::ostream&, const Matrix&);
	friend std::istream& operator >> (std::istream&, Matrix&);
	~Matrix();
};

class HardMat:public Matrix
{
public:
	HardMat(double);
};

class TurnMat:public Matrix
{
public:
	TurnMat();
};

#endif
