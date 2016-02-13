#pragma once

#include <zlib.h>

namespace loki
{

	struct Package {
		enum PackageFlag
		{
			PackageZlib = 1<<31,      //压缩
			PackageEnc  = 1<<30,      //机密
		};
		static const uint32_t MsgNameLengthSize = 2;
		static const uint32_t MsgNameMaxSize = 128;

		static bool IsCompress(const uint32_t flag) { return flag & PackageZlib; }
		static bool IsEncrypt(const uint32_t flag) { return flag & PackageEnc; }
	};

	struct NetMisc 
	{
		static bool compress(const void* msg, const size_t size, char* buff, size_t& len)
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
		static bool uncompress(const void* msg, const size_t size, char* unzipbuf, size_t& len)
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
	};
};
