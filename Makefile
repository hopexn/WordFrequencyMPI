build:
	mpicxx -g -Wall master.cxx utils.cxx -o master
	mpicxx -g -Wall slave.cxx utils.cxx -o slave
clean:
	rm master slave
