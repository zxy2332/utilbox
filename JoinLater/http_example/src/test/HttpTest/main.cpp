#include "HttpTest.h"
#include "cmdline/cmdline.h"
#include <filesystem>



int main(int argc,char *argv[]) {
    try {
        auto current_path = std::filesystem::current_path().generic_string();
        cmdline::parser p;
        //http地址
        p.add<std::string>("url", 'u', "Http Sever url", false, "localhost:8000");

        //图路径文件
        p.add<std::string>("graph_path_data", 'g', "Graph Path Data file", false,current_path+"/PathData.csv");

        //约束文件路径
        p.add<std::string>("calculate_file", 'c', "Calculate file path", false, current_path+"/calculate_file.json");

        p.parse_check(argc, argv);

        auto url = p.get<std::string>("url");
        auto graph_path_data = p.get<std::string>("graph_path_data");
        auto calculate_file = p.get<std::string>("calculate_file");


        HttpTest client;
        std::string op;
        std::string value;
        client.Conncet(url);
        std::cout<<"input option:";
        while(std::cin>>op){
            try{
                std::cout<<"input option value:";
                std::cin>>value;
                if (op == "url" || op == "-u"){
                    if (!value.empty()){
                        url = value;
                    }
                    std::cout<<"Connect to "<<url<<std::endl;
                    client.Conncet(url);
                }else if(op == "graph_path_data" || op == "-g"){
                    if (!value.empty()){
                        graph_path_data = value;
                    }
                    std::cout<<"graph_path_data path: "<<graph_path_data<<std::endl;
                    auto body = file_contents(graph_path_data);
                    std::cout<<"Call InitPathData,req.body: "<<body<<std::endl;
                    auto res =client.InitPathData(body);
                    if(res){
                        std::cout<<"Call InitPathData,res.error:"<<res.error()<<" res.body: "<<res->body<<std::endl;
                    }else{
                        std::cout<<"Call InitPathData Faild: "<<res.error()<<std::endl;
                    }
                }else if(op == "calculate_file" || op == "-c"){
                    if (!value.empty()){
                        calculate_file = value;
                    }
                    std::cout<<"calculate_file path: "<<calculate_file<<std::endl;
                    auto body = file_contents(calculate_file);
                    std::cout<<"Call CalculateShortest,req.body: "<<body<<std::endl;
                    auto res =client.CalculateShortest(body);
                    if(res){
                        std::cout<<"Call CalculateShortest,res.error:"<<res.error()<<" res.body: "<<res->body<<std::endl;
                    }else{
                        std::cout<<"Call CalculateShortest Faild: "<<res.error()<<std::endl;
                    }
                }else if(op == "RefreshPathData" || op == "-R"){
                    auto res =client.RefreshPathData();
                    if(res){
                        std::cout<<"Call RefreshPathData,res.error:"<<res.error()<<" res.body: "<<res->body<<std::endl;
                    }else{
                        std::cout<<"Call RefreshPathData Faild: "<<res.error()<<std::endl;
                    }
                }
            }catch (const std::exception &e){
                std::cout<<e.what()<<std::endl;
            }
            std::cout<<"input option:";
        }
    }catch (const std::exception &e){
        std::cout<<e.what()<<std::endl;
        return -1;
    }
    return 0;
}

