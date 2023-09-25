#pragma once
//C++
#include <functional>
#include <thread>
//obj
#include "ByteStream.hpp"
#include "zsocket_export.h"

namespace zsocket {
	class ZSOCKET_EXPORT UdpObject {
	public:
		UdpObject();
		//debug=true 输出调试信息
		UdpObject(bool debug);
		~UdpObject();


		/* 
		 * 监听组播数据，监听端口号，并将自身ip加入组播组
		 * port					监听端口号
		 * MultiCastGroup_addr	组播地址
		 * recv_func			接收回调函数
		 * 返回值	bool		true 继续监听 false 停止监听
		 * 创建线程后立即返回，回调函数返回false,调用stop 调用close 均可停止监听，注意线程安全
		 */
		bool ListenMultiCast(int port,const std::string MultiCastGroup_addr, std::function<bool(const ByteStream &data)> recv_func = nullptr);
		/* 
		 * 监听组播数据，监听端口号，并将自身ip加入组播组
		 * port					监听端口号
		 * MultiCastGroup_addr	组播地址
		 * recv_func			接收回调函数
		 * 返回值	bool		true 继续监听 false 停止监听
		 * 创建线程后阻塞，回调函数返回false,调用stop 调用close 均可停止监听，注意线程安全	
		 */
		bool ListenMultiCastBlock(int port,const std::string MultiCastGroup_addr, std::function<bool(const ByteStream &data)> recv_func = nullptr);

		/* 
		 * 监听端口数据
		 * port					监听端口号
		 * recv_func			接收回调函数
		 * is_reuse				是否启用端口复用
		 * 返回值	bool		true 继续监听 false 停止监听
		 * 创建线程后阻塞，回调函数返回false,调用stop 调用close 均可停止监听，注意线程安全
		 */
		bool ListenBlock(int port,std::function<bool(const ByteStream &data)> recv_func = nullptr, bool is_reuse = false);
		/* 
		 * 监听端口数据
		 * port					监听端口号
		 * recv_func			接收回调函数
		 * is_reuse				是否启用端口复用
		 * 返回值	bool		true 继续监听 false 停止监听
		 * 创建线程后立即返回，回调函数返回false,调用stop 调用close 均可停止监听，注意线程安全
		 */
		bool Listen(int port, std::function<bool(const ByteStream &data)> recv_func = nullptr, bool is_reuse = false);
		/* 
		 * 停止监听
		 * 返回值	bool		true 成功 false 失败
		 * 该操作会调用close
		 */
		bool StopListen();
		/* 
		 * 向ip:port发送数据
		 * address			目标地址
		 * port				目标端口号
		 * data				要发送的数据
		 * 返回值	bool	true 成功 false 失败
		 */
		bool SendTo(const std::string address, const int port, const  ByteStream &data);
		/*
		 * 向端口号广播数据
		 * port				目标端口号
		 * data				要发送的数据
		 * 返回值	bool	true 成功 false 失败
		 */
		bool SendTo(const int port, const  ByteStream &data);
		/*
		 * 关闭socket
		 * 返回值	bool	true 成功 false 失败		 * 
		 */
		bool Close();

		//设置输出调试信息
		//debug=true 输出调试信息
		void SetDebug(bool debug);
		//设置接收缓冲区大小 Default = 1024
		void SetRecvBuffSize(size_t size);
	private:	
		//绑定端口 并将自身ip加入组播组
		bool Bind(int port,const std::string &MultiCastGroup_addr);
		//绑定端口 is_reuse 是否复用(复用无法接收单播数据)
		bool Bind(int port,bool is_reuse);
		//创建socket
		bool CreateSocket();
		//接收数据并调用回调函数
		bool RecvAndCallBack(std::function<bool(const ByteStream &data)> recv_func = nullptr);
		//接收数据
		bool RecvFrom(ByteStream &data);
	private:
		bool is_listen_ = false;
		std::thread listen_thread_;
		struct SocketType;
		SocketType *sock_;
		bool debug_ = false;
		size_t recv_size_ = 1024;
	};
};
