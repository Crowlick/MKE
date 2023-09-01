#include <iostream>
#include <fstream>
#include "system.h"
#include "matrix.h"

System::System(): _nodes(0), _rods(0)
{
	std::string files[] = 
	{
		"Params/nodes.txt", 
		"Params/lims.txt",
		"Params/index.txt",
		"Params/parms.txt",
		"Params/forces.txt"
	};
	std::ifstream file(files[0]);
	if (!file.is_open())
	{
		std::cout << "Cannot open file " << files[0] << std::endl;
		exit(-1);
	}
	if (!(file >> _nodes))
	{
		std::cout << "Wrong number of nodes\n";
		exit(-2);
	}
	if (_nodes < 2)
	{
		std::cout << "Number of nodes can't be lower than 2\n";
		exit(-3);
	}
	_nds = new Node[_nodes];
	for (int i = 0; i < _nodes; i++)
	{
		double tmp[] = {0, 0};
		for (int j = 0; j < 2; j++)
			if (!(file >> tmp[j]))
			{
				std::cout << "Wrong value in " << files[0] << " at: " << i + 2 << " " << j + 1 << std::endl;
				exit(-4);
			}
		_nds[i] = Node(tmp[0], tmp[1]);
	}
	file.close();
	file.open(files[1]);
	if (!file.is_open())
	{
		std::cout << "Cannot open file " << files[0] << std::endl;
		exit(-1);
	}
	for (int i = 0; i < _nodes; i++)
	{
		int lim[] = {0, 0};
		for (int j = 0; j < 2; j++)
			if (!(file >> lim[j]))
			{
				std::cout << "Wrong value in " << files[1] << " at: " << i + 1 << " " << j + 1 << std::endl;
				exit(-4);
			}
		_nds[i].setlx(lim[0]);
		_nds[i].setly(lim[1]);
	}
	file.close();
	file.open(files[2]);
	if (!file.is_open())
	{
		std::cout << "Cannot open file " << files[2] << std::endl;
		exit(-1);
	}
	
	
	for (int i = 0; i < _nodes * (_nodes - 1) / 2; i++)
	{
		int tmp = 0;
		for (int j = 0; j < 2; j++)
		{
			if (!(file >> tmp) || tmp < 0)
				{
					if (file.eof() && j == 0)
					{
						i = _nodes * (_nodes - 1) / 2;
						_rods--;
						break;
					}
					std::cout << "Wrong value in " << files[2] << " at: " << i + 1 << " " << j + 1 << std::endl;
					exit(-4);
				}
		}
		_rods++;
	}
	file.close();
	file.open(files[2]);
	if (!file.is_open())
	{
		std::cout << "Cannot open file " << files[2] << std::endl;
		exit(-1);
	}
	_index_matrix = new int* [_rods];
	for (int i = 0; i < _rods; i++)
		_index_matrix[i] = new int[2];
	for (int i = 0; i < _rods; i++)
	{
		for (int j = 0; j < 2; j++)
			file >> _index_matrix[i][j];
		if (_index_matrix[i][0] > _nodes || _index_matrix[i][1] > _nodes)
		{
			std::cout << "Wrong input in: " << i << std::endl;
			exit(-4);
		}
	}

	file.close();
	file.open(files[3]);
	if (!file.is_open())
	{
		std::cout << "Cannot open file " << files[3] << std::endl;
		exit(-1);
	}
	_syst = new Rod* [_rods];
	for (int i = 0; i < _rods; i++)
	{
		double tmp[] = {0, 0};
		for (int j = 0; j < 2; j++)
			if (!(file >> tmp[j]))
			{
				if (file.eof())
					std::cout << "Unexpected EOF in " << files[3] << " at: " << i + 1 << std::endl;
				else
					std::cout << "Wrong value in " << files[3] << " at: " << i + 1 << std::endl;
				exit(-4);
			}
		_syst[i] = new Rod(_nds[_index_matrix[i][0] - 1], _nds[_index_matrix[i][1] - 1], tmp[0], tmp[1]);
	}
	file.close();
	file.open(files[4]);
	if (!file.is_open())
	{
		std::cout << "Cannot open file " << files[4] << std::endl;
		exit(-1);
	}
	_forces = Matrix(1, _nodes * 2);
	for (int i = 0; i < _nodes * 2; i++)
	{
		if (!(file >> _forces[0][i]))
		{
			std::cout << "Wrong value in " << files[4] << " at: " << i + 1 << std::endl;
			exit(-4);
		}
	}
	
	_forces = _forces.transp();
	file.close();
	mkgsm();
}

