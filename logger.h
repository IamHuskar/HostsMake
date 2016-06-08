#ifndef  _LOGGER_H
#define  _LOGGER_H
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#pragma warning(disable:4018)
#endif
#include <string.h>
#include "time.h"
#include <stdio.h>

//���û����c++11�����mutex
//����lock.h����ƽ̨��ص�API

#include "lock.h"




/*
��־ʹ��

��MAIN �����ʼ��


������
1Ĭ�� ������ļ� ���ж����
LOGDEFAULTINIT

2�������ļ���ȡ
LOGINITFROMFILE(cfgfilename)


�����ļ����ݸ�ʽΪ
filename@loglevel@logtype
��Ҫ�ж�����ַ��Ϳո���
filename���ļ���
loglevelȡֵΪ  debug info warning error fatal
logtypeȡֵΪ tofile toconsole




3�̶���
loglevelȡֵΪ  LOGL_DEBUG LOGL_INFO LOGL_WARNING LOGL_ERROR LOGL_FATAL
logtypeȡֵΪ LOGTOFILE LOGTOCONSOLE

LOGINIT(logtype,loglevel,filename) p_g_log->LogInit((logtype),(loglevel),(filename));


*/









typedef enum{ LOGTOFILE, LOGTOCONSOLE } LOGTYPE;
typedef enum{ LOGL_DEBUG, LOGL_INFO, LOGL_WARNING, LOGL_ERROR, LOGL_FATAL } LOGLEVEL;
typedef void* PLOGGER;

#define DEFAULT_LOGFILENAME "vsmlog.log"




class SimpleLog
{
	//������̴߳�ӡ����̨��ʱ������⡣
	static Lock g_console_mutex;
public:
	/*
	 * ���ڹ��캯�������ʼ��·��Ҳ������һ����file�������ⲻ�ò���,
	 * ����������LogInit
	 */
	SimpleLog();
	~SimpleLog();

	bool LogInit(const char* filename="log.txt",LOGTYPE logtype=LOGTOCONSOLE,LOGLEVEL loglevel=LOGL_DEBUG);
	void LogShutdown();
	void SetLevel(LOGLEVEL loglevel);
//{LOGL_DEBUG,LOGL_INFO,LOGL_WARNING,LOGL_ERROR,LOGL_FATAL}
	bool logdebug(const char *msg, const char* srcfile = "", int srcline = -1);
	bool loginfo(const char *msg, const char* srcfile = "", int srcline = -1);
	bool logwarning(const char *msg, const char* srcfile = "", int srcline = -1);
	bool logerror(const char *msg, const char* srcfile = "", int srcline = -1);
	bool logfatal(const char *msg, const char* srcfile = "", int srcline = -1);
	bool logmsg(LOGLEVEL level, const char* msg, const char* srcfile = "", int srcline = -1);
	bool LogInitFromFile(char* cfgfilename);
private:
	Lock filemutex;
	void lock()
	{
		filemutex.lock();
	};
	void unlock()
	{
		filemutex.unlock();
	};

	bool   isinit;
	LOGTYPE  currenttype;
	LOGLEVEL currentlevel;
	FILE* fp;
};

extern SimpleLog* p_g_log_unique_hw;

#define LOGDEFAULTINIT  p_g_log_unique_hw->LogInit();
#define LOGINITFROMFILE(cfgfilename) p_g_log_unique_hw->LogInitFromFile((cfgfilename));
#define LOGINIT(logtype,loglevel,filename) p_g_log_unique_hw->LogInit((filename),(logtype),(loglevel));
#define LOGFATAL(x) p_g_log_unique_hw->logfatal((x),__FILE__,__LINE__);
#define LOGERROR(x) p_g_log_unique_hw->logerror((x),__FILE__,__LINE__);
#define LOGWARNING(x) p_g_log_unique_hw->logwarning((x),__FILE__,__LINE__);
#define LOGINFO(x) p_g_log_unique_hw->loginfo((x),__FILE__,__LINE__);
#define LOGDEBUG(x) p_g_log_unique_hw->logdebug((x),__FILE__,__LINE__);

#ifndef MAX_LOG_LEN
#define MAX_LOG_LEN 512
#endif
#endif
