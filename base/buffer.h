#pragma once
#include <vector>
#include <boost/asio.hpp>
#include <cassert>
#include <cstring>

namespace loki
{

using namespace boost::asio;

class buffer
{
public:
	enum { big_size = 65536, limited_size = 1024 };
	explicit buffer(size_t size)
		: buff_(size)
	{
		read_offset_ = 0;
		write_offset_ = 0;
	}
	size_t data_size() const 
	{ 
		return write_offset_ - read_offset_; 
	}
	size_t free_size() const
	{
		return buff_.size() - write_offset_;
	}
	size_t capacity() const
	{
		return buff_.size();
	}
	void clear()
	{
		read_offset_ = 0;
		write_offset_ = 0;
	}

	const_buffer data() const
	{
		return boost::asio::buffer(buff_.data() + read_offset_, data_size());
	}
	const void* data_ptr() const
	{
		return (const void*)(buff_.data() + read_offset_);
	}
	//buffers not buffer
	mutable_buffer free_data()
	{
		make_space(limited_size);
		return boost::asio::buffer(buff_.data() + write_offset_, free_size());
	}
	void add_data_size(const size_t count)
	{
		assert(write_offset_ + count <= capacity());
		write_offset_ += count;
	}
	void append(const void* p, const size_t n)
	{
		make_space(n);
		memcpy(buff_.data() + write_offset_, p, n);
		write_offset_ += n;
	}

	void make_space(const size_t n)
	{
		if (free_size() < n)
		{
			buff_.resize(write_offset_ + n);
		}
	}
	void consume(size_t count)
	{
		if (count == 0) return;

		assert(read_offset_ + count <= write_offset_);
		read_offset_ += count;

		const size_t size = data_size();
		if (size > 0)
		{
			const size_t free = capacity() - size;
			if (free > big_size * 2)
			{
				//keep a reasonable buffer size
				size_t new_size = ((size + big_size - 1)/big_size + 1) * big_size;
				std::vector<char> tmp(new_size);
				memcpy(tmp.data(), data_ptr(), data_size());
				tmp.swap(buff_);
				read_offset_ = 0;
				write_offset_ = size;
			}
			else
			{
				memmove(buff_.data(), buff_.data() + read_offset_, size);
				read_offset_ = 0;
				write_offset_ = size;
			}
		}
		else
		{
			clear();
		}
	}
	bool empty() const
	{
		return read_offset_ == write_offset_;
	}

	template<class T>
	T get() const
	{
		if (data_size() < sizeof(T))
			return 0;
		const T n = *((T*)data_ptr());
		return n;
	}
public:
	std::vector<char> buff_;
	size_t read_offset_;
	size_t write_offset_;
};
}
