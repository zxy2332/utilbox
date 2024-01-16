#include "file_util.h"
#include "time_util.h"
#include "iostream"


using namespace zxy;

int TestTime();
int main(int argc,char **argv){



    return  0;
    try{
        util::write_file("只需考虑成为.txt","awdaaaaaaa大卫卡梅伦看啊达瓦大");
        TestTime();
        //std::cout<<util::TimeStringNow()<<std::endl;
        //std::cout<<util::file_contents("H:/导出/五自由度无人机/五自由度无人机.xml")<<std::endl;
    }catch (const std::exception &e){
        std::cout<<e.what()<<std::endl;
    }
    return 0;
}

int TestTime() {
    int count = 0;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::steady_clock::now().time_since_epoch());
    auto print = [&]() {
        auto new_now = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::steady_clock::now().time_since_epoch());

        std::cout<< new_now.count() <<":" << count++ << std::endl;
        std::cout<< (new_now -now).count()<<"ms"  << std::endl;
        now = new_now;
    };
    print();
    util::ZTimer t(std::chrono::milliseconds(1000), print);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    t.Stop();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    print();
    t.Start(std::chrono::milliseconds(2000));
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}