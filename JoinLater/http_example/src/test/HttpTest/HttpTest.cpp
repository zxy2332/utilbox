#include "HttpTest.h"

#include "cpp-httplib/httplib.h"
#include "json/json.hpp"


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


struct HttpTestPrivate {
    httplib::Client *cli = nullptr;

    ~HttpTestPrivate(){
        delete cli;
    }
};

HttpTest::HttpTest() {
    d_ = new HttpTestPrivate;
}

HttpTest::~HttpTest() {
    delete d_;
}

bool HttpTest::Conncet(const std::string &url) {
    delete d_->cli;
    d_->cli = new httplib::Client(url);
    return true;
}

httplib::Result HttpTest::CalculateShortest(const std::string &body) {
    if(d_->cli == nullptr){
        return  httplib::Result();
    }
    auto res = d_->cli->Post("/CalculateShortest", httplib::Headers(),body,"application/json");
    return res;
}

httplib::Result HttpTest::RefreshPathData() {
    if(d_->cli == nullptr){
        return  httplib::Result();
    }
    auto res = d_->cli->Get("/RefreshPathData", httplib::Headers());
    return res;
}

httplib::Result HttpTest::InitPathData(const std::string &body) {
    if(d_->cli == nullptr){
        return  httplib::Result();
    }
    auto res = d_->cli->Post("/InitPathData", httplib::Headers(),body,"application/json");
    return res;
}
