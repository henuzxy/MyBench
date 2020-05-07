#include <iostream>
#include "CliEngine.h"
using namespace std;
int main(int argc, char* argv[]){
    CLI::CliEngine::getinstance().Usage();
    CLI::CliEngine::getinstance().GetInput(argc,argv);
    return 0;
}