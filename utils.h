#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

#ifndef __UTILS__
#define __UTILS__

#define DATASIZE 60   //Entrys中的条目数量， 控制slave向master发送信息的数量
#define MAXSTRLEN 64	//允许字符串的最大长度
/**
 * 保存字符串与其对应的计数值
 */
struct Entrys {
    int size;  //条目数量
    char keys[DATASIZE][MAXSTRLEN];  //保存字符串
    int values[DATASIZE];   //对应的计数值
};

MPI_Datatype build_datatype();

long long get_file_size(FILE* fp);

#endif
