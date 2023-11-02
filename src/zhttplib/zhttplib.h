#ifndef Z_HTTPLIB_H
#define Z_HTTPLIB_H

#include <string>
#include <functional>
#include <memory>
#include <map>


namespace zhttplib {
    class HttpBasePrivate;

    class HttpBase {
    public:
        enum LogLevel{
            Trace,
            Debug,
            Info,
            Error,
            FatalError
        };
    public:
        HttpBase();

        virtual ~HttpBase();

        //开启调试，暂未明确作用
        void SetDebug(bool debug);

        //设置日志回调
        void SetLogFun(std::function<bool(const std::string &msg, int level)> log);

    protected:
        //写日志
        bool WriteLog(const std::string &msg, int level = 0);
        //判断调试是否开启
        bool IsDebug();
    private:
        std::unique_ptr<HttpBasePrivate> d_ = nullptr;
    };

    class HttpServerPrivate;

    class HttpServer :public HttpBase{
    public:
        HttpServer();

        virtual ~HttpServer();

        //初始化路径文件
        void AddPost(const std::string &path,std::function<bool(const std::string &req_body,std::string &res_body)> cb);

        void AddGet(const std::string &path,std::function<bool(const std::string &req_body,std::string &res_body)> cb);

        //监听端口
        bool Listen(int port);

    private:
        void InitRoute();

    private:
        std::unique_ptr<HttpServerPrivate> d_ = nullptr;
    };


    class HttpClientPrivate;

    class HttpClient {
    public:
        struct Status{
            enum Error {
                Success = 0,
                Unknown,
                Connection,
                BindIPAddress,
                Read,
                Write,
                ExceedRedirectCount,
                Canceled,
                SSLConnection,
                SSLLoadingCerts,
                SSLServerVerification,
                UnsupportedMultipartBoundaryChars,
                Compression,
                ConnectionTimeout,
                ProxyConnection,

                // For internal use only
                SSLPeerCouldBeClosed_,
                ClientNullptr,
            };
            //错误信息
            std::string msg;
            //http返回的错误码
            int http_code;
            //接口状态码
            int error;
            //响应体
            std::string body;
        };
    public:
        HttpClient();

        virtual ~HttpClient();
    //建立连接
    bool Connenct(const std::string &url);
    //发出post请求
    Status Post(const std::string &path,const std::string &req_body,const std::string &content_type);
    //发出Get请求
    Status Get(const std::string &path);
    private:
        std::unique_ptr<HttpClientPrivate> d_ = nullptr;
    };

};
#endif //Z_HTTPLIB_H
