#pragma once
#include <cstddef>
#include <utility>

namespace loki
{

class compressor
{
public:
	virtual bool compress(const void* msg, const size_t size, char* buff, size_t& len) = 0;
	virtual bool uncompress(const void* msg, const size_t size, char* buf, size_t& len) = 0;

	virtual ~compressor(){}
};

}
