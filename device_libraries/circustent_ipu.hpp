#pragma once

#include <iostream>
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

void printMatrix(std::string matrix_name, std::vector<float> matrix, int cols);

void printMatrixInt(std::string matrix_name, std::vector<float> matrix, int cols);

void frontEnd_TensorDecomp(bool& flag, long unsigned int& rows, long unsigned int& cols, long unsigned int& exp_size, std::vector<float>& cpu_input0, std::vector<float>& cpu_output0, std::vector<float>& cpu_output1);

void backEnd_TensorDecomp(poplar::Engine& engine, bool& flag, long unsigned int& exp_size);

void tensorDecomp();