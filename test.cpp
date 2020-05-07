#include <iostream>
#include "CliEngine.h"
using namespace std;
int main(int argc, char* argv[]){
    CLI::CliEngine::Usage();
    CLI::CliEngine::GetInput(argc,argv);
    return 0;
}