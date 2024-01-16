#ifndef SHORTESTPATH_HTTPSERVER_H
#define SHORTESTPATH_HTTPSERVER_H
#include <string>

struct HttpServerPrivate;
class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    //开启调试
    void SetDebug(bool debug);
    //设置输出文件路径
    void SetOutFilePath(const std::string &file_path);
    //初始化路径文件
    void InitPathData(const std::string &file_name);
    //监听端口
    bool Listen(int port);
private:
    void InitRoute();
private:
    HttpServerPrivate *d_= nullptr;
};


#endif //SHORTESTPATH_HTTPSERVER_H
