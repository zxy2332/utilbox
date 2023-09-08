#include "file_util.h"
#include "time_util.h"
#include "iostream"

int main(int argc,char **argv){
    using namespace zxy;
    try{

        std::cout<<util::TimeStringNow()<<std::endl;
        std::cout<<util::file_contents("H:/导出/五自由度无人机/五自由度无人机.xml")<<std::endl;
    }catch (const std::exception &e){
        std::cout<<e.what()<<std::endl;
    }
    return 0;
}