CC = g++

all: circustent

circustent: libipu.a
	$(CC) -fopenmp circustent_main.cpp -L/home/ngazda/myFiles/circustent/device_libraries -lipu -lpoplar -lpoplin -lpoputil -lpopops -lpoprand -o circustent
libipu.a: mylib.o
	ar rcs ./device_libraries/libipu.a ./device_libraries/mylib.o
mylib.o:
	popc -o ./device_libraries/io_codelet.gp ./device_libraries/io_codelet.cpp
	popc -o ./device_libraries/io_codelet_strideN.gp ./device_libraries/io_codelet_strideN.cpp
	popc -o ./device_libraries/io_codelet_rand.gp ./device_libraries/io_codelet_rand.cpp
	$(CC) -c -fopenmp ./device_libraries/circustent_ipu.cpp -o ./device_libraries/mylib.o

clean_app:
	rm circustent

clean_lib:
	rm ./device_libraries/mylib.o

clean_logs:
	rm tensor_decomp_test_*

clean:
	rm circustent
	rm ./device_libraries/mylib.o
	rm tensor_decomp_test_*

prun:
	git pull
	rm tensor_decomp_test_*
	sbatch demo.batch
	watch squeue -u ngazda

show:
	cat tensor_decomp_test_*
	cat IPU_INPUTS*
	cat IPU_OUTPUTS*