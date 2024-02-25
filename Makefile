CC = g++

all: firehose

firehose: libipu.a
	$(CC) -fopenmp firehose_main.cpp -L/home/ngazda/myFiles/circustent/device_libraries -lipu -lpoplar -lpoplin -lpoputil -lpopops -lpoprand -o firehose
libipu.a: mylib.o
	ar rcs ./device_libraries/libipu.a ./device_libraries/mylib.o
mylib.o:
#   popc -o ./device_libraries/io_codelet.gp ./device_libraries/io_codelet.cpp
#   popc -o ./device_libraries/io_codelet.gp ./device_libraries/io_codelet_strideN.cpp
  popc -o ./device_libraries/io_codelet.gp ./device_libraries/io_codelet_rand.cpp
	$(CC) -c -fopenmp ./device_libraries/firehose_ipu.cpp -o ./device_libraries/mylib.o

clean_app:
	rm firehose

clean_lib:
	rm ./device_libraries/mylib.o

clean_logs:
	rm tensor_decomp_test_*

clean:
	rm firehose
	rm ./device_libraries/mylib.o
	rm tensor_decomp_test_*