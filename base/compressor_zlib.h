#pragma once

#include "compressor.h"
#include <utility>

namespace loki
{
	class compressor_zlib: public compressor
	{
	public:
		bool compress(const void* msg, const size_t size, char* buff, size_t& len);
		bool uncompress(const void* msg, const size_t size, char* buf, size_t& len);
	};
}
