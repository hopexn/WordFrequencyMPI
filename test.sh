#!/bin/bash
make
mpirun -np 1 ./master $1 ./testfile/testdata11 ./testfile/out/$1.txt
rm -rf master slave master.dSYM slave.dSYM
