#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <string>
#include "mpi.h"
#include "utils.h"

#include <iostream>

using namespace std;


/**
 * 将大写字母转成小写
 * @param str
 * @return
 */
char *strtolower(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 'a' - 'A';
        }
    }
    return str;
}

/**
 * 根据分隔符分割单词，转成小写形式后插入hashmap中
 * @param buffer
 * @param hashmap
 * @param bufsize
 */
void process_buffer(char *buffer, unordered_map<string, int> &map) {
    //char delimiters[] = " ,!.1234567890?/*():;\t\n{}<>#=&$%^+\"";  //设定分隔符
    char delimiters[] = "\n";  //设定分隔符
    char *word;
    string word_lower;
    word = strtok(buffer, delimiters);  //使用分隔符分割字符串获取单词
    while (word != NULL) {
        word_lower = string(strtolower(word));
        if (map.find(word_lower) != map.end()) {
            map[word_lower] += 1;
        } else {
            map[word_lower] = 1;
        }
        word = strtok(NULL, delimiters);
    }
}

int main(int argc, char **argv) {
    int my_rank, comm_size;
    MPI_File file;
    long long bufsize, offset, filesize;
    char *buffer, *buffer_begin, *buffer_end;
    MPI_Datatype datatype;
    MPI_Status status;
    struct Entrys entrys;
    MPI_Comm master_comm;

    //新建一个hashmap
    unordered_map<string, int> map;

    //初始化 MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_get_parent(&master_comm);
    printf("Slave %d start~\n", my_rank);
    //创建一个Datatype，用于传输Entrys结构
    datatype = build_datatype();


    //子进程打开文件
    MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
    MPI_File_get_size(file, &filesize);
    if (my_rank == 0) {
        printf("Filesize: %lld\n", filesize);
    }

    //计算每个节点读取文件的大小
    if (filesize % comm_size == 0) {
        bufsize = filesize / comm_size;
    } else {
        bufsize = filesize / comm_size + 1;
    }

    offset = my_rank * bufsize;
    /**
     *  调整读取位置，防止文件分割导致字符串断开
     */
    if (my_rank == comm_size - 1) {  //最后一部分
        bufsize = filesize - offset;
        buffer = (char *) malloc(sizeof(char) * (bufsize));
        MPI_File_read_at(file, offset, buffer, bufsize, MPI_CHAR, &status);
        buffer_begin = buffer;
        while (*buffer_begin != '\n') buffer_begin++;
        buffer_begin++;
    } else {    //前面部分
        buffer = (char *) malloc(sizeof(char) * (bufsize + MAXSTRLEN));
        MPI_File_read_at(file, offset, buffer, bufsize + MAXSTRLEN, MPI_CHAR, &status);
        buffer_begin = buffer;
        if (my_rank != 0) {
        }
        buffer_begin++;
        buffer_end = buffer + bufsize + 1;
        while (*buffer_end != '\n' && buffer_end - buffer < bufsize + MAXSTRLEN) buffer_end++;
        *buffer_end = '\0';
    }

    //关闭文件
    MPI_File_close(&file);


    //根据分隔符分割单词，转成小写形式后插入hashmap中
    process_buffer(buffer_begin, map);

    /**
     * 将Hashmap中的数据放入Entries结构，进行传输
     */
    entrys.size = 0;
    std::unordered_map<string, int>::iterator it;
    int tag = 0;
    for (it = map.begin(); it != map.end(); it++) {
        if (entrys.size == DATASIZE) {   //当Entrys结构满后开启发送
            MPI_Send(&entrys, 1, datatype, 0, tag++, master_comm);
            entrys.size = 0;
        }
        entrys.values[entrys.size] = it->second;
        strcpy(entrys.keys[entrys.size], it->first.c_str());
        entrys.size += 1;
    }

    //将余下的Entrys发送
    if (entrys.size > 0) {
        MPI_Send(&entrys, 1, datatype, 0, tag++, master_comm);
    }
    //将size置为-1， 作为信号发送给主进程， 表示流程结束
    entrys.size = -1;
    MPI_Send(&entrys, 1, datatype, 0, tag++, master_comm);

    MPI_Finalize();
    return 0;
}
