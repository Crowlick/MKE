#include <iostream>
#include <fstream>
#include <string>
#include <X11/Xlib.h>
#include <unistd.h>
#include "graph.h"


int main()
{
	System wow;
	wow.calculate();
	Environment env(wow);
	
	dispatch(&env);
	return 0;
}







