#include "device_libraries/circustent_ipu.hpp"

int main() {
    long unsigned int row = 3;
    long unsigned int col = 3;
    long unsigned int num_packets = 1;
    long unsigned int num_streams = 1;
    long unsigned int num_devices = 1;
    bool get_from_file = false;

    tensorDecomp(row, col, num_packets, num_streams, num_devices, get_from_file);

    return 0;
}