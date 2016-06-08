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

//如果没有用c++11里面的mutex
//换用lock.h里面平台相关的API

#include "lock.h"




/*
日志使用

在MAIN 里面初始化


有三种
1默认 输出到文件 所有都输出
LOGDEFAULTINIT

2从配置文件获取
LOGINITFROMFILE(cfgfilename)


配置文件内容格式为
filename@loglevel@logtype
不要有多余的字符和空格换行
filename是文件名
loglevel取值为  debug info warning error fatal
logtype取值为 tofile toconsole




3固定死
loglevel取值为  LOGL_DEBUG LOGL_INFO LOGL_WARNING LOGL_ERROR LOGL_FATAL
logtype取值为 LOGTOFILE LOGTOCONSOLE

LOGINIT(logtype,loglevel,filename) p_g_log->LogInit((logtype),(loglevel),(filename));


*/









typedef enum{ LOGTOFILE, LOGTOCONSOLE } LOGTYPE;
typedef enum{ LOGL_DEBUG, LOGL_INFO, LOGL_WARNING, LOGL_ERROR, LOGL_FATAL } LOGLEVEL;
typedef void* PLOGGER;

#define DEFAULT_LOGFILENAME "vsmlog.log"




class SimpleLog
{
	//避免多线程打印控制台的时候出问题。
	static Lock g_console_mutex;
public:
	/*
	 * 不在构造函数里面初始化路径也是怕万一操作file出了问题不好捕获,
	 * 所以这里用LogInit
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