void System::mkgsm()
{
	gsm = Matrix(_nodes * 2);
	//std::cout << gsm << std::endl;
	int ind[] = {0,0,0,0};
	for (int i = 0; i < _rods; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			int n = _index_matrix[i][j] * 2 - 1;
			ind[j * 2] = n - 1;
			ind[j * 2 + 1] = n;
		}
		for (int j = 0; j < 4; j++)
		{
			int n = ind[j];
			for (int k = 0; k < 4; k++)
			{
				int m = ind[k];
				gsm[n][m] = gsm[n][m] + _syst[i]->_stiffness_matrix[j][k];
			}
		}
	}
}

void System::bcond() 
{
	for (int i = 0; i < _nodes; i++)
	{
		if (_nds[i].getlx())
		{
			for (int m = 0; m < gsm.getRow(); m++)
			{
				gsm[i*2][m] = 0;
				gsm[m][i*2] = 0;
			}
			gsm[i*2][i*2] = 1;
			_forces[i*2][0] = 0;
		}
		if (_nds[i].getly())
		{
			for (int m = 0; m < gsm.getRow(); m++)
			{
				gsm[i*2+1][m] = 0;
				gsm[m][i*2+1] = 0;
			}
			gsm[i*2+1][i*2+1] = 1;
			_forces[i*2+1][0] = 0;
		}
	}
}

Matrix System::mktrg(Matrix& r, Matrix l)
{
	double n = 0;
	for (int i = 0; i < r.getRow(); i++)
	{
		
		if (r[i][i] == 0)
		{
			int c = i;
			while (r[c][i] == 0)
				if (++c >= r.getRow())
					return l;
			r.swapline(c, i);
		}
		for (int k = i; k + 1 < r.getRow(); k++)
		{
			n = r[k + 1][i];
			for (int j = i; j < r.getRow(); j++)
			{
				r[k+1][j] -=  r[i][j] * n / r[i][i];
			}
			l[k+1][0] -=  l[i][0] * n / r[i][i];
		}
	}
	return l;
}

void System::calculate() 
{
	Matrix movement(_nodes * 2, 1);
	Matrix dop;
	//std::cout << gsm << std::endl;
	bcond();
	std::cout << _forces << std::endl;
	//std::cout << gsm << std::endl;
	dop = mktrg(gsm, _forces);
	for (int i = _nodes * 2 - 1; i >= 0; i--)
	{
		movement[i][0]  = dop[i][0] / gsm[i][i];
		for (int j = _nodes * 2 - 1; j > i; j--)
			movement[i][0]  -= movement[j][0]*gsm[i][j] / gsm[i][i];
	}
	//std::cout << movement << std::endl;
	
	for (int i = 0; i < _nodes; i++)
		_nds[i].setU(movement[i * 2][0],movement[2 * i + 1][0]);
	/*for (int i = 0; i < _nodes; i++)
	{
		_nds[i].setX(_nds[i].getX() + _nds[i].getU()[0][0]);
		_nds[i].setY(_nds[i].getY() + _nds[i].getU()[1][0]);
	}*/
	for (int i = 0; i < _nodes; i++)
		std::cout << "Node " << i + 1 << ":\n" << _nds[i].getU() << std::endl;
	for (int i = 0; i < _rods; i++)
	{
		_syst[i]->update();
		std::cout << "Deformation of " << i + 1 << " rod: " << _syst[i]->_defs << std::endl;
		std::cout << "Tension in " << i + 1 << " rod: " << _syst[i]->_tens << std::endl;
		std::cout << "Forces in " << i + 1 << " rod: " << _syst[i]->_N << std::endl << std::endl;
	}
	
}

System::~System()
{
	for (int i = 0; i < _rods; i++)
		delete[] _index_matrix[i];
	delete[] _index_matrix;
	for (int i = 0; i < _rods; i++)
		delete _syst[i];
	delete[] _syst;
	delete[] _nds;
}



