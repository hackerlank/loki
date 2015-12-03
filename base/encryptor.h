#pragma once
#include <cstddef>

namespace loki
{
class encryptor
{
public:
	virtual bool encrypt(const void* ptr, const size_t size, void* buf, size_t& len) = 0;
	virtual bool decrypt(const void* ptr, const size_t size, void* buf, size_t& len) = 0;
	virtual ~encryptor() {}
};
}
