#pragma once
//C++
#include <type_traits>
#include <string>
#include <vector>
//obj
#include "zsocket_export.h"

#if (defined(_MSC_VER) && _MSVC_LANG>=201703L) || \
	(defined(__cplusplus) && __cplusplus>=201703L)
typedef  std::byte udp_byte;
#else
typedef  unsigned char udp_byte;
#endif

namespace zsocket {
	class ZSOCKET_EXPORT ByteStream {
	public:
		ByteStream() = default;
		~ByteStream() {
			clear();
		}
		ByteStream(const ByteStream& from) {
			data_ = from.data_;
		}
		ByteStream(const char *buf, size_t size) {
			for (int i = 0; i < size; i++)
			{
				data_.push_back((udp_byte)buf[i]);
			}
		}
		ByteStream(const char *buf) {
			for (int i = 0; buf[i]!= '\0'; i++)
			{
				data_.push_back((udp_byte)buf[i]);
			}
		}

		ByteStream(const std::string &str) {
			for (const auto &it : str)
			{
				data_.push_back((udp_byte)it);
			}
		}
		template<class T>
		ByteStream(const T& t) {
			DataCopy(t, 0);
		}
		//重载自身<<
		ByteStream & operator<<(const ByteStream &byte) {
			for (const auto &it : byte.data_)
			{
				data_.push_back(it);
			}
			return *this;
		}
		//重载std::string <<
		ByteStream & operator<<(const std::string &str) {
			//data_.insert(data_.end(),str.begin(), str.end());
			for (const auto &it : str)
			{
				data_.push_back((udp_byte)it);
			}
			return *this;
		}
		//重载其他类型<<
		template<class T>
		ByteStream & operator<<(const T &t) {
			DataCopy(t, data_.size());
			return *this;
		}

		//重载自身>>
		const ByteStream & operator>>(ByteStream &byte) const {
			for (const auto &it : data_)
			{
				byte.data_.push_back(it);
			}
			return *this;
		}
		//重载std::string>>
		const ByteStream & operator>>(std::string &str) const {
			//data_.insert(data_.end(),str.begin(), str.end());
			for (const auto &it : data_)
			{
				str.push_back((char)it);
			}
			return *this;
		}
		//重载其他类型>>
		template<class T>
		const ByteStream & operator>>(T &t) const {
			auto size = data_.size() < sizeof(t)?  data_.size() :sizeof(t);
			memcpy(&t, data(), size);
			return *this;
		}
		void clear() {
			data_.clear();
		}
		void resize(size_t size) {
			data_.resize(size);
		}
		size_t size() const {
			return data_.size();
		}
		std::string str() const {
			return std::string((char *)data(), size());
		}
		char * c_str() const {
			return (char *)data();
		}
		const udp_byte *data() const {
			return data(0);
		}
		const udp_byte *data(const size_t &first_index) const {
			auto ptr = data_.data();
			return ptr + first_index;
		}
	private:
		template<class T>
		void DataCopy(T t, const size_t &first_index) {
			const auto &t_size = sizeof(typename std::decay<T>::type);
			//先扩容
			if (first_index + t_size > data_.size())
			{
				data_.resize(first_index + t_size);
			}
			if (1) {
				udp_byte *p = new udp_byte[t_size];
				memcpy(p, &t, t_size);
				for (int i = 0; i < t_size; i++)
				{
					data_[first_index + i] = (*(p + i));
				}
				delete[]p;
			}
			else {
				//memcpy(&data_[first_index], &t, t_size);
				memcpy(data_.data()+ first_index, &t, t_size);
			}
		}
	private:
		std::vector<udp_byte> data_;
	};

};