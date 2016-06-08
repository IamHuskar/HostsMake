#include "lock.h"
#include "stdio.h"
Lock::Lock()
{
#ifdef _WIN32
	InitializeCriticalSection(&m_lock);
#else
	pthread_mutex_init(&m_lock, NULL);
#endif
}

Lock::~Lock()
{
#ifdef _WIN32
	DeleteCriticalSection(&m_lock);
#else
	pthread_mutex_destroy(&m_lock);
#endif 
}

void Lock::lock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_lock);
#else
	pthread_mutex_lock(&m_lock);
#endif
}

void Lock::unlock()
{
#ifdef _WIN32
	LeaveCriticalSection(&m_lock);
#else
	pthread_mutex_unlock(&m_lock);
#endif
}