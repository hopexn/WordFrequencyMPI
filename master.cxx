#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "utils.h"
#include <string>
#include <unordered_map>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    FILE *fp;
    MPI_Comm slave_comm;
    MPI_Datatype datatype;
    MPI_Status status;
    char *filename, *filename_result, **slave_argv;
    char command[128];
    int *errcodes;
    int i, j, slave_size;
    struct Entrys entrys;
    unordered_map<string, int> map;
    string key;

    slave_size = atoi(argv[1]);     //子进程数量
    filename = argv[2];             //文件输入
    filename_result = argv[3];      //结果输出

    //分配空间
    slave_argv = (char **) malloc(2 * sizeof(char *));
    errcodes = (int *) malloc(sizeof(int) * slave_size);

    //用于定位子程序
    strcpy(command, "./slave");

    //创建子进程时的参数，将文件名发送给子进程
    slave_argv[0] = filename;
    slave_argv[1] = NULL;

    //初始化MPI
    MPI_Init(&argc, &argv);

    //创建一个MPI数据类型，用来传送Entries结构
    datatype = build_datatype();

    printf("Slave size: %d\n", slave_size);

    MPI_Comm_spawn(command, slave_argv, slave_size, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &slave_comm,
                   errcodes);         //创建子进程

    printf("Spawn completed!\n");
    //接收子进程发送的统计数据，并合并
    for (i = 0; i < slave_size; i++) {
        int tag = 0;
        while (1) {
            MPI_Recv(&entrys, 1, datatype, i, tag++, slave_comm, &status);
            if (entrys.size == -1) break;
            for (j = 0; j < entrys.size; j++) {
                key = string(entrys.keys[j]);
                if (map.find(key) != map.end()) {
                    map[key] += entrys.values[j];
                } else {
                    map[key] = entrys.values[j];
                }
            }

        }
    }

    MPI_Finalize();
    //输出词频统计结果至文件
    fp = fopen(filename_result, "w");
    int count = 0;
    std::unordered_map<string, int>::iterator it;
    for (it = map.begin(); it != map.end(); it++) {
        fprintf(fp, "%s %d\n", it->first.c_str(), it->second);
        count++;
    }
    cout << count << endl;

    //释放资源
    fclose(fp);
    free(slave_argv);
    free(errcodes);
    return 0;
}
