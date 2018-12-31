#include "utils.h"

/*
*   创建MPI数据类型, 用于传输Entrys结构
*   struct Entrys {
*       int size;  //条目数量
*       int values[DATASIZE];   //对应的计数值
*       char keys[DATASIZE][MAXSTRLEN];  //保存字符串
*   };
*   可以看到， 传输一个Entrys结构需要DATASIZE+1个int型空间
*   以及DATASIZE * MAXSTRLEN个char型空间
*/
MPI_Datatype build_datatype() {
    //用于保存设置参数
    MPI_Aint offsets[2], extent;
    MPI_Datatype types[2], new_type;
    int sizes[2];

    //获取MPI_INT型需要的空间大小  类似于extent = sizeof(int)
    MPI_Type_extent(MPI_INT, &extent);

    //DATASIZE+1个int型数据
    offsets[0] = 0;
    types[0] = MPI_INT;
    sizes[0] = DATASIZE + 1;

    //DATASIZE * MAXSTRLEN个char型数据
    offsets[1] = (DATASIZE + 1) * extent;
    types[1] = MPI_CHAR;
    sizes[1] = DATASIZE * MAXSTRLEN;

    //根据前面两部分构造一个new_type的MPI类型
    MPI_Type_struct(2, sizes, offsets, types, &new_type);
    //生成new_type类型
    MPI_Type_commit(&new_type);

    return new_type;
}


long long get_file_size(FILE *fp) {
    long long length;
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    return length;
}
