#include "encryptor_aes.h"

namespace loki
{

void encryptor_aes::setkey(const unsigned char* key, const size_t n)
{
	aes_.setkey(key, n);
}

bool encryptor_aes::encrypt(const void* ptr, const size_t size, void* buf, size_t& len)
{
	aes_.encrypt((const unsigned char*)ptr, size, (unsigned char*)buf, len);
	return true;
}

bool encryptor_aes::decrypt(const void* ptr, const size_t size, void* buf, size_t& len)
{
	aes_.decrypt((const unsigned char*)ptr, size, (unsigned char*)buf, len);
	return true;
}

}
