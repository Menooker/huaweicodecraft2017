#include "deploy.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"
#include <chrono>
#include <iostream>
int main(int argc, char *argv[])
{
    print_time("Begin");
    char *topo[MAX_IN_NUM];
    int in_line_num;

    char *topo_file = argv[1];

    in_line_num = read_file(topo, MAX_IN_NUM, topo_file);

    printf("line num is :%d \n", in_line_num);
    if (in_line_num == 0)
    {
        printf("Please input valid topo file.\n");
        return -1;
    }

    char *result_file = argv[2];
	auto t=std::chrono::system_clock::now();
    deploy_server(topo, in_line_num, result_file);

    release_buff(topo, in_line_num);

    print_time("End");
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t).count();
	return 0;
}

