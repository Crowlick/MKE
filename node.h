#ifndef Node_H
#define Node_H

#include "matrix.h"

class Node 
{                                                  
private:
	double _x;
	double _y;                                        
	Matrix _u;                                             
	int _limx;
	int _limy;
public:
	Node(double x, double y) : _x(x), _y(y) { _u = Matrix(2,1); }
	Node(): _x(0), _y(0) { _u = Matrix(2,1);}
	Matrix getU() const { return _u; }  
	double getX() const { return _x; }
	double getY() const { return _y; }
	int getlx() const { return _limx; }
	int getly() const { return _limy; }
	void setX(double x) {_x = x;}
	void setY(double y) {_y = y;}
	void setU(double u, double v) { _u[0][0] = u; _u[1][0] = v; }
	void setlx(int i) { _limx = i; }
	void setly(int i) { _limy= i; }
};


class Rod 
{                                                      
private:
	Node& _first;
	Node& _second;
	double _e, _a, _l;
	Matrix _d;
	double _defs;
	double _tens;
	double _N;
	Matrix _stiffness_matrix;
	Matrix _turn_matrix;
public:
	Rod(Node& f, Node& s, double e, double a);
	void setLocal() { _stiffness_matrix = HardMat(_e * _a / _l); }         
	void create_turn_matrix();                                  
	void create_stiffness_matrix();                             
	Node& getFirst() const { return _first; }
	Node& getSecond() const { return _second; }
	void update();
	double getd() const {return _defs;}
	double getten() const {return _tens;}
	double gete() const {return _e;}
	friend class System;
};

#endif
