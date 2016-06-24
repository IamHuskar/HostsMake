#ifndef _COMMON_LOCK_H_
#define _COMMON_LOCK_H_
#ifdef _WIN32
#include <winsock2.h>
#include <Windows.h>



#else
#include <pthread.h>
#endif

#ifdef _USRDLL
#define DLLEXPORT __declspec(dllexport)
#else   //LIBMSD_EXPORTS
#define DLLEXPORT
#endif //LIBMSD_EXPORTS


class DLLEXPORT Lock
{
public:
	Lock();
	~Lock();
	void lock();
	void unlock();
private:
#ifdef _WIN32
	CRITICAL_SECTION m_lock;
#else
	pthread_mutex_t m_lock;
#endif
};

#endif
