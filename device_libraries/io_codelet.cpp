#include <poplar/Vertex.hpp>

using namespace poplar;

class IOVertex : public Vertex {
public:

Input<Vector<float>> strm_in;
//Input<Vector<int>> ready_flag;
//Input<Vector<int>> num_elements;
Output<Vector<float>> strm_out;

// function:
// stride1
// 
// description:
// takes in a vector of floats and adds 1 to every element.

// Compute function
    bool compute() {
        //if (ready_flag[0]) {
            for (unsigned i = 0; i < strm_in.size(); i++) {
                strm_out[i] = strm_in[i]+1;
            }
        //}
        return true;
    }
};

// function:
// strideN
// 
// description:
// takes in a vector of floats and adds 1 to every Nth element.
// 
// to-do:
// accept N as input for stride length
// 
// question:
// - Should we just be accessing the elements we want to update?
//   By doing it this way we're accessing the info in each element of the array,
//   but if we only access the elements we want to change, we'd either have to 
//   return an output array w/ a different size w/ just those changes or same size
//   w/ zeros for the rest of the elements, and then somehow merge once out the codelet  

/*
// Compute function
    bool compute() {
        //if (ready_flag[0]) {
            for (unsigned i = 0; i < strm_in.size(); i++) {
                if (i%N == 0) { // adding 1 to appropriate stride elements 
                    strm_out[i] = strm_in[i]+1;
                else { // copying array for the rest
                    strm_out[i] = strm_in[i];
                }
            }
        //}
        return true;
    }
};

// function:
// rand
// 
// description:
// takes in a vector of floats and adds 1 to every element in a random order.
// 
// to-do:
// 
// 
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