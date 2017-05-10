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

	vector<vector<int>> result = network.getResult(pso.getGlobalMin(), pso.getGlobalMinDim());
	// network.printFlow();
	cout << "===== Final Result =====" << endl;
	cout << "Total Cost: " << pso.getGlobalMin() << endl;
	cout << result.size() << " routes: " << endl;
	for (auto e : result) {
		for (auto i : e) {
			cout << i << " ";
		}
		cout << endl;
	}

	stringstream res;
	if (result.size() == 0)
		res << "NA";
	else {
		res << result.size();
		res << endl << endl;
		for (auto e : result) {
			for (auto i : e) {
				res << i << " ";
			}
			res << endl;
		}
	}

    	write_result(res.str().c_str(), filename);
}
