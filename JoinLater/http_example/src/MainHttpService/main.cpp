#include "cslog.h"
#include "HttpServer.h"
#include "cmdline/cmdline.h"
#include <filesystem>

const std::vector<std::string> &LogStr = {"TRACE(0)","DEBUG(1)","INFO(2)","WARNING(3)","ERROR(4)","CRITICAL(5)","OFF(6)"};

enum Environment{
    Debug,
    Production
};

int main(int argc,char *argv[]) {
    try {
        int port = 8000;
        std::string graph_path_data = "PathData.csv";
        std::string out_file = "out/out.json";
        bool debug = true;
        int log_level = 0;

        auto env = Debug;

        if(env == Environment::Debug ) {
            port = 8000;
            graph_path_data = "PathData.csv";
            out_file = "out/out.json";
            debug = true;
            log_level = 0;
        }else if(env == Environment::Production){
            port = 8000;
            graph_path_data = "PathData.csv";
            out_file = "out/out.json";
            debug = false;
            log_level = 1;
        }

        std::filesystem::remove("logs/service.log");
        //日志
        Log_Manager()->CreateLog("logs/service.log");
        cmdline::parser p;
        //http端口号
        p.add<int>("port", 'p', "Http Sever port", false, port);

        //图路径文件
        p.add<std::string>("graph_path_data", 'g', "Graph Path Data file", false, graph_path_data);

        //输出文件路径
        p.add<std::string>("out_file", 'o', "Out file path", false, out_file);

        //开启调试输出
        p.add("debug", 'd', "Open debug output", false, debug);

        std::string log_str ;
        for (auto it = LogStr.begin(); it != LogStr.end(); it++) {
            log_str += *it + ";";
        }
        //设置日志等级
        p.add<int>("log_level", 'l', "Log Level:"+log_str, false, log_level);

        p.parse_check(argc, argv);

        port = p.get<int>("port");
        graph_path_data = p.get<std::string>("graph_path_data");
        out_file = p.get<std::string>("out_file");
        debug = p.get<bool>("debug");
        log_level = p.get<int>("log_level");
        if (log_level>=LogStr.size()){
            log_level = LogStr.size()-1;
        }
        //设置日志等级
        Log_Manager()->SetLevel(log_level);

        LOG_INFO()<<"Shortest Server Start";
        LOG_INFO()<<"port = "<< port;
        LOG_INFO()<<"graph_path_data = "<< graph_path_data;
        LOG_INFO()<<"out_file = "<< out_file;
        LOG_INFO()<<"debug = "<< debug;
        LOG_INFO()<<"log_level = "<< LogStr[log_level];


        HttpServer server;
        server.SetDebug(debug);
        server.SetOutFilePath(out_file);
        server.InitPathData(graph_path_data);
        if(!server.Listen(port)){
            LOG_ERROR()<<"Port("<<port<<") listening failed"<<std::endl;
            Log_Manager()->CloseLog();
            return -1;
        }
        Log_Manager()->CloseLog();
    }catch (const std::exception &e){
        LOG_ERROR()<<e.what()<<std::endl;
        Log_Manager()->CloseLog();
        return -1;
    }
    return 0;
}

