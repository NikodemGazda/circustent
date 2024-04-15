#include <poplar/Vertex.hpp>
#include <cstdlib>


using namespace poplar;

class IOVertex : public Vertex {
public:

// Input<Vector<float>> strm_in;
// Input<Vector<int>> randomIndices;
// Output<Vector<float>> strm_out;

Vector<Input<Vector<float>>> strm_in;
Vector<Input<Vector<int>>> randomIndices;
Vector<Output<Vector<float>>> strm_out;

// function:
// rand
// 
// description:
// takes in a vector of floats and adds 1 to elements at random indices.

// Compute function
    bool compute() {
        //if (ready_flag[0]) {
            // srand(2048);
            // int random_value;
            for (unsigned i = 0; i < strm_in.size(); i++) {
                // random_value = rand() % strm_in.size();
                // strm_out[random_value] = strm_in[random_value]+1;
                strm_out[randomIndices[i]/strm_in.size()][randomIndices[i]%strm_in.size()] = strm_in[randomIndices[i]/strm_in.size()][randomIndices[i]%strm_in.size()]+1;
            }
        //}
        return true;
    }
};

// question:
// - should the rand poplar functions be used here or outside the codelet?
//   I ask because we're using the graph object in the poplar functions, and
//   I know we set those outside the codelet--would we have to pass the graph as
//   an input to the codelet?
// - have yall used the rand poplar functions in the past, is there anything we can
//   refer to for populating the parameters?
// - do we want to include the same size input array as in stride1? if so, do we
//   want to go thru each element in the array just in random order, or completely random
//   indices?
// - do we want the output array to be ordered in the same way as the input array or in the
//   order of the random indices?

/*
// Compute function
    bool compute() {
        //if (ready_flag[0]) {
            // set seed for randomGen--probably not needed, I think it can be done w/
            // the createRandomIndices function below, but here it is just in case
            // void setSeed(poplar::Graph &graph, const poplar::Tensor &masterSeed, uint32_t seedModifier, poplar::program::Sequence &prog, const poplar::DebugContext &debugContext = {})
            
            // generate array of random indices--function definition below just in case
            // poplar::Tensor uniform(poplar::Graph &graph, const poplar::Tensor *seed, uint32_t seedModifier, const poplar::Tensor &reference, const poplar::Type &outType, double minVal, double maxVal, poplar::program::Sequence &prog, const poplar::DebugContext &debugContext = {})
            for (unsigned i = 0; i < strm_in.size(); i++) {
                strm_out[i] = strm_in[idx[i]]+1;
            }
        //}
        return true;
    }
};
    
    // example of how we could create random indices:
    poplar::Tensor createRandomIndices(poplar::Graph &graph, const poplar::Tensor &reference, poplar::program::Sequence &prog) {
        unsigned size = reference.dim(0);
        poplar::Tensor seed = graph.addConstant(poplar::UNSIGNED_INT, {}, {123});
        return poplar::uniform(graph, &seed, 0, reference, poplar::UNSIGNED_INT, 0, size - 1, prog);
    }  

*/