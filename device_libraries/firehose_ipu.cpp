#include "firehose_ipu.hpp"
#include <poprand/codelets.hpp>
#include <poprand/RandomGen.hpp>

enum Progs {
    STREAM_INPUTS,
    // ALIGN_INPUTS,
    // CONSUMPTION_TASK,
    // ALIGN_OUTPUTS,
    // STREAM_OUTPUTS,
    NUM_PROGRAMS
};

void printMatrix(std::string matrix_name, std::vector<float> matrix, int cols) {
  std::cout << matrix_name << std::endl;

  for (int i = 0; i < matrix.size(); i++) {

    std::cout << std::fixed << matrix[i] << "\t";
    
    if ( (i+1)%cols == 0) {
      std::cout << std::endl;
    }

  }

  std::cout << std::endl;

}

void printMatrixInt(std::string matrix_name, std::vector<int> matrix, int cols) {
  std::cout << matrix_name << std::endl;

  for (int i = 0; i < matrix.size(); i++) {

    std::cout << std::fixed << matrix[i] << "\t";
    
    if ( (i+1)%cols == 0) {
      std::cout << std::endl;
    }

  }

  std::cout << std::endl;

}

// void frontEnd_TensorDecomp(bool& flag, long unsigned int& rows, long unsigned int& cols, long unsigned int& exp_size, std::vector<float>& cpu_input0, std::vector<float>& cpu_output0, std::vector<float>& cpu_output1) {
//     /* Create data to input into back-end */
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<float> distribution(0.0f, 100.0f);

//     for (int i = 0; i < rows; i++) {
//         for (int j = 0; j < cols; j++) {
//             cpu_input0[j+(cols*i)] = distribution(gen);
//         }
//     }
//     printMatrix("GenMatrix", cpu_input0, cols);

//     flag = true;
//     /* Loop to create multiple matrices and decompose */
//     for (int i = 0; i < exp_size; i++) {
        
//         for (int i = 0; i < rows; i++) {
//             for (int j = 0; j < cols; j++) {
//                 cpu_input0[j+(cols*i)] = distribution(gen);
//             }
//         }

//         while(flag) {}
//         printMatrix("QMatrix", cpu_output0, cols);
//         printMatrix("RMatrix", cpu_output1, cols);
//         sleep(1);
//     }
// }

// void backEnd_TensorDecomp(poplar::Engine& engine, bool& flag, long unsigned int& exp_size) {
//     for (int i = 0; i < exp_size; i++) {
//         while(!flag) {}
//         flag = false;
//         engine.run(Progs::STREAM_INPUTS);
//         engine.run(Progs::ALIGN_INPUTS);
//         engine.run(Progs::CONSUMPTION_TASK);
//         engine.run(Progs::ALIGN_OUTPUTS);
//         engine.run(Progs::STREAM_OUTPUTS);
//     }
// }

void tensorDecomp() {
    std::cout << "Getting Device..." << std::endl;
    // Get an IPU Device
    auto manager = poplar::DeviceManager::createDeviceManager();
    auto hwDevices = manager.getDevices(poplar::TargetType::IPU, 1);
    auto it = std::find_if(hwDevices.begin(), hwDevices.end(), [](poplar::Device &device) { return device.attach(); });
    poplar::Device device;

    if (it != hwDevices.end()) {
        device = std::move(*it);
    }

    std::cout << "Got Device!" << std::endl;

    /* Expose Shared Memory */

    // Graph
    std::cout << "Creating Graph..." << std::endl;
    poplar::Graph graph(device.getTarget());
    std::cout << "Created Graph!" << std::endl;

    // Programs
    std::vector<poplar::program::Program> progs(Progs::NUM_PROGRAMS);

    // Flags
    bool data_ready_flag = false;

    // Parameters
    long unsigned int rows = 3;
    long unsigned int cols = 3;
    long unsigned int packet_size = 9;
    long unsigned int num_transfers = (rows*cols) /packet_size;
    long unsigned int exp_size = 1;

    // Tensors
    std::cout << "Adding Tensors..." << std::endl;
    auto input_tensor0 = graph.addVariable(poplar::FLOAT, {packet_size}, "Input Tensor 0");
    auto output_tensor0 = graph.addVariable(poplar::FLOAT, {packet_size}, "Output Tensor 0");

    /***** UNCOMMENT FOR STRIDEN *****/
    // auto N_input = graph.addVariable(poplar::INT, {1}, "N Input");

    /***** UNCOMMENT FOR RAND *****/
    auto randomIndices = graph.addVariable(poplar::INT, {packet_size}, "randomIndices");

    // constants
    /***** UNCOMMENT FOR STRIDEN *****/
    // auto c1 = graph.addConstant<int>(poplar::INT, {1}, {2});

    /***** UNCOMMENT FOR RAND *****/
    auto c2 = graph.addConstant<int>(poplar::INT, {packet_size}, {2, 6, 1, 1, 3, 5, 4, 3, 4});

    std::cout << "Added Tensors!" << std::endl;

    std::cout << "Mapping Tensors..." << std::endl;
    poputil::mapTensorLinearly(graph, input_tensor0);
    poputil::mapTensorLinearly(graph, output_tensor0);

    /***** UNCOMMENT FOR STRIDEN *****/
    // poputil::mapTensorLinearly(graph, N_input);
    // poputil::mapTensorLinearly(graph, c1);

    /***** UNCOMMENT FOR RAND *****/
    poputil::mapTensorLinearly(graph, randomIndices);
    poputil::mapTensorLinearly(graph, c2);

    std::cout << "Mapped Tensors!" << std::endl;

    std::cout << "Adding Codelets..." << std::endl;
    // Add standard codelets
    popops::addCodelets(graph);

    // Add custom codelets
    // graph.addCodelets("./device_libraries/io_codelet.gp");

    /***** UNCOMMENT FOR STRIDEN *****/
    // graph.addCodelets("./device_libraries/io_codelet_strideN.gp");

    /***** UNCOMMENT FOR RAND *****/
    poprand::addCodelets(graph);
    graph.addCodelets("./device_libraries/io_codelet_rand.gp");

    std::cout << "Added Codelets!" << std::endl;

    std::cout << "Adding Vertices..." << std::endl;
    // Vertices
    //auto consumption_task_cs = graph.addComputeSet("Consumption Task CS");
    auto io_in = graph.addComputeSet("IO in CS");
    auto io_out = graph.addComputeSet("IO out CS");
    auto input_io0 = graph.addVertex(io_in, "IOVertex");
    auto output_io0 = graph.addVertex(io_out, "IOVertex");

    std::cout << "Added Vertices!" << std::endl;

    std::cout << "Mapping Vertices..." << std::endl;
    graph.setTileMapping(input_io0, 3);
    graph.setTileMapping(output_io0, 4);

    std::cout << "Mapped Vertices!" << std::endl;

    std::cout << "Adding Streams..." << std::endl;
    // Streams
    auto input_strm0 = graph.addHostToDeviceFIFO("Input Stream 0", input_tensor0.elementType(), input_tensor0.numElements());
    auto output_strm0 = graph.addDeviceToHostFIFO("Output Stream 0", poplar::FLOAT, packet_size);
    auto random_strm0 = graph.addDeviceToHostFIFO("Random Stream 0", poplar::INT, packet_size);

    std::cout << "Added Streams!" << std::endl;

    // Misc
    //auto ready_flag = graph.addVariable(poplar::INT, {1}, "Ready Flag");
    //auto num_elements = graph.addVariable(poplar::INT, {1}, "Number of elements");

    //poputil::mapTensorLinearly(graph, ready_flag);
    //poputil::mapTensorLinearly(graph, num_elements);

    // CPU Vectors
    std::vector<float> cpu_input0(rows*cols);
    std::vector<float> cpu_output0(rows*cols);
    std::vector<int> cpu_output_rand(packet_size);

    /* Stream Inputs Program */

    auto seq = poplar::program::Sequence();

    for(int i = 0; i < num_transfers; i++) {
        seq.add(poplar::program::Copy(input_strm0, input_tensor0));
    }

    // copying N to the input tensor
    /***** UNCOMMENT FOR STRIDEN *****/
    // seq.add(poplar::program::Copy(c1, N_input));

    /***** UNCOMMENT FOR RAND *****/
    // seq.add(poplar::program::Copy(c2, randomIndices));
    randomIndices = poprand::uniform(graph, nullptr, 0, randomIndices, poplar::INT, 0, packet_size-1, seq);

    graph.connect(input_io0["strm_in"], input_tensor0);
    graph.connect(input_io0["strm_out"], output_tensor0);
    graph.connect(output_io0["strm_in"], input_tensor0);
    graph.connect(output_io0["strm_out"], output_tensor0);

    /***** UNCOMMENT FOR STRIDEN *****/
    // graph.connect(input_io0["N"], N_input);
    // graph.connect(output_io0["N"], N_input);

    /***** UNCOMMENT FOR RAND *****/
    graph.connect(input_io0["randomIndices"], randomIndices);
    graph.connect(output_io0["randomIndices"], randomIndices);
    
    seq.add(poplar::program::Execute(io_in));



    for(int i = 0; i < num_transfers; i++) {
        seq.add(poplar::program::Copy(output_tensor0, output_strm0));
    }

    seq.add(poplar::program::Copy(randomIndices, random_strm0));

    progs[Progs::STREAM_INPUTS] = seq;

    auto exe = poplar::compileGraph(graph, progs);
    poplar::Engine engine(std::move(exe));
    engine.load(device);

    /* Connect Streams */

    engine.connectStream("Input Stream 0", cpu_input0.data(), cpu_input0.data() + cpu_input0.size());
    engine.connectStream("Output Stream 0", cpu_output0.data(), cpu_output0.data() + cpu_output0.size());
    engine.connectStream("Random Stream 0", cpu_output_rand.data(), cpu_output_rand.data() + cpu_output_rand.size());

    std::cout << "Loaded Device" << std::endl;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            while(data_ready_flag) {}

            /* Create data to input into back-end */
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distribution(0.0f, 100.0f);

            /* Loop to create multiple matrices and decompose */
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        cpu_input0[j+(cols*i)] = 1;
                        // cpu_input0[j+(cols*i)] = distribution(gen);
                    }
                }
                printMatrix("Input Matrix", cpu_input0, cols);
                data_ready_flag = true;
        }

        #pragma omp section
        {
            while(!data_ready_flag) {}
            data_ready_flag = false;
            engine.run(Progs::STREAM_INPUTS);


            printMatrix("Output Matrix", cpu_output0, cols);

            // reading random indices
            printMatrixInt("Random Indices", cpu_output_rand, 9);
        }
    }
    return;
}