#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <unistd.h>
#include <omp.h>

#include <poplar/DeviceManager.hpp>
#include <poplar/Engine.hpp>
#include <poplar/Graph.hpp>
#include <poplar/IPUModel.hpp>
#include <poplar/Tensor.hpp>
#include <poprand/RandomGen.hpp>

#include <popops/codelets.hpp>

#include <poputil/TileMapping.hpp>
#include <poplar/Engine.hpp>

#include <poplin/codelets.hpp>
#include <poplin/experimental/QRFactorization.hpp>

#include <poprand/codelets.hpp>
#include <poprand/RandomGen.hpp>

void printMatrix(std::string matrix_name, std::vector<float> matrix, int cols, int id, int packet, int io);

void printMatrixInt(std::string matrix_name, std::vector<int> matrix, int cols, int id, int packet, int io);

void tensorDecomp(long unsigned int row, long unsigned int col, long unsigned int num_packets, long unsigned int num_streams, long unsigned int num_devices, bool get_from_file);