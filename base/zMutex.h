#pragma once
#include "mutex.h"
#include "zNoncopyable.h"

class zMutex 
{
	friend class zCond;
public:
	inline void lock() { mutex_.lock(); }
	inline void unlock() { mutex_.unlock(); }
private:
	std::mutex mutex_;
};

class zMutex_scope_lock : private zNoncopyable
{
public:
	zMutex_scope_lock(zMutex &m) : mlock(m)
	{
		mlock.lock();
	}
	~zMutex_scope_lock()
	{
		mlock.unlock();
	}

private:
	zMutex &mlock;
};

/**
* \brief 封装了系统条件变量，使用上要简单，省去了手工初始化和销毁系统条件变量的工作，这些工作都可以由构造函数和析构函数来自动完成
*
*/
class zCond : private zNoncopyable
{

public:
	zCond()
	{
		::pthread_cond_init(&cond, nullptr);
	}

	~zCond()
	{
		::pthread_cond_destroy(&cond);
	}

	void broadcast()
	{
		::pthread_cond_broadcast(&cond);
	}

	/**
	* \brief 对所有等待这个条件变量的线程发送信号，使这些线程能够继续往下执行
	*
	*/
	void signal()
	{
		::pthread_cond_signal(&cond);
	}

	/**
	* \brief 等待特定的条件变量满足
	*
	*
	* \param m_hMutex 需要等待的互斥体
	*/
	void wait(zMutex &mutex)
	{
		::pthread_cond_wait(&cond, mutex.mutex_.native_handle());
	}
private:
	pthread_cond_t cond;
};

class zRWLock : private zNoncopyable
{

public:
	zRWLock()
	{
	}

	/**
	* \brief 析构函数，用于销毁一个读写锁
	*
	*/
	~zRWLock()
	{
	}

	/**
	* \brief 对读写锁进行读加锁操作
	*
	*/
	inline void rdlock()
	{
		mutex_.lock();
	};

	/**
	* \brief 对读写锁进行写加锁操作
	*
	*/
	inline void wrlock()
	{
		mutex_.lock();
	}

	/**
	* \brief 对读写锁进行解锁操作
	*
	*/
	inline void unlock()
	{
		mutex_.unlock();
	}

private:
	std::mutex mutex_;
};

/**
* \brief rdlock Wrapper
* 方便在复杂函数中读写锁的使用
*/
class zRWLock_scope_rdlock : private zNoncopyable
{

public:

	/**
	* \brief 构造函数
	* 对锁进行rdlock操作
	* \param m 锁的引用
	*/
	zRWLock_scope_rdlock(zRWLock &m) : rwlock(m)
	{
		rwlock.rdlock();
	}

	/**
	* \brief 析购函数
	* 对锁进行unlock操作
	*/
	~zRWLock_scope_rdlock()
	{
		rwlock.unlock();
	}

private:

	/**
	* \brief 锁的引用
	*/
	zRWLock &rwlock;

};

/**
* \brief wrlock Wrapper
* 方便在复杂函数中读写锁的使用
*/
class zRWLock_scope_wrlock : private zNoncopyable
{

public:

	/**
	* \brief 构造函数
	* 对锁进行wrlock操作
	* \param m 锁的引用
	*/
	zRWLock_scope_wrlock(zRWLock &m) : rwlock(m)
	{
		rwlock.wrlock();
	}

	/**
	* \brief 析购函数
	* 对锁进行unlock操作
	*/
	~zRWLock_scope_wrlock()
	{
		rwlock.unlock();
	}

private:

	/**
	* \brief 锁的引用
	*/
	zRWLock &rwlock;

};
