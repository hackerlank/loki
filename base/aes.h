#pragma once

#include <openssl/aes.h>
#include <iostream>
#include <string.h>

namespace loki
{

class aes_crypto
{
public:
	aes_crypto()
	{
	}
	~aes_crypto()
	{
	}
	void setkey(const unsigned char* key, const size_t n)
	{
		if (0 != AES_set_encrypt_key((unsigned char*)key, n, &key_))
		{
			std::cout<<"set encrypt key error"<<std::endl;
		}
		if (0 != AES_set_decrypt_key((unsigned char*)key, n, &de_key_))
		{
			std::cout<<"set decrypt key error"<<std::endl;
		}
	}
	int encrypt(const unsigned char* in, const size_t in_len, unsigned char* out, size_t& out_len)
	{
		uint32_t offset = 0;
		while (offset <= in_len - AES_BLOCK_SIZE)
		{
			AES_encrypt((unsigned char*)in, (unsigned char*)out, &key_);
			in += AES_BLOCK_SIZE;
			out += AES_BLOCK_SIZE;
			offset += AES_BLOCK_SIZE;
		}
		//not enough 16 bytes, simply copy to out buffer
		if (offset < in_len)
		{
			memcpy(out, in, in_len - offset);
		}
		out_len = in_len;
		return 0;
	}
	int decrypt(const unsigned char* in, const size_t in_len, unsigned char* out, size_t& out_len)
	{
		uint32_t offset = 0;
		while (offset <= in_len - AES_BLOCK_SIZE)
		{
			AES_decrypt((unsigned char*)in, (unsigned char*)out, &de_key_);
			in += AES_BLOCK_SIZE;
			out += AES_BLOCK_SIZE;
			offset += AES_BLOCK_SIZE;
		}
		//not enough 16 bytes, simply copy to out buffer
		if (offset < in_len)
		{
			memcpy(out, in, in_len - offset);
		}
		out_len = in_len;
		return 0;
	}

	static bool test()
	{
		unsigned char txt[128 + 1];
		unsigned char back[128];
		memset(txt, 0, sizeof(txt));
		for (size_t i=0;i<sizeof(txt) - 1; ++i)
		{
			txt[i] = 'A' + rand()%20;
			back[i] = txt[i];
		}
		std::cout<<txt<<std::endl;
		aes_crypto aes;
		aes.setkey(txt, 256);
		//unsigned char enc[256] = {'\0'};
		unsigned char denc[256] = {'\0'};
		//原地加解密ok
		size_t nnn = 0;
		aes.encrypt(txt, 128, txt, nnn);
		std::cout<<txt<<std::endl;
		aes.decrypt(txt, 128, txt, nnn);
		if (0 == strncmp((const char*)txt, (const char*)back, 128))
		{
			std::cout<<"test ok"<<std::endl;
			return true;
		}
		else
		{
			std::cout<<"明文 " <<txt<<std::endl;
			std::cout<<"解密 " <<denc<<std::endl;
			std::cout<<"test failed"<<std::endl;
			return false;
		}
	}

private:
	AES_KEY key_;
	AES_KEY de_key_;
};

}
