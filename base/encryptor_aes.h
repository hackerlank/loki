#pragma once
#include "encryptor.h"
#include "aes.h"

namespace loki
{
class encryptor_aes : public encryptor
{
public:
	void setkey(const unsigned char* key, const size_t n);
	virtual bool encrypt(const void* ptr, const size_t size, void* buf, size_t& len);
	virtual bool decrypt(const void* ptr, const size_t size, void* buf, size_t& len);
private:
	aes_crypto aes_;
};
}
