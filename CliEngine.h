#include <string>
#include "InputInfo.h"
#include "common/SingleTon.h"
namespace CLI{

class CliEngine: public SingleTon<CliEngine>{
public:
    friend SingleTon;
    static void Usage();
    static InputInfo GetInput(int argc,char *argv[]);
};



}