#ifndef SYSTEM_H
#define SYSTEM_H

#include "node.h"

class System 
{
private:
	int _nodes;
	int _rods;
	Rod** _syst;
	Node* _nds;
	int** _index_matrix;
	Matrix _forces;
	Matrix gsm;
public:
	System(Rod** syst, Node* k, int** index, int nodes, int rods, Matrix& forc) : _syst(syst), _nds(k), 
		_index_matrix(index), _nodes(nodes), _rods(rods), _forces(forc) {mkgsm();}
	System();
	void mkgsm();                                               
	void bcond();
	void calculate();
	Matrix mktrg(Matrix&, Matrix);
	Node* getN() const {return _nds;}
	Rod** getR() const {return _syst;}
	int getNN() const {return _nodes;}
	int getRN() const {return _rods;}
	Matrix getF() const {return _forces;}
	~System();
};


#endif
