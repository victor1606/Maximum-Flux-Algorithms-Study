#include "maxflow.h"
#include <fstream>

int main () {
    std::ifstream input_file;
    std::ofstream output_file;
    
    input_file.open("./test.in");
    output_file.open("./test.out");
 
    output_file << maxflow(input_file) << "\n";
    
    input_file.close();
    output_file.close();
    return 0;
}