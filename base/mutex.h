#pragma once
#include <mutex>

namespace loki
{

struct scoped_lock
{
	scoped_lock(std::mutex& m):mutex_(m) { mutex_.lock(); }
	~scoped_lock() { mutex_.unlock(); }

	std::mutex& mutex_;
};

}
