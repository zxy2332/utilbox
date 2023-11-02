#include "zhttplib.h"
#include <iostream>
#include "json/json.hpp"
#include "cpp-httplib/httplib.h"


void AddCros(httplib::Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");  // 若有端口需写全（协议+域名+端口）
    res.set_header("Access-Control-Allow-Credentials", "true");
    res.set_header("Access-Control-Expose-Headers", "content-type");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers",
                   "Content-Type,Access-Control-Allow-Headers,Authorization,X-Requested-With,Accept, X-Access-Token, X-Application-Name, X-Request-Sent-Time");
}

//请求转为字符串
std::string ToString(const httplib::Request &req) {
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
std::string ToString(const httplib::Response &res) {
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
std::string ToString(const httplib::Headers &header) {
    nlohmann::json header_json;
    for (const auto &it: header) {
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

namespace zhttplib {
    class HttpBasePrivate {
    public:
        bool debug = false;
        std::function<bool(const std::string &, int)> log;

        const std::vector<std::string> log_level = {"Trace", "Debug", "Info", "Error", "FatalError"};
    };

    HttpBase::HttpBase() {
        d_ = std::make_unique<HttpBasePrivate>();
    }

    zhttplib::HttpBase::~HttpBase() {

    }

    void zhttplib::HttpBase::SetDebug(bool debug) {
        d_->debug = debug;
    }

    void zhttplib::HttpBase::SetLogFun(std::function<bool(const std::string &, int)> log) {
        d_->log = log;
    }

    bool zhttplib::HttpBase::WriteLog(const std::string &msg, int level) {
        if (d_->log) {
            return d_->log(msg, level);
        } else {
            if (level >= d_->log_level.size()) {
                level = int(d_->log_level.size()) - 1;
            }
            if (level < 0) {
                level = 0;
            }
            std::cout << "[" << d_->log_level[level] << "]" << msg;
            return true;
        }
    }

    bool HttpBase::IsDebug() {
        return d_->debug;
    }

    class HttpServerPrivate {
    public:
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        // HTTPS
    httplib::SSLServer svr;
#else
        // HTTP
        httplib::Server svr;
#endif
        std::unordered_map<std::string, std::function<bool(const std::string &, std::string &)>> post;
        std::unordered_map<std::string, std::function<bool(const std::string &, std::string &)>> get;

    };

    HttpServer::HttpServer() {

    }

    HttpServer::~HttpServer() {

    }

    void HttpServer::AddPost(const std::string &path, std::function<bool(const std::string &, std::string &)> cb) {
        d_->post[path] = cb;
    }

    void HttpServer::AddGet(const std::string &path, std::function<bool(const std::string &, std::string &)> cb) {
        d_->get[path] = cb;
    }

    bool HttpServer::Listen(int port) {
        InitRoute();
        return d_->svr.listen("0.0.0.0", port);
    }

    void HttpServer::InitRoute() {
        for (const auto &[path, recv_cb]: d_->post) {
            d_->svr.Post(path, [=](const httplib::Request &req, httplib::Response &res) {
                try {
                    if (IsDebug()) {
                        WriteLog(path + " req:" + ToString(req), HttpBase::Debug);
                    }
                    WriteLog(path + " req body:" + req.body, HttpBase::Info);
                    //添加跨域响应头
                    AddCros(res);

                    bool rv = false;
                    std::string res_body;
                    if (recv_cb) {
                        rv = recv_cb(req.body, res_body);
                    } else {
                        throw std::runtime_error("The callback function is not registered");
                    }
                    res.set_content(res_body, "application/json");
                    if (rv) {
                        res.status = 200;
                    } else {
                        res.status = 500;
                    }
                    if (IsDebug()) {
                        WriteLog(path + " res:" + ToString(res), HttpBase::Debug);
                    }
                    WriteLog(path + " res body:" + res.body, HttpBase::Info);
                } catch (const std::exception &e) {
                    WriteLog(path + " catch exception:" + e.what(), HttpBase::Error);

                    res.status = 500;
                    nlohmann::json res_json;
                    res_json["msg"] = e.what();
                    res_json["code"] = 500;
                    //项目实践中发现 e.what() 中可能存在非utf-8的内容，json的dump会抛出异常
                    //此处对其针对处理
                    std::string error_msg;
                    try {
                        error_msg = res_json.dump();
                    } catch (const std::exception &e) {
                        error_msg = std::string("{\"msg\": \"") + e.what() + "\",\"code\":500}";
                    }
                    res.set_content(error_msg, "application/json");
                    WriteLog(path + " req(Error):" + ToString(req), HttpBase::Error);
                    WriteLog(path + " res(Error):" + ToString(res), HttpBase::Error);
                }
            });
        }
    }

    class HttpClientPrivate {
    public:
        std::unique_ptr<httplib::Client> cli = nullptr;
    };

    HttpClient::HttpClient() {

    }

    HttpClient::~HttpClient() {

    }

    bool HttpClient::Connenct(const std::string &url) {
        d_->cli = std::make_unique<httplib::Client>(url);
        return d_->cli != nullptr;
    }

    HttpClient::Status
    HttpClient::Post(const std::string &path, const std::string &req_body, const std::string &content_type) {
        if (d_->cli == nullptr) {
            return {"No connection",-1,-1,""};
        }
        auto res = d_->cli->Post(path, httplib::Headers(), req_body, content_type);
        auto err = res.error();
        return {httplib::to_string(err),Status::Error(err), res->status, res->body};
    }

    HttpClient::Status
    HttpClient::Get(const std::string &path) {
        if (d_->cli == nullptr) {
            return {"No connection",-1,-1,""};
        }
        auto res = d_->cli->Get(path);
        auto err = res.error();
        return {httplib::to_string(err),Status::Error(err), res->status, res->body};

    }
};