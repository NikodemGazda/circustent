#include "circustent_ipu.hpp"

#define num_programs 3  

#define MODERUN RAND

enum mode {
    STRIDEN,
    STRIDE,
    RAND
};

void printMatrix(std::string matrix_name, std::vector<float> matrix, int cols, int id, int packet, int io) {

    std::string fileName;
  switch(io) {
    case 0:
        fileName = "IPU_INPUTS" + std::to_string(id) + ".out";
        break;
    default:
        fileName = "IPU_OUTPUTS" + std::to_string(id) + ".out";
        break;
  }
  std::ofstream fileStream(fileName, std::ios::app);
  fileStream << matrix_name << " THREAD " << id << " PACKET " << packet << std::endl;
  //std::cout << matrix_name << " THREAD " << id << " PACKET " << packet << std::endl;

  for (int i = 0; i < matrix.size(); i++) {

    fileStream << std::fixed << matrix[i] << "\t";
    //std::cout << std::fixed << matrix[i] << "\t";
    
    if ( (i+1)%cols == 0) {
      fileStream << std::endl;
      //std::cout << std::endl;
    }

  }

  fileStream << std::endl;
  fileStream.close();
  //std::cout << std::endl;

}

void printMatrixInt(std::string matrix_name, std::vector<int> matrix, int cols, int id, int packet, int io) {

    std::string fileName;
  switch(io) {
    case 0:
        fileName = "IPU_INPUTS" + std::to_string(id) + ".out";
        break;
    default:
        fileName = "IPU_OUTPUTS" + std::to_string(id) + ".out";
        break;
  }
  std::ofstream fileStream(fileName, std::ios::app);
  fileStream << matrix_name << " THREAD " << id << " PACKET " << packet << std::endl;
  //std::cout << matrix_name << " THREAD " << id << " PACKET " << packet << std::endl;

  for (int i = 0; i < matrix.size(); i++) {

    fileStream << std::fixed << matrix[i] << "\t";
    //std::cout << std::fixed << matrix[i] << "\t";
    
    if ( (i+1)%cols == 0) {
      fileStream << std::endl;
      //std::cout << std::endl;
    }

  }

  fileStream << std::endl;
  fileStream.close();
  //std::cout << std::endl;

}

