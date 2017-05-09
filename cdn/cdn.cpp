#include "deploy.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
	print_time("Begin");
	char *topo[MAX_EDGE_NUM];
	int line_num;

	//char *topo_file = argv[1];
	char *topo_file = "case_example///case2.txt";
	//char *topo_file = "case_example//2//2//case2.txt";
	//char *result_file = argv[2];
	char *result_file = "result//result.txt";

	line_num = read_file(topo, MAX_EDGE_NUM, topo_file);

	printf("line num is :%d \n", line_num);
	if (line_num == 0)
	{
		printf("Please input valid topo file.\n");
		return -1;
	}

	deploy_server(topo, line_num, result_file);

	release_buff(topo, line_num);

	print_time("End");

	return 0;
}

/*
int mrand()
{
	static unsigned state1=21347;
	static unsigned state2 = -11347;
	unsigned ret = ((state1 + state1) << 20) ^ (state2*state1) ^ ((state2*state2) & 0x0000ffff);
	state1 = (ret>>16) - state2;
	state2 = (ret & 0x0000ffff) - state1;
	return ret;

}


#include <chrono>
#include <iostream>
int main()
{
	srand(time(NULL));
	auto t=std::chrono::system_clock::now();
	int sum = 1;
	for (int i = 0; i < 100000000; i++)
	{
		int rnd= mrand();
		sum ^= rnd;
		//printf("rnd=%d\n", rnd);
	}
	printf("sum=%d\n", sum);
	print_time("End");
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t).count();
	return 0;
}
*/