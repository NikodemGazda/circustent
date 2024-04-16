#include <poplar/Vertex.hpp>
#include <cstdlib>
using namespace poplar;

class IOVertex : public Vertex {
public:

// Input<Vector<float>> strm_in;
Input<Vector<int>> N;
// Output<Vector<float>> strm_out;

Vector<Input<Vector<float>>> strm_in;
// Vector<Input<Vector<int>>> N;
Vector<Output<Vector<float>>> strm_out;

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

// Compute function
    bool compute() {
        //if (ready_flag[0]) {
            // for (unsigned i = 0; i < strm_in.size()+strm_in[0].size(); i+=N[0]) {
            //     strm_out[i/strm_in.size()][i%strm_in.size()] = strm_in[i/strm_in.size()][i%strm_in.size()]+1;
            // }
            strm_out[0][0] = strm_in.size();
            strm_out[0][1] = strm_in[0].size();
            strm_out[0][2] = strm_in.size()+strm_in[0].size();


            // change to this if we care abt the contents of the output array vvvv

            // for (unsigned i = 0; i < strm_in.size(); i++) {
            //     if (i%N == 0) { // adding 1 to appropriate stride elements 
            //         strm_out[i] = strm_in[i]+1;
            //     else { // copying array for the rest
            //         strm_out[i] = strm_in[i];
            //     }
            // }
        //}
        return true;
    }
};