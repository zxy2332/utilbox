#include "tcp_obj.h"

//C++
#include <iostream>

#ifdef WIN32 
#include <WS2tcpip.h>
//#include <ws2tcpip.h>
//#include <winsock2.h>
//#include <ws2ipdef.h>
//#pragma comment(lib,"wsock32")
#pragma comment(lib,"Ws2_32.lib")

#define CS_GetErrorCode (WSAGetLastError())
#define CS_REUSEPORT SO_REUSE_MULTICASTPORT
#define CS_SOCKET_SIZE int
#else
#include <linux/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
//#include <dlfcn.h>
//#include <unistd.h>

#define CS_GetErrorCode (errno)
#define CS_REUSEPORT SO_REUSEPORT
#define CS_SOCKET_SIZE socklen_t

#endif
//obj

static constexpr int MaxBackLog = 5;

namespace zsocket {

///////////////////////////////////////////////////////////////////////////////////
	/***************** TcpObject ***************************/

	TcpObject::TcpObject()
	{
		sock_ = new SocketType();

#ifdef WIN32 //如果定义了WINDOWS 
		WSADATA wsaData;
		WSAStartup(MAKEWORD(1, 1), &wsaData); //windows下必须要初始化，不然socket会失败 
#endif
	}

	TcpObject::TcpObject(const std::string & ip, int port):TcpObject()
	{
		SetIpPort(ip, port);
	}

	TcpObject::TcpObject(bool debug):TcpObject()
	{
		debug_ = debug;
	}

	TcpObject::~TcpObject()
	{
		Close();
		delete sock_;
	}

	bool TcpObject::Close()
	{
		if (sock_->effective == true)
		{
			if (!CloseSocket(*sock_)) {
				return false;
			}
			sock_->effective = false;
		}
		return true;
	}

	bool TcpObject::CreateSocket()
	{
		if (sock_->effective == false)
		{
			if ((sock_->sc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				std::cerr<<"socket creation failed, error code:" + std::to_string(CS_GetErrorCode)<<std::endl;
				return false;
			}
			/*int nRcvBufferLen = 10 * 1024;
			int nSndBufferLen = 10 * 1024;
			int nLen = sizeof(int);

			setsockopt(sock_->sc, SOL_SOCKET, SO_SNDBUF, (char*)&nSndBufferLen, nLen);
			setsockopt(sock_->sc, SOL_SOCKET, SO_RCVBUF, (char*)&nRcvBufferLen, nLen);*/
			sock_->effective = true;
		}
		return true;
	}
	bool TcpObject::CloseSocket(const SocketType &sk)
	{
#ifdef WIN32
        if (closesocket(sock_->sc) != 0)
#else
        if(close(sock_->sc))
#endif
		{
			std::cerr << u8"Socket closing faild, error code:" + std::to_string(CS_GetErrorCode) << std::endl;
			return false;
		}
		return true;
	}
	bool TcpObject::Bind(int port)
	{
		if (CreateSocket()) {
			sockaddr_in listen_addr;
			memset(&listen_addr, 0, sizeof(listen_addr));
			listen_addr.sin_family = AF_INET;
			listen_addr.sin_port = htons(port);
			listen_addr.sin_addr.s_addr = INADDR_ANY;

			if (bind(sock_->sc, (sockaddr*)&listen_addr, sizeof(listen_addr)))
			{
				std::cerr << "port:" + std::to_string(port) + " binding error, error code:" + std::to_string(CS_GetErrorCode) << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}


	void TcpObject::SetRecvBuffSize(size_t size)
	{
		recv_size_ = size;
	}

	void TcpObject::SetDebug(bool debug)
	{
		debug_ = debug;
	}

	void TcpObject::SetIpPort(const std::string & ip,int port)
	{
		my_ip_ = ip;
		my_port_ = port;
	}

	std::string TcpObject::GetIpPort()
	{
		return my_ip_+":"+std::to_string(my_port_);
	}

	std::string TcpObject::GetIP()
	{
		return my_ip_;
	}

	int TcpObject::GetPort()
	{
		return my_port_;
	}

	int TcpObject::RecvFrom(const std::string &connect_ip, SocketType sock, ByteStream &data)
	{
		data.clear();
		data.resize(recv_size_);
		int recv_size = recv(sock.sc, data.c_str(), data.size(), 0);
		if (recv_size < 0) {
			auto error_code = CS_GetErrorCode;
			if (error_code == 10054) {
				std::cerr << u8"Connection reset by peer, error code:" + std::to_string(error_code) << std::endl;
			}
			else {
				std::cerr << u8"Data receiving failed, error code:" + std::to_string(error_code) << std::endl;
			}
			return error_code;
		}
		data.resize(recv_size);
		if (debug_)
		{
			std::string msg;
			msg += u8"Recv address:" + connect_ip;
			msg += u8" ,size:" + std::to_string(recv_size) + u8" byte";
			msg += u8"\n data:\n";
			msg += data.str();
			std::cout << msg << std::endl;
		}
		return 0;
	}

///////////////////////////////////////////////////////////////////////////////////
	/***************** TcpSever ***************************/
	TcpSever::~TcpSever()
	{
		Close();
	}

	bool TcpSever::Listen(int port)
	{
		if (Bind(port)) {
			if (listen(sock_->sc, MaxBackLog) !=0) {
				std::cerr << "port:" + std::to_string(port) + " listen faild, error code:" + std::to_string(CS_GetErrorCode) << std::endl;
				return false;
			}
			listen_thread_ = std::thread(&TcpSever::ListenFunc, this);
		}
		return false;
	}
	bool TcpSever::ListenFunc()
	{
		all_stop_ = false;
		stop_listen_ = false;
		while (!all_stop_.load() && !stop_listen_.load()) {
            sockaddr_in client_addr;
			CS_SOCKET_SIZE clinet_len = sizeof(client_addr);
			memset(&client_addr, 0, clinet_len);
			auto sock_connect = accept(sock_->sc, (sockaddr *)&client_addr, &clinet_len);
			if (all_stop_.load()|| stop_listen_.load()) {
				return true;
			}
			std::string client_ip_port = inet_ntoa(client_addr.sin_addr);
			client_ip_port += ":" + std::to_string(client_addr.sin_port);
			if (debug_) {
				std::cout << "A new connection, client IP Port:" << client_ip_port << std::endl;
			}
			CheckDisConect();
			{
				std::lock_guard<std::mutex> lk(conn_mx_);
				if (connects_.find(client_ip_port) != connects_.end()) {
					std::cerr << "Duplicate connections are received:" << client_ip_port << std::endl;
					continue;
				}
				if (connects_.size() >= max_connect_count_) {
					std::cerr << "Exceeded the Max Connect Number:" << max_connect_count_ << std::endl;
					continue;
				}
			auto &client = connects_[client_ip_port];
				if (client.sk.effective == false) {
					client.sk.sc = sock_connect;
					client.sk.effective = true;
					if (client.th.joinable()) {
						client.th.join();
					}
					client.th = std::thread([client_ip_port, &client, this]() {
						RecvAndCallBack(client_ip_port, client);
					});
				}
			}
			if (on_connect_) {
				on_connect_(client_ip_port);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return true;
	}
	bool TcpSever::RecvAndCallBack(const std::string & client_ip_port, ClientStruct &client)
	{
		ByteStream data;
		while (!all_stop_.load() && client.sk.effective) {
			auto rv = RecvFrom(client_ip_port, client.sk, data);
			if (rv == 0)
			{
				std::lock_guard<std::mutex> lk(client.mx);
				for (auto it = client.recv_list.begin(); it != client.recv_list.end();) {
					if (*it) {
						if (!(*it)(client_ip_port, data)) {
							it = client.recv_list.erase(it);
						}
						else {
							it++;
						}
					}
				}
				std::lock_guard<std::mutex> clk(common_recv_mx_);
				for (auto it = common_recv_list_.begin(); it != common_recv_list_.end();) {
					if (*it) {
						if (!(*it)(client_ip_port, data)) {
							it = common_recv_list_.erase(it);
						}
						else {
							it++;
						}
					}
				}
			}
			else if (rv == 10054) {
				//连接断开
				std::lock_guard lk(dis_mx_);
				dis_connect_.push_back(client_ip_port);
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return true;
	}
	void TcpSever::CheckDisConect()
	{
		//检查需要断开的连接
		std::lock_guard lk(dis_mx_);
		for (auto it : dis_connect_) {
			DisConnect(it);
		}
		dis_connect_.clear();
	}
	bool TcpSever::OnConnect(std::function<void(const std::string&connect_ip)> func)
	{
		on_connect_ = func;
		return true;
	}

	bool TcpSever::DisConnect(const std::string & connect_ip)
	{
		std::lock_guard lk(conn_mx_);
		auto it = connects_.find(connect_ip);
		if (it != connects_.end()) {
			if (DisConnect(it->second)) {
				connects_.erase(it);
				return true;
			}
		}
		return false;
	}

	bool TcpSever::DisConnect(ClientStruct & client)
	{
		if (!CloseSocket(client.sk)) {
			return false;
		}
		client.sk.effective = false;
		std::lock_guard clk(client.mx);
		client.recv_list.clear();
		if (client.th.joinable()) {
			client.th.join();
		}
		return true;
	}
	bool TcpSever::SetRecvCallBack(std::function<bool(const std::string&connect_ip_port, const ByteStream&data)> recv_func)
	{
		if (recv_func) {
			std::lock_guard lk(common_recv_mx_);
			common_recv_list_.push_back(recv_func);
			return true;
		}
		return false;
	}

	bool TcpSever::SetRecvCallBack(const std::string & connect_ip, std::function<bool(const std::string&connect_ip_port, const ByteStream&data)> recv_func)
	{
		if (recv_func) {
			std::lock_guard lk(conn_mx_);
			auto &client = connects_[connect_ip];
			std::lock_guard clk(client.mx);
			client.recv_list.push_back(recv_func);
			return true;
		}
		return false;
	}
	bool TcpSever::SendTo(const std::string &connect_ip, const ByteStream &data)
	{
		SocketType sk;
		{
			std::lock_guard lk(conn_mx_);
			auto it = connects_.find(connect_ip);
			if (it == connects_.end()) {
				std::cerr << u8"The connection has not been established! ip:port:" << connect_ip << std::endl;
				return false;
			}
			sk = it->second.sk;
		}

		int send_size = send(sk.sc, data.c_str(), data.size(), 0);
		if (send_size < 0) {
			std::cerr << u8"Message sending faild, error code:" + std::to_string(CS_GetErrorCode) << std::endl;
			return false;
		}
		if (debug_)
		{
			std::string msg;
			msg += u8"Send to address: " + connect_ip + u8", size: " + std::to_string(send_size) + u8" byte";
			msg += u8"\n data:\n";
			msg += data.str();
			std::cout << msg << std::endl;
		}
		return true;
	}
	void TcpSever::SetMaxConncetCount(int max_connect_count)
	{
		max_connect_count_ = max_connect_count;
	}


	bool TcpSever::Close()
	{
		all_stop_.store(true);
		if (sock_->effective == true)
		{
			if (!CloseSocket(*sock_)) {
				return false;
			}
			stop_listen_.store(true);
			if (listen_thread_.joinable())
			{
				listen_thread_.join();
			}
			sock_->effective = false;
		}
		for (auto it = connects_.begin(); it != connects_.end();) {
			if (DisConnect(it->second)) {
				it = connects_.erase(it);
			}
			else {
				it++;
			}
		}
		common_recv_list_.clear();
		on_connect_ = nullptr;
		return true;
	}


///////////////////////////////////////////////////////////////////////////////////
	/***************** TcpClient ***************************/

	TcpClient::~TcpClient()
	{
		Close();
	}

	bool TcpClient::Connect(const std::string & address, int port)
	{
		if (CreateSocket()) {
			sockaddr_in to_addr;
			memset(&to_addr, 0, sizeof(to_addr));
			to_addr.sin_family = AF_INET;
			to_addr.sin_port = htons(port);
			to_addr.sin_addr.s_addr = inet_addr(address.c_str());

			if (connect(sock_->sc, (sockaddr *)&to_addr, sizeof(sockaddr_in)) != 0) {
				std::cerr << u8"Client failed to create conncetion:"<< address +":"<<port<<" error code:" + std::to_string(CS_GetErrorCode) << std::endl;
				return false;
			}
			ser_ip_port_ = address + ":" + std::to_string(port);
			is_connect_ = true;

			recv_thread_ = std::thread(&TcpClient::RecvAndCallBack,this);
		}
		return false;
	}

	bool TcpClient::DisConnect()
	{
		return Close();
	}

	bool TcpClient::SetRecvCallBack(std::function<bool(const ByteStream&data)> recv_func)
	{
		recv_func_ = recv_func;
		return true;
	}

	bool TcpClient::Send(const ByteStream & data)
	{
		if (!is_connect_) {
			std::cerr << u8"The connection has not been established!"<< std::endl;
			return false;
		}
		int send_size = send(sock_->sc, data.c_str(), data.size(), 0);
		if (send_size < 0) {
			std::cerr << u8"Message sending faild, error code:" + std::to_string(CS_GetErrorCode) << std::endl;
			return false;
		}
		if (debug_)
		{
			std::string msg;
			msg += u8"Send to address: " + ser_ip_port_ + u8", size: " + std::to_string(send_size) + u8" byte";
			msg += u8"\n data:\n";
			msg += data.str();
			std::cout << msg << std::endl;
		}
		return true;
	}

	bool TcpClient::Close()
	{
		if (CloseSocket(*sock_)) {
			stop_recv_ = true;
			if (recv_thread_.joinable()) {
				recv_thread_.join();
			}
			sock_->effective = false;
			is_connect_ = false;
		}
		return false;
	}

	bool TcpClient::RecvAndCallBack()
	{
		ByteStream data;
		while (!stop_recv_) {
			auto rv = RecvFrom(ser_ip_port_,*sock_, data);
			if (rv == 0) {
				if (recv_func_) {
					if (!recv_func_(data)) {
						stop_recv_ = true;
						break;
					}
				}
			}
			else if(rv == 10054){
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return true;
	}
};

