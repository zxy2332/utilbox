#pragma once
//C++
#include <functional>
#include <thread>
#include <map>
#include <mutex>
#include <atomic>
//obj
#include "ByteStream.hpp"
#include "zsocket_export.h"

#ifdef WIN32 
#include <WinSock2.h>
#else
#include <dlfcn.h>
#include <unistd.h>

#endif

namespace zsocket {
	struct SocketType {
#ifdef WIN32 //如果定义了WINDOWS 
		SOCKET sc;
		SocketType(const SOCKET &s) {
			sc = s;
		}
#else
		int sc;
		SocketType(const int &s) {
			sc = s;
		}
#endif
		bool effective = false;

		SocketType() {
		}
	};
	class ZSOCKET_EXPORT TcpObject {
	public:
		TcpObject();
		TcpObject(const std::string &ip, int port);
		//debug=true 输出调试信息
		TcpObject(bool debug);
		virtual ~TcpObject();

		virtual bool Close();

		//设置接收缓冲区大小 Default = 1024
		void SetRecvBuffSize(size_t size);
		//设置输出调试信息
		//debug=true 输出调试信息
		void SetDebug(bool debug);

		void SetIpPort(const std::string &ip, int port);
		std::string GetIpPort();
		std::string GetIP();
		int GetPort();
	protected:
		//创建socket
		virtual bool CreateSocket();
		//关闭socket
		bool CloseSocket(const SocketType &sk);
		//绑定端口
		bool Bind(int port);
		//接收数据
		int RecvFrom(const std::string &connect_ip, SocketType sock, ByteStream &data);
	protected:
		SocketType *sock_;
		bool debug_ = false;
		std::string my_ip_;
		int my_port_;
		size_t recv_size_ = 1024;
	};

	class ZSOCKET_EXPORT TcpSever:public TcpObject {
	public:
		TcpSever():TcpObject(){}
		TcpSever(bool debug) :TcpObject(debug) {}
		~TcpSever();

		bool Listen(int port);
		//connect_ip_port: ip:port
		bool OnConnect(std::function<void(const std::string &connect_ip_port)> func);

		//connect_ip_port: ip:port
		bool DisConnect(const std::string &connect_ip);
		
		//设置接收数据回调函数
		bool SetRecvCallBack(std::function<bool(const std::string &connect_ip_port, const ByteStream &data)> recv_func = nullptr);
		//设置接收数据回调函数
		bool SetRecvCallBack(const std::string &connect_ip,std::function<bool(const std::string &connect_ip_port, const ByteStream &data)> recv_func = nullptr);

		bool SendTo(const std::string &connect_ip, const ByteStream &data);

		void SetMaxConncetCount(int max_connect_count);

		virtual bool Close() override;
	protected:
		struct ClientStruct {
			SocketType sk;
			std::thread th;
			std::vector<std::function<bool(const std::string &connect_ip_port, const ByteStream &data)>> recv_list;
			std::mutex mx;

			ClientStruct() {

			}
		};
		//服务端监听
		bool ListenFunc();

		bool RecvAndCallBack(const std::string &client_ip_port, ClientStruct &client);

		//检查需要断开的连接
		void CheckDisConect();

		//connect_ip_port: ip:port
		bool DisConnect(ClientStruct &client);
	protected:
		std::map<std::string, ClientStruct> connects_;
		std::mutex conn_mx_;

		std::vector<std::function<bool(const std::string &connect_ip_port, const ByteStream &data)>> common_recv_list_;
		std::mutex common_recv_mx_;

		//listen thread
		std::thread listen_thread_;
		std::function<void(const std::string &connect_ip_port)> on_connect_;
		std::atomic<bool> stop_listen_ = false;

		std::atomic<bool> all_stop_ = false;

		int max_connect_count_ = 10;

		std::vector<std::string> dis_connect_;
		std::mutex dis_mx_;
	};

	class ZSOCKET_EXPORT TcpClient :public TcpObject {
	public:
		TcpClient() :TcpObject() {}
		TcpClient(bool debug) :TcpObject(debug) {}
		~TcpClient();

		bool Connect(const std::string &address, int port);

		bool DisConnect();
		//接收数据并调用回调函数
		bool SetRecvCallBack(std::function<bool(const ByteStream &data)> recv_func = nullptr);

		bool Send(const ByteStream &data);

		virtual bool Close() override;

	private:
		//接收数据并调用回调函数
		bool RecvAndCallBack();
	private:
		bool is_connect_ = false;
		std::thread recv_thread_;
		std::atomic<bool> stop_recv_= false;
		std::function<bool(const ByteStream &data)> recv_func_;

		std::string ser_ip_port_;
	};
};
