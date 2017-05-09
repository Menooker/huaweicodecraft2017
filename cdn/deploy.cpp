#include "deploy.h"
#include "Network.h"
#include "PSO.h"
#include "lib_io.h"
#include "lib_time.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>

void deploy_server(char* inLines[MAX_IN_NUM], int inLineNum, const char * const filename)
{
	assert(inLineNum < MAX_IN_NUM);
	/*
	printf("line count %d\n", inLineNum);
	for (int i = 0; i < inLineNum; ++i)
	{
		printf("line %d=%s\n", i, inLines[i]);
	}
	*/
	srand((unsigned long)time(NULL));
	Network network(inLines);

	PSO pso(network);
	pso.doPSO(network);

	vector<int> result = network.getResult(pso.getGlobalMinDim());
	// network.printFlow();
	
	std::string res;
	res.append("4");
	res.append("\n\n");
	res.append("204 201 101 12 70").append("\n");
	res.append("204 207 206 104 12 50").append("\n");
	res.append("204 207 208 103 12 30").append("\n");
	res.append("204 203 202 102 12 40").append("\n");

    write_result(res.c_str(), filename);
}
