#include <iostream>
#include <cmath>
#include "node.h"

Rod::Rod(Node& f, Node& s, double e, double a) : _first(f), _second(s), _e(e), _a(a), _defs(0), _N(0), _tens(0)
{
	_d = Matrix(2,1);
	_l = sqrt((f.getX() - s.getX()) * (f.getX() - s.getX()) + (f.getY() - s.getY()) * (f.getY() - s.getY()));
	setLocal();
	create_turn_matrix();
	create_stiffness_matrix();
}

void norm(double* ne_norm) 
{
	double l = sqrt(ne_norm[0] * ne_norm[0] + ne_norm[1] * ne_norm[1]);
	ne_norm[0] = ne_norm[0] / l;
	ne_norm[1] = ne_norm[1] / l;
}

void Rod::create_turn_matrix() 
{
	Matrix v1(2,1);
	v1[0][0] = _second.getX() - _first.getX() ;
	v1[1][0] =  _second.getY() - _first.getY();
	Matrix v2(2,1);
	v2 = TurnMat() * v1;
	Matrix fuz(2, 2);
	v1 = v1.transp();
	v2 = v2.transp();
	fuz.linecp(v1, 0, 0);
	fuz.linecp(v2, 1, 0);
	for (int i = 0; i < 2; i++)
		norm(fuz[i]);
	Matrix turn(4, 4);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			turn[i][j] = fuz[j][i];
	for (int i = 2; i < 4; i++)
		for (int j = 2; j < 4; j++)
			turn[i][j] = fuz[j % 2][i % 2];
	_turn_matrix = turn;
	//std::cout <<  _turn_matrix << std::endl;
}

void Rod::create_stiffness_matrix()
{
	//std::cout << "Origin: \n" <<  _stiffness_matrix << std::endl;
	_stiffness_matrix = _turn_matrix * _stiffness_matrix * _turn_matrix.transp();
	//std::cout << "Rotated\n" << _stiffness_matrix << std::endl;
}

void Rod::update()
{
	Matrix dop(2,4);
	Matrix ex(4,1);
	ex[0][0] = _first.getU()[0][0];
	ex[1][0] = _first.getU()[1][0];
	ex[2][0] = _second.getU()[0][0];
	ex[3][0] = _second.getU()[1][0];
	dop.linecp(_turn_matrix, 0, 0);
	dop.linecp(_turn_matrix, 1, 2);
	dop[0][1] = -dop[0][1];
	dop[1][3] = -dop[1][3];
	//std::cout << "turn: \n" << _turn_matrix << std::endl;
	//std::cout << "dop: \n" << dop << std::endl;
	//std::cout << "ex: \n" << ex << std::endl;
	_d = dop * ex;
	//std::cout << "D: \n" << _d << std::endl;
	//std::cout << "L: " << _l << std::endl;
	_defs = (_d[1][0] - _d[0][0]) / _l;
	_tens = _defs * _e;
	_N = _tens * _a;
}

