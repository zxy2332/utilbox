#include "udp_obj.h"

//C++
#include <iostream>

#ifdef WIN32 
//#include <WinSock2.h>
//#include <ws2tcpip.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#pragma comment(lib,"wsock32")
//#pragma comment(lib,"ws2_32")

#define Z_GetErrorCode (WSAGetLastError())
#define Z_REUSEPORT SO_REUSE_MULTICASTPORT
#define Z_SOCKET_SIZE int
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


namespace zsocket {
	struct zsocket::UdpObject::SocketType {
#ifdef WIN32 //如果定义了WINDOWS 
		SOCKET sc;
#else
		int sc;
#endif
		bool effective = false;
	};

	UdpObject::UdpObject()
	{
		sock_ = new SocketType();

#ifdef WIN32 //如果定义了WINDOWS 
		WSADATA wsaData;
		WSAStartup(MAKEWORD(1, 1), &wsaData); //windows下必须要初始化，不然socket会失败 
#endif
	}

	UdpObject::UdpObject(bool debug):UdpObject()
	{
		debug_ = debug;
	}

	UdpObject::~UdpObject()
	{
		Close();
		delete sock_;
	}

	bool UdpObject::CreateSocket()
	{
		if (sock_->effective == false)
		{
			//if ((sock_->sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
			if ((sock_->sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
			{
				std::cerr<<"socket creation failed, error code:" + std::to_string(Z_GetErrorCode)<<std::endl;
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
	bool UdpObject::Bind(int port, bool is_reuse)
	{
		if (CreateSocket()) {
			sockaddr_in listen_addr;
			listen_addr.sin_family = AF_INET;
			listen_addr.sin_port = htons(port);
			listen_addr.sin_addr.s_addr = INADDR_ANY;

			if (is_reuse) {
				//允许端口复用 则无法接收单播数据
				int opt = 1;
				setsockopt(sock_->sc, SOL_SOCKET, Z_REUSEPORT, (char*)&opt, sizeof(opt));
			}
			if (bind(sock_->sc, (sockaddr*)&listen_addr, sizeof(listen_addr)))
			{
				std::cerr << "port:" + std::to_string(port) + " binding error, error code:" + std::to_string(Z_GetErrorCode)<<std::endl;
				return false;
			}
			return true;
		}
		return false;
	}

	bool UdpObject::Bind(int port, const std::string &MultiCastGroup_addr)
	{
		if (CreateSocket()) {
			sockaddr_in listen_addr;
			listen_addr.sin_family = AF_INET;
			listen_addr.sin_port = htons(port);
			listen_addr.sin_addr.s_addr = INADDR_ANY;

			//允许本地地址复用
			int opt = 1;
			setsockopt(sock_->sc, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

			if (bind(sock_->sc, (sockaddr*)&listen_addr, sizeof(listen_addr)))
			{
				std::cerr << u8"port:" + std::to_string(port) + " binding error, error code:" + std::to_string(Z_GetErrorCode)<<std::endl;
				return false;
			}
			ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = inet_addr(MultiCastGroup_addr.c_str());  //组播组的ip地址  
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);       //加入的客户端主机的ip地址  INADDR_ANY为0.0.0.0,泛指本机,表示本机所有的ip.

			//客户端只有在加入多播组后才能接受多播组的数据
			if (setsockopt(sock_->sc, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0)   //IP_ADD_MEMBERSHIP:加入组播组
			{
				std::cerr << u8"Failed to join multicast address:" + MultiCastGroup_addr + " ,error code:" + std::to_string(Z_GetErrorCode) << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}

	bool UdpObject::Listen(int port, std::function<bool(const ByteStream &data)> recv_func, bool is_reuse)
	{
		if (Bind(port, is_reuse)) {
			listen_thread_ = std::thread(&UdpObject::RecvAndCallBack,this,recv_func);
			return true;
		}
		return false;
	}

	bool UdpObject::ListenBlock(int port, std::function<bool(const ByteStream &data)> recv_func, bool is_reuse)
	{
		if (Bind(port, is_reuse)) {
			RecvAndCallBack(recv_func);
			return true;
		}
		return false;
	}

	bool UdpObject::RecvAndCallBack(std::function<bool(const ByteStream &data)> recv_func /*= nullptr*/)
	{
		is_listen_ = true;
		ByteStream data;
		while (is_listen_) {
            if (RecvFrom(data))
			{
                if(is_listen_ == false){
                    break;
                }
				if (recv_func)
				{
					if (!recv_func(data)) {
						is_listen_ = false;
						break;
					}
				}
			}
            if(is_listen_ == false){
                break;
            }
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return true;
	}
	bool UdpObject::RecvFrom(ByteStream &data)
	{
		data.clear();
		data.resize(recv_size_);
		sockaddr_in from_addr;
		memset(&from_addr, 0, sizeof(from_addr));
        Z_SOCKET_SIZE addr_size = sizeof(from_addr);
#ifdef linux
        fd_set rfd;		// 读描述符集
        FD_ZERO(&rfd); // 在使用之前总是要清空
        // 开始使用select
        FD_SET(sock_->sc, &rfd); // 把socka放入要测试的描述符集中

        // 设置超时时间
        timeval timeout;	// 定时变量
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        auto nRet = select(sock_->sc+1, &rfd, NULL, NULL, &timeout);// 检测是否有套接口是否可读
        if (nRet == SO_ERROR)
        {
            std::cerr << u8"Select Error, error code:" + std::to_string(CS_GetErrorCode) << std::endl;
            return false;
        }
        else if (nRet == 0)	// 超时
        {
            //std::cerr << u8"Select Timeout"<< std::endl;
            return false;
        }
        // 检测到有套接口可读
#endif
		int recv_size = recvfrom(sock_->sc, data.c_str(), data.size(), 0, (sockaddr*)&from_addr, &addr_size);
		if (recv_size < 0) {
			std::cerr << u8"Data receiving failed, error code:" + std::to_string(Z_GetErrorCode) << std::endl;
			return false;
		}
		data.resize(recv_size);
		if (debug_)
		{
			std::string msg;
			msg += u8"Recv address:" + std::string(inet_ntoa(from_addr.sin_addr)) + ":" + std::to_string(ntohs(from_addr.sin_port));
			msg += u8" ,size:" + std::to_string(recv_size) + u8" byte";
			msg += u8"\n data:\n";
			msg += data.str();
			std::cout<<msg<<std::endl;
		}
		return true;

	}

	bool UdpObject::SendTo(const std::string address, const int port, const ByteStream &data)
	{
		if (CreateSocket()) {
			sockaddr_in to_addr;
			memset(&to_addr, 0, sizeof(to_addr));
			to_addr.sin_family = AF_INET;
			to_addr.sin_port = htons(port);
			to_addr.sin_addr.s_addr = inet_addr(address.c_str());
			int send_size = sendto(sock_->sc, data.c_str(), data.size(), 0, (sockaddr*)&to_addr, sizeof(to_addr));
			if (send_size < 0) {
				return false;
			}			
			if (debug_)
			{
				std::string msg;
				msg += u8"Send to address:" + address + ":" + std::to_string(port) + u8" ,size :" + std::to_string(send_size) + u8" byte";
				msg += u8"\n data:\n";
				msg += data.str();
				std::cout << msg << std::endl;
			}
			return true;
		}
		return false;
	}

	bool UdpObject::SendTo(const int port, const ByteStream &data)
	{
		if (CreateSocket()) {
			bool bOpt = true;
			//设置该套接字为广播类型
			setsockopt(sock_->sc, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));
			sockaddr_in to_addr;
			memset(&to_addr, 0, sizeof(to_addr));
			to_addr.sin_family = AF_INET;
			to_addr.sin_port = htons(port);
			to_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			int send_size = sendto(sock_->sc, data.c_str(), data.size(), 0, (sockaddr*)&to_addr, sizeof(to_addr));
			if (send_size < 0) {
				std::cerr << u8"Message sending faild, error code:"+std::to_string(Z_GetErrorCode) << std::endl;
				return false;
			}
			if (debug_)
			{
				std::string msg;
				msg += u8"Send to port" + std::to_string(port) + u8", size: " + std::to_string(send_size) + u8" byte";
				msg += u8"\n data:\n";
				msg += data.str();
				std::cout << msg << std::endl;
			}
			return true;
		}
		return false;
	}

	bool UdpObject::Close()
	{
		if (sock_->effective == true)
		{
            is_listen_ = false;
#ifdef WIN32
			if (closesocket(sock_->sc) != 0)
#else
            if(close(sock_->sc))
#endif
			{
				std::cerr << u8"Socket closing faild, error code:" + std::to_string(Z_GetErrorCode) << std::endl;
				return false;
			}
			if (listen_thread_.joinable())
			{
				listen_thread_.join();
			}
			sock_->effective = false;
		}
		return true;
	}

	
	bool UdpObject::StopListen()
	{
		return Close();
	}

	void UdpObject::SetDebug(bool debug)
	{
		debug_ = debug;
	}

	void UdpObject::SetRecvBuffSize(size_t size)
	{
		recv_size_ = size;
	}

	bool UdpObject::ListenMultiCast(int port, const std::string MultiCastGroup_addr, std::function<bool(const ByteStream &data)> recv_func /*= nullptr*/)
	{
		if (Bind(port, MultiCastGroup_addr)) {
			listen_thread_ = std::thread(&UdpObject::RecvAndCallBack, this, recv_func);
			return true;
		}
		return false;
	}

	bool UdpObject::ListenMultiCastBlock(int port, const std::string MultiCastGroup_addr, std::function<bool(const ByteStream &data)> recv_func /*= nullptr*/)
	{
		if (Bind(port, MultiCastGroup_addr)) {
			RecvAndCallBack(recv_func);
			return true;
		}
		return false;
	}

};

