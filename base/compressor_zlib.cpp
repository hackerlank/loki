#include "compressor_zlib.h"
#include <zlib.h>
#include "logger.h"

namespace loki
{

	bool compressor_zlib::compress(const void* msg, const size_t size, char* buff, size_t& len)
	{
		int retcode = ::compress((Bytef*)buff, (uLongf*)&len, (const Bytef*)msg, size);
		switch (retcode)
		{
			case Z_OK:
				break;
			case Z_MEM_ERROR:
				LOG(ERROR)<<("Z_MEM_ERROR");
				break;
			case Z_BUF_ERROR:
				LOG(ERROR)<<("Z_BUF_ERROR");
				break;
		}
		return (retcode == Z_OK);
	}
	bool compressor_zlib::uncompress(const void* msg, const size_t size, char* unzipbuf, size_t& len)
	{
		uLong unziplen = len;
		int retcode = ::uncompress((Bytef*)unzipbuf, &unziplen, (const Bytef*)msg, size);
		switch (retcode)
		{
			case Z_OK:
				{
					len = unziplen;
				}
				break;
			case Z_MEM_ERROR:
				LOG(ERROR)<<"Z_MEM_ERROR";
				break;
			case Z_BUF_ERROR:
				LOG(ERROR)<<"Z_BUF_ERROR";
				break;
			case Z_DATA_ERROR:
				LOG(ERROR)<<"Z_DATA_ERROR";
				break;
		}
		return (retcode == Z_OK);
	}
}
