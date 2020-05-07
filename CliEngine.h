#include <string>
#include "InputInfo.h"
namespace CLI{

class CliEngine{
public:
    static void Usage();
    static InputInfo GetInput(int argc,char *argv[]);
};



}