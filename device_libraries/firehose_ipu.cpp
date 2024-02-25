#include "firehose_ipu.hpp"

enum Progs {
    STREAM_INPUTS,
    ALIGN_INPUTS,
    CONSUMPTION_TASK,
    ALIGN_OUTPUTS,
    STREAM_OUTPUTS,
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

void frontEnd_TensorDecomp(bool& flag, long unsigned int& rows, long unsigned int& cols, long unsigned int& exp_size, std::vector<float>& cpu_input0, std::vector<float>& cpu_output0, std::vector<float>& cpu_output1) {
    /* Create data to input into back-end */
    //std::random_device rd;
    //std::mt19937 gen(rd());
    //std::uniform_real_distribution<float> distribution(0.0f, 100.0f);

    //for (int i = 0; i < rows; i++) {
        //for (int j = 0; j < cols; j++) {
            //cpu_input0[j+(cols*i)] = distribution(gen);
        //}
    //}
    //printMatrix("GenMatrix", cpu_input0, cols);

    //flag = true;
    /* Loop to create multiple matrices and decompose */
    //for (int i = 0; i < exp_size; i++) {
        
        //for (int i = 0; i < rows; i++) {
            //for (int j = 0; j < cols; j++) {
                //cpu_input0[j+(cols*i)] = distribution(gen);
            //}
        //}

        //while(flag) {}
        //printMatrix("QMatrix", cpu_output0, cols);
        //printMatrix("RMatrix", cpu_output1, cols);
        //sleep(1);
    //}
}

void backEnd_TensorDecomp(poplar::Engine& engine, bool& flag, long unsigned int& exp_size) {
    //for (int i = 0; i < exp_size; i++) {
        //while(!flag) {}
        //flag = false;
        //engine.run(Progs::STREAM_INPUTS);
        //engine.run(Progs::ALIGN_INPUTS);
        //engine.run(Progs::CONSUMPTION_TASK);
        //engine.run(Progs::ALIGN_OUTPUTS);
        //engine.run(Progs::STREAM_OUTPUTS);
    //}
}

void tensorDecomp() {
    std::cout << "START" << std::endl;
    // Get an IPU Device
    auto manager = poplar::DeviceManager::createDeviceManager();
    auto hwDevices = manager.getDevices(poplar::TargetType::IPU, 1);
    auto it = std::find_if(hwDevices.begin(), hwDevices.end(), [](poplar::Device &device) { return device.attach(); });
    poplar::Device device;

    if (it != hwDevices.end()) {
        device = std::move(*it);
    }

    /* Expose Shared Memory */

    // Graph
    poplar::Graph graph(device.getTarget());
    std::cout << "CHECK1" << std::endl;

    // Programs
    std::vector<poplar::program::Program> progs(Progs::NUM_PROGRAMS);
    std::cout << "CHECK2" << std::endl;

    // Flags
    bool flag = false;

    // Parameters
    long unsigned int rows = 3;
    long unsigned int cols = 3;
    long unsigned int packet_size = 9;
    long unsigned int num_transfers = (rows*cols) /packet_size;
    long unsigned int exp_size = 1;
    std::cout << "CHECK3" << std::endl;

    // Tensors
    auto input_tensor0 = graph.addVariable(poplar::FLOAT, {packet_size}, "Input Tensor 0");
    auto output_tensor0 = graph.addVariable(poplar::FLOAT, {packet_size}, "Output Tensor 0");
    // setting up random indices for rand
    // reference tensor
    poplar::Tensor randomIndices = graph.addVariable(poplar::INT, {packet_size}, "randomIndices");

    poputil::mapTensorLinearly(graph, input_tensor0);
    poputil::mapTensorLinearly(graph, output_tensor0);
    poputil::mapTensorLinearly(graph, randomIndices);

    std::cout << "CHECK4" << std::endl;

    // Add standard codelets
    popops::addCodelets(graph);
    poprand::addCodelets(graph);

    // Add custom codelets
    // ******* UNCOMMENT THE FOLLOWING LINES TO TEST THE OTHER CODELETS *******
    graph.addCodelets("./device_libraries/io_codelet.gp");
    // graph.addCodelets("./device_libraries/io_codelet_strideN.gp");
    // graph.addCodelets("./device_libraries/io_codelet_rand.gp");

    // Vertices
    auto consumption_task_cs = graph.addComputeSet("Consumption Task CS");
    auto input_io0 = graph.addVertex(consumption_task_cs, "IOVertex");
    auto output_io0 = graph.addVertex(consumption_task_cs, "IOVertex");

    graph.setTileMapping(input_io0, 3);
    graph.setTileMapping(output_io0, 4);

    // Streams
    auto input_strm0 = graph.addHostToDeviceFIFO("Input Stream 0", poplar::FLOAT, packet_size);
    auto output_strm0 = graph.addDeviceToHostFIFO("Output Stream 0", poplar::FLOAT, packet_size);
    auto rand_strm0 = graph.addDeviceToHostFIFO("Rand Stream 0", poplar::FLOAT, packet_size);

    // ********** REQUIRED FOR STRIDEN CODELET **********
    // set to N=2 for now
    //auto N  = graph.addVariable(poplar::INT, {1}, "N");
    //auto c1 = graph.addConstant<int>(INT, {1}, {2});

    // poputil::mapTensorLinearly(graph, N);
    // poputil::mapTensorLinearly(graph, c1);

    // CPU Vectors
    std::vector<float> cpu_input0(rows*cols);
    std::vector<float> cpu_output0(rows*cols);

    // return;

    /* Stream Inputs Program */

    auto seq = poplar::program::Sequence();

    // // step to initialize N with the constant value in c1
    // seq.add(poplar:program::Copy(c1, N));

    seq.add(poplar::program::Copy(input_strm0, input_tensor0));

    // adding random indices to the graph
    randomIndices = poprand::uniform(graph, NULL, 0, randomIndices, poplar::INT, 0, packet_size-1, seq);
    std::cout << "randomIndices: "<< randomIndices << std::endl;

    seq.add(poplar::program::Copy(rand_strm0, randomIndices));

    progs[Progs::STREAM_INPUTS] = seq;

    graph.connect(input_io0["strm_in"], input_tensor0);
    graph.connect(input_io0["randomIndices"], randomIndices);
    // graph.connect(input_io0["N"], N);
    graph.connect(input_io0["strm_out"], output_tensor0);

    /* Compiling Graph */
    auto exe = poplar::compileGraph(graph, progs);
    poplar::Engine engine(std::move(exe));
    engine.load(device);

    std::cout << "HERE" << std::endl;

    // return 0;

    // #pragma omp parallel sections
    // {
    //     #pragma omp section
    //     {
    //         frontEnd_TensorDecomp(flag, rows, cols, exp_size, cpu_input0, cpu_output0, cpu_output1);
    //     }

    //     #pragma omp section
    //     {
    //         backEnd_TensorDecomp(engine, flag, exp_size);
    //     }
    // }
}