#include "HttpServer.h"

#include "cpp-httplib/httplib.h"
#include "HttpServer.h"

#include "cslog.h"
#include "ShortestEscape.h"
#include "util.h"
#include "json/json.hpp"

void AddCros(httplib::Response &res){
    res.set_header("Access-Control-Allow-Origin", "*");  // 若有端口需写全（协议+域名+端口）
    res.set_header("Access-Control-Allow-Credentials", "true");
    res.set_header("Access-Control-Expose-Headers", "content-type");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type,Access-Control-Allow-Headers,Authorization,X-Requested-With,Accept, X-Access-Token, X-Application-Name, X-Request-Sent-Time");
}
//请求转为字符串
std::string ToString(const httplib::Request &req){
    nlohmann::json req_json;
    req_json["method"] = req.method;
    req_json["path"] = req.path;
    req_json["headers"] = req.headers;
    req_json["body"] = req.body;
    req_json["remote_addr"] = req.remote_addr;
    req_json["remote_port"] = req.remote_port;
    req_json["local_addr"] = req.local_addr;
    req_json["local_port"] = req.local_port;
    return req_json.dump();
}
//响应转为字符串
std::string ToString(const httplib::Response &res){
    nlohmann::json res_json;
   res_json["version"] = res.version;
   res_json["status"] = res.status;
   res_json["reason"] = res.reason;
   res_json["headers"] = res.headers;
   res_json["body"] = res.body;
   res_json["location"] = res.location;
   return res_json.dump();
}

//Header转为字符串
std::string ToString(const httplib::Headers &header){
    nlohmann::json header_json;
    for(const auto &it:header){
        nlohmann::json h;
        h["key"] = it.first;
        h["value"] = it.second;
        header_json.push_back(h);
    }
    return header_json.dump();
}

//读取json数据
template<class T>
bool GetJsonData(const nlohmann::json &js, const std::string &key, T &data) {
    if (js.count(key)) {
        if (!js.at(key).empty()) {
            data = js[key];
            return true;
        }
    }
    return false;
}


struct HttpServerPrivate {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    // HTTPS
    httplib::SSLServer svr;
#else
    // HTTP
    httplib::Server svr;
#endif
    //最短路径算法
    ShortestEscape algo;
    //路径图csv文件路径
    std::string path_data;
    //调试
    bool debug = false;
    //输出文件路径
    std::string out_file;
};

HttpServer::HttpServer() {
    d_ = new HttpServerPrivate;
}

HttpServer::~HttpServer() {
    delete d_;
}

void HttpServer::InitPathData(const std::string &file_name) {
    d_->path_data = file_name;
    try {
        d_->algo.ReadPathDataFile(d_->path_data);
    }catch (const std::exception &e){
        LOG_ERROR()<<"ReadPathDataFile Error:"<<e.what();
    }
}

bool HttpServer::Listen(int port) {
    InitRoute();
    return d_->svr.listen("0.0.0.0", port);
}

void HttpServer::InitRoute() {
    // 刷新路径文件
    d_->svr.Get("/RefreshPathData", [this](const httplib::Request &req, httplib::Response &res) {
        TimeBegin(RefreshPathData_time)
        try {
            AddCros(res);
            LOG_DEBUG()<<"RefreshPathData req:"<<ToString(req);
            LOG_INFO()<<"RefreshPathData req.body:"<<req.body;

            d_->algo.ReadPathDataFile(d_->path_data);

            nlohmann::json res_json;
            res_json["msg"] = u8"Refresh successfully";
            res_json["code"] = 200;
            res.set_content(res_json.dump(),"application/json");

            LOG_INFO()<<"RefreshPathData res:"<<ToString(res);
        } catch (const std::exception &e) {
            LOG_ERROR() <<"RefreshPathData Catch:"<< e.what() << std::endl;

            res.status = 500;
            nlohmann::json res_json;
            res_json["msg"] = e.what();
            res_json["code"] = 500;
            res.set_content(res_json.dump(),"application/json");
            LOG_ERROR()<<"RefreshPathData res: "<<ToString(res);
        }
        TimeEnd(d_->debug,RefreshPathData_time)
    });
    // 初始化路径文件
    // 从本地读取 解析file_path
    // TODO::读取文件内容 解析file_content
    // {
    //      ”file_path“:"D:/AStar/cwt/PathData.csv"
    //      "file_content": "TODO"
    // }
    d_->svr.Post("/InitPathData", [this](const httplib::Request &req, httplib::Response &res) {
        TimeBegin(InitPathData_time)
        try {
            AddCros(res);
            LOG_DEBUG()<<"InitPathData req:"<<ToString(req);
            LOG_INFO()<<"InitPathData req.body:"<<req.body;
            auto req_js = nlohmann::json::parse(req.body);
            std::string file_path;
            if (GetJsonData(req_js,"file_path",file_path)){
                d_->path_data = file_path;
                d_->algo.ReadPathDataFile(d_->path_data);
            }

            //TODO::读取文件内容
            std::string file_content;
            if (GetJsonData(req_js,"file_content",file_content)){
                d_->algo.ReadPathDataContent(file_content);
            }

            nlohmann::json res_json;
            res_json["msg"] = u8"Init successfully";
            res_json["code"] = 200;
            res.set_content(res_json.dump(),"application/json");

            LOG_INFO()<<"InitPathData res:"<<ToString(res);
        } catch (const std::exception &e) {
            LOG_ERROR() << "InitPathData catch:"<< e.what() << std::endl;

            res.status = 500;
            nlohmann::json res_json;
            res_json["msg"] = e.what();
            res_json["code"] = 500;
            res.set_content(res_json.dump(),"application/json");
            LOG_ERROR()<<"InitPathData res:"<<ToString(res);
        }
        TimeEnd(d_->debug,InitPathData_time)
    });

    //读取约束，并计算最短路径
    // {
    //     "StartID": 1,    //开始节点id
    //     "EndID": 24,     //结束节点id
    //     "State": [       //约束节点状态
    //         {
    //             "NodeID": 23,    //约束节点id
    //             "State": 1       //约束状态，0为关，1为开
    //         }
    //     ]
    // }
    d_->svr.Post("/CalculateShortest", [this](const httplib::Request &req, httplib::Response &res) {
        TimeBegin(CalculateShortest_time)
        try {
            AddCros(res);
            const auto &req_js =ToString(req);
            LOG_DEBUG()<<"CalculateShortest req:"<<req_js;
            if(d_->debug){
                util::write_file("out/req.json", req_js);
            }
            LOG_INFO()<<"CalculateShortest req.body:"<<req.body;

            std::string path;
            if (d_->algo.GetShortestPath(req.body, path)) {
                LOG_INFO() << "CalculateShortest result:" << path << std::endl;
            }
            util::write_file(d_->out_file, path);
            res.body = path;
            res.set_content(path,"application/json");
            res.status = 200;
            LOG_INFO()<<"CalculateShortest res:"<<ToString(res);
        } catch (const std::exception &e) {
            LOG_ERROR() <<"CalculateShortest catch:"<< e.what();
            res.status = 500;
            nlohmann::json res_json;
            res_json["msg"] = e.what();
            res_json["code"] = 500;
            res.set_content(res_json.dump(),"application/json");
            LOG_ERROR()<<"CalculateShortest res:"<<ToString(res);
        }
        TimeEnd(d_->debug,CalculateShortest_time)
    });
}

void HttpServer::SetDebug(bool debug) {
    LOG_DEBUG()<<"SetDebug:"<<debug;
    d_->debug = debug;
    d_->algo.SetDebug(debug);
}

void HttpServer::SetOutFilePath(const std::string &file_path) {
    LOG_DEBUG()<<"SetOutFilePath:"<<file_path;
    d_->out_file = file_path;
}

