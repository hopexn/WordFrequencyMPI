### 一、 使用方式：

##### 方式一：
mpirun -np 1 ./master <slave number> <input file name> <output file name>
1. 打开Terminal，进入项目根目录
2. 输入make命令，编译程序，生成master slave两个程序
3. 输入mpirun -np 1 ./master <slave number> ./testfile/in.txt ./testfile/out/out.txt运行程序， 其中<slave number>为大于0的整数， 表示子进程的个数
4. 词频统计信息会存放在./testfile/out/out.txt文件中

##### 方式二：
1. 打开Terminal，进入项目根目录，输入bash ./test.sh 10
2. 程序会自动编译，生成并测试程序
3. 测试程序会运行程序10次， 每一次运行设为<i>个子进程， 结果存放在./testfile/out/<i>.txt中

### 二、文件结构
#### 1. master.c
主程序，创建子程序以及合并子程序的结果

#### 2. slave.c
子程序，统计词频，结束后将结果发送给父程序

#### 3. hashmap.h hashmap.c
哈希表的结构与操作函数

#### 4. utils.h utils.c
存放一些常用的参数、数据结构、函数

#### 5. test.sh
测试脚本