void tensorDecomp(long unsigned int row, long unsigned int col, long unsigned int num_packets, long unsigned int num_streams, long unsigned int num_devices, bool get_from_file) {

    /* Get an IPU Device */

    std::cout << "Getting Device..." << std::endl;

    auto manager = poplar::DeviceManager::createDeviceManager();
    auto hwDevices = manager.getDevices(poplar::TargetType::IPU, num_devices);
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
    std::vector<poplar::program::Program> progs(num_streams*num_programs);

    // Flags
    bool data_ready_flags[num_streams];

    for (int i = 0; i < num_streams; i++) {
        data_ready_flags[i] = false;
    }

    // Variable Tensors
    std::cout << "Adding Tensors..." << std::endl;

    std::vector<poplar::Tensor> v_io_in0(num_streams);
    std::vector<poplar::Tensor> v_io_out0(num_streams);

    //**** STRIDE N ****//
    auto v_con_N_input = graph.addVariable(poplar::INT, {1}, "v_con_N_input");
    auto c_con_N_input = graph.addConstant<int>(poplar::INT, {1}, {2});

    //**** RAND ****//
    auto v_con_randomIndices = graph.addVariable(poplar::INT, {row*col}, "v_con_randomIndices");
    auto v_con_randomIndices_in = graph.addVariable(poplar::INT, {row*col}, "v_con_randomIndices_in");
    auto c_con_rand_seed = graph.addConstant<int>(poplar::UNSIGNED_INT, {2}, {10, 7}); // create seed here

    // mapping tensors/constants
    //**** STRIDE N ****//
    poputil::mapTensorLinearly(graph, v_con_N_input);
    poputil::mapTensorLinearly(graph, c_con_N_input);
    //**** RAND ****//
    poputil::mapTensorLinearly(graph, v_con_randomIndices);
    poputil::mapTensorLinearly(graph, v_con_randomIndices_in);
    poputil::mapTensorLinearly(graph, c_con_rand_seed);

    // std::vector<poplar::Tensor> v_con1(num_streams);
    // std::vector<poplar::Tensor> v_io_out1(num_streams);

    std::string db_name;

    for (int i = 0; i < num_streams; i++) {

        db_name = "Input Tensor " + std::to_string(i) + " of Set 0";
        v_io_in0[i] = graph.addVariable(poplar::FLOAT, {row, col}, db_name);
        poputil::mapTensorLinearly(graph, v_io_in0[i]);

        db_name = "Output Tensor " + std::to_string(i) + " of Set 0";
        v_io_out0[i] = graph.addVariable(poplar::FLOAT, {row, col}, db_name);
        poputil::mapTensorLinearly(graph, v_io_out0[i]);

    }

    std::cout << "Added Tensors!" << std::endl;

    // Add standard codelets
    std::cout << "Adding Codelets..." << std::endl;

    popops::addCodelets(graph);

    if (MODERUN == STRIDE) {
      graph.addCodelets("./device_libraries/io_codelet.gp");
    }

    if (MODERUN == STRIDEN) {
      graph.addCodelets("./device_libraries/io_codelet_strideN.gp");
    }

    poprand::addCodelets(graph);
    if (MODERUN == RAND) {
      graph.addCodelets("./device_libraries/io_codelet_rand.gp");
    }

    std::cout << "Added Codelets!" << std::endl;

    // Vertices
    std::cout << "Adding Vertices..." << std::endl;

    std::vector<poplar::ComputeSet> cps_con(num_streams);

    for (int i = 0; i < num_streams; i++) {
        db_name = "Con in CS " + std::to_string(i);
        cps_con[i] = graph.addComputeSet(db_name);
    }

    std::vector<poplar::VertexRef> vtx_con0(num_streams);

    for (int i = 0; i < num_streams; i++) {

        vtx_con0[i] = graph.addVertex(cps_con[i], "IOVertex");
        graph.setTileMapping(vtx_con0[i], i+5);

    }

    for(int i = 0; i < num_streams; i++) {

        
        graph.connect(vtx_con0[i]["strm_in"], v_io_in0[i]);
        graph.connect(vtx_con0[i]["strm_out"], v_io_out0[i]);
        // adding extra inputs for their respective algs
        if (MODERUN == STRIDEN) {
            graph.connect(vtx_con0[i]["N"], v_con_N_input);
        } else if (MODERUN == RAND) {
            graph.connect(vtx_con0[i]["randomIndices"], v_con_randomIndices_in);
        }
    }

    std::cout << "Added Vertices!" << std::endl;

    // Streams
    std::cout << "Adding Streams..." << std::endl;

    std::vector<poplar::DataStream> strm_in0(num_streams);
    std::vector<poplar::DataStream> strm_out0(num_streams);

    for (int i = 0; i < num_streams; i++) {
        db_name = "Input Stream " + std::to_string(i) + " for input 0";
        strm_in0[i] = graph.addHostToDeviceFIFO(db_name, poplar::FLOAT, row*col);

        db_name = "Output Stream " + std::to_string(i) + " for output 0";
        strm_out0[i] = graph.addDeviceToHostFIFO(db_name, poplar::FLOAT, row*col);
    }

    std::cout << "Added Streams!" << std::endl;

    // CPU Vectors
    std::vector<std::vector<float>> cpu_in0(num_streams, std::vector<float> (row*col, 1.0));
    std::vector<std::vector<float>> cpu_out0(num_streams, std::vector<float> (row*col, 1.0));

    std::cout << "Adding Programs..." << std::endl;

    /* Stream Inputs Programs */

    int prog_idx = 0;

    auto seq = poplar::program::Sequence();

    // stream the constants in
    //**** STRIDE N ****//
    if (MODERUN == STRIDEN) {
        seq.add(poplar::program::Copy(c_con_N_input, v_con_N_input));
        db_name = "v_con_N_input";
        seq.add(poplar::program::PrintTensor(db_name, v_con_N_input));
    }
    //**** RAND ****//
    v_con_randomIndices = poprand::uniform(graph, &c_con_rand_seed, 0, v_con_randomIndices, poplar::INT, 0, row*col-1, seq);
    
    if (MODERUN == RAND) {
        seq.add(poplar::program::Copy(v_con_randomIndices, v_con_randomIndices_in));
        db_name = "v_con_randomIndices_in";
        seq.add(poplar::program::PrintTensor(db_name, v_con_randomIndices_in));
    }

    // stream the inputs in
    for(int i = 0; i < num_streams; i++) {

        seq.add(poplar::program::Copy(strm_in0[i], v_io_in0[i]));

        db_name = "v_io_in[" + std::to_string(i) + "]";
        seq.add(poplar::program::PrintTensor(db_name, v_io_in0[i]));

        progs[prog_idx++] = seq;

        seq = poplar::program::Sequence();
    }

    /* Consumption Task Programs */


    for(int i = 0; i < num_streams; i++) {

        seq = poplar::program::Sequence();

        seq.add(poplar::program::Execute(cps_con[i]));

        progs[prog_idx++] = seq;
    }

    /* Stream Outputs Programs */

    for(int i = 0; i < num_streams; i++) {

        seq = poplar::program::Sequence();

        seq.add(poplar::program::Copy(v_io_out0[i], strm_out0[i]));
        db_name = "out[" + std::to_string(i) + "]";
        seq.add(poplar::program::PrintTensor(db_name, v_io_out0[i]));

        progs[prog_idx++] = seq;
    }

    std::cout << "Added Programs!" << std::endl;

    /* Create and Load Engine */

    std::cout << "Loading Device..." << std::endl;

    auto exe = poplar::compileGraph(graph, progs);
    poplar::Engine engine(std::move(exe));
    engine.load(device);

    std::cout << "Loaded Device!" << std::endl;

    /* Connect Streams */

    std::cout << "Connecting Streams..." << std::endl;

    for (int i = 0; i < num_streams; i++) {
        db_name = "Input Stream " + std::to_string(i) + " for input 0";
        engine.connectStream(db_name, cpu_in0[i].data(), cpu_in0[i].data() + cpu_in0[i].size());

        db_name = "Output Stream " + std::to_string(i) + " for output 0";
        engine.connectStream(db_name, cpu_out0[i].data(), cpu_out0[i].data() + cpu_out0[i].size());

        // db_name = "Output Stream " + std::to_string(i) + " for output 1";
        // engine.connectStream(db_name, cpu_out1[i].data(), cpu_out1[i].data() + cpu_out1[i].size());
    }

    std::cout << "Connected Streams!" << std::endl << std::endl;

    /* Run Parallel Threads for FireHose */

    omp_set_num_threads(num_streams*2);

    //if(get_from_file) {
        //auto source = distribution;
    //}


    //}

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int gbl_id = (int) thread_id;
        int snd_id = (int) thread_id;
        int rcv_id = thread_id-num_streams;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distribution(0.0f, 100.0f);

        // std::string fileName = "/home/ngazda/myFiles/circustent/input" + std::to_string(snd_id) + ".mtx";

        // std::ifstream file(fileName);
        // std::string line;


        if(gbl_id < num_streams) {
            for (int a = 0; a < num_packets; a++) {
                while(data_ready_flags[snd_id]);

                // if (!get_from_file) {
                //     for (int i = 0; i < row*col; i++) {
                //         cpu_in0[snd_id][i] = distribution(gen);
                //     }
                // }
                // else {
                //     std::getline(file, line);
                //     std::istringstream iss(line);
                //     float value;
        
                //     // Split the line into floats
                //     int i = 0;
                //     while (iss >> value) {
                //         cpu_in0[snd_id][i++] = value;
                //     }
                // }

                #pragma omp critical(print)
                    printMatrix("GenMatrix", cpu_in0[snd_id], col, snd_id, a, 0);

                data_ready_flags[snd_id] = true;

            }
        }
        else {

            for (int a = 0; a < num_packets; a++) {

                while(!data_ready_flags[rcv_id]) {}

                #pragma omp critical(ipu_work)
                {
                    engine.run(rcv_id);
                    engine.run(num_streams+rcv_id);
                    engine.run((num_streams*2)+rcv_id);
                }

                #pragma omp critical(print)
                {
                    printMatrix("QMatrix", cpu_out0[rcv_id], col, rcv_id, a, 1);
                    // printMatrix("RMatrix", cpu_out1[rcv_id], col, rcv_id, a, 1);
                }

                data_ready_flags[rcv_id] = false;
            }
        }

        // file.close();
    }

    return;
}

//void placeholder(long unsigned int row, long unsigned int col, long unsigned int num_streams, long unsigned int num_devices) {

//}