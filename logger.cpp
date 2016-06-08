#include "logger.h"



SimpleLog  g_log_p_g_log_unique_hw;
SimpleLog* p_g_log_unique_hw=&g_log_p_g_log_unique_hw;
Lock SimpleLog::g_console_mutex;




SimpleLog::SimpleLog()
{
	currenttype=LOGTOCONSOLE;
	currentlevel=LOGL_DEBUG;
	isinit=false;
	fp=NULL;
}

SimpleLog::~SimpleLog()
{
	LogShutdown();
}

void SimpleLog::LogShutdown()
{
	lock();
	//打印到控制台是不能关闭的。
	if(currenttype==LOGTOFILE&&fp)
	{
		fflush(fp);
		fclose(fp);
	}
	fp=NULL;
	isinit=false;
	unlock();
}


bool SimpleLog::logdebug(const char *msg, const char* srcfile, int srcline)
{
	return logmsg(LOGL_DEBUG,msg,srcfile,srcline);
}

bool SimpleLog::loginfo(const char *msg, const char* srcfile, int srcline)
{
	return logmsg(LOGL_INFO, msg, srcfile, srcline);
}

bool SimpleLog::logwarning(const char *msg, const char* srcfile, int srcline)
{
	return logmsg(LOGL_WARNING, msg, srcfile, srcline);
}

bool SimpleLog::logerror(const char *msg, const char* srcfile, int srcline)
{
	return logmsg(LOGL_ERROR, msg, srcfile, srcline);
}

bool SimpleLog::logfatal(const char *msg, const char* srcfile, int srcline)
{
	return logmsg(LOGL_FATAL, msg, srcfile, srcline);
}

bool SimpleLog::logmsg(LOGLEVEL level, const char* msg, const char* srcfile, int srcline)
{
	bool bret=false;
	lock();
	do
	{

		if(!isinit)
		{
			break;
		}
		//级别问题
		if(level<currentlevel)
		{
			bret=true;
			break;
		}
		if(!fp)
		{
			break;
		}



		if(currenttype==LOGTOCONSOLE)
		{
			SimpleLog::g_console_mutex.lock();
		}


		/*
		时间这里可能出现的问题就先忽略了
		*/
		char tmpstr[256]={0};
		time_t pt;
		time(&pt);
		struct tm * timeinfo;
		timeinfo = localtime(&pt);
		sprintf(tmpstr, "[%d-%d-%d %d:%d:%d]", timeinfo->tm_year + 1900, \
			timeinfo->tm_mon + 1, \
			timeinfo->tm_mday, \
			timeinfo->tm_hour,\
			timeinfo->tm_min,\
			timeinfo->tm_sec);

		//fwrite(tmpstr, 1, strlen(tmpstr), fp);
		int flen = strlen(srcfile);
		if (flen)
		{
			/*
			有可能是全路径
			只需要后缀
			*/
			while (flen)
			{
				flen--;
				if (srcfile[flen]=='\\'||srcfile[flen]=='/')
				{
					flen++;
					break;
				}
			}
			sprintf(tmpstr, "[%s", &srcfile[flen]);
			fwrite(tmpstr, 1, strlen(tmpstr), fp);
		}
		if (srcline!=-1)
		{
			sprintf(tmpstr, " line:%d] ", srcline);
			fwrite(tmpstr, 1, strlen(tmpstr), fp);
		}


		const char* prefix="[DEFAULT]";
		switch(level)
		{
		case LOGL_DEBUG:
			prefix="[DEBUG] ";
			break;
		case LOGL_INFO:
			prefix="[INFO] ";
			break;
		case LOGL_WARNING:
			prefix="[WARNING] ";
			break;
		case LOGL_ERROR:
			prefix="[ERROR] ";
			break;
		case LOGL_FATAL:
			prefix="[FATAL] ";
			break;
		}
		fwrite(prefix,1,strlen(prefix),fp);
		fwrite(msg,1,strlen(msg),fp);

#ifdef _WIN32
		fwrite("\r\n",1,2,fp);
#else
		fwrite("\n",1,1,fp);
#endif
		fflush(fp);

		if(currenttype==LOGTOCONSOLE)
		{
			SimpleLog::g_console_mutex.unlock();
		}


		bret=true;

	}while(false);


	unlock();
	return bret;
}




bool _is_valid_char(char ch)
{
	

	if (ch>='0'&&ch<='9')
	{
		return true;
	}
	if (ch >= 'a'&&ch <= 'z')
	{
		return true;
	}
	if (ch >= 'A'&&ch <= 'Z')
	{
		return true;
	}

	if (ch=='_'||ch=='@'||ch=='.')
	{
		return true;
	}

	return false;
};
bool SimpleLog::LogInitFromFile(char* cfgfilename)
{
	bool cfgfileok=false;
	LOGTYPE logtype = LOGTOFILE;
	LOGLEVEL loglevel = LOGL_INFO;
	FILE* f = NULL;// fopen(cfgfilename);
	char cfgstr[100];
	memset(cfgstr, 0, 100);
	char* ptype = NULL;
	char* pfilename = NULL;
	char* plevel = NULL;
	
	do 
	{
		if (!cfgfilename)
		{
			break;
		}
		f = fopen(cfgfilename, "r");
		if (!f)
		{
			break;
		}
		size_t rdl = fread(cfgstr, 1, 80, f);
		if (rdl>=80)
		{
			break;
		}

		//第一个是文件名
		pfilename = cfgstr;
		for (int i = 0; i < rdl;i++)
		{
			if (!_is_valid_char(cfgstr[i]))break;
			if (cfgstr[i] == '@')
			{
				cfgstr[i] = 0;
				if (plevel==NULL)
				{
					plevel = &cfgstr[i+1];
				}
				else if (ptype==NULL)
				{
					ptype = &cfgstr[i + 1];
				}
			};

			if (cfgstr[i] >= 'A'&&cfgstr[i] <= 'Z')
				cfgstr[i] = cfgstr[i] + ('a' - 'A');
		}



		if (pfilename&&plevel&&ptype)
		{
		
			if (strcmp(ptype,"tofile")==0)
			{
				logtype= LOGTOFILE;
			}
			else if (strcmp(ptype, "toconsole") == 0)
			{
				logtype = LOGTOCONSOLE;
			}
			else
			{
				break;
			}
			
			if (strcmp(plevel,"debug")==0)
			{
				loglevel = LOGL_DEBUG;
			}
			else if (strcmp(plevel, "info") == 0)
			{
				loglevel = LOGL_INFO;
			}
			else if (strcmp(plevel, "warning") == 0)
			{
				loglevel = LOGL_WARNING;
			}
			else if (strcmp(plevel, "error") == 0)
			{
				loglevel = LOGL_ERROR;
			}
			else if (strcmp(plevel, "fatal") == 0)
			{
				loglevel = LOGL_FATAL;
			}
			else
			{
				break;
			}
			
			cfgfileok = true;
		}



	} while (false);

	if (cfgfileok)
	{
		return LogInit(pfilename, logtype, loglevel);
	}
	
	return LogInit();
};

bool SimpleLog::LogInit(const char* filename,LOGTYPE logtype,LOGLEVEL loglevel)
{
	bool bret=false;
	lock();
	do
	{
		if(isinit)
		{

			bret=true;
			break;
		}

		currenttype=logtype;
		currentlevel=loglevel;
		if(currenttype==LOGTOFILE)
		{
			if(!filename)
			{
				break;
			}
			fp=fopen(filename,"wb");
			if(!fp)
			{
				break;
			}
		}
		else
		{
			fp=stdout;
		}
		isinit=true;
		bret=true;
	}while(false);

	unlock();


	return bret;
}
void SimpleLog::SetLevel(LOGLEVEL loglevel)
{
	lock();
	currentlevel=loglevel;
	unlock();
}


/*
 * 封装的Ｃ接口客户端调用。如果需要
 *
 *
 *
 *
 */

PLOGGER  LoggerCreate(LOGTYPE logtype,LOGLEVEL loglevel,const char* logfilename)
{
	PLOGGER pret=NULL;
	SimpleLog* log=	new SimpleLog();
	if(log->LogInit(logfilename,logtype,loglevel))
	{
		pret=(PLOGGER)log;
	}
	else
	{
		delete log;
	}

	return pret;
}


void     LoggerDelete(PLOGGER p)
{
	SimpleLog* log=(SimpleLog*)p;
	if(log)
	{
		delete log;
	}
}

void     LoggerSetLevel(PLOGGER p,LOGLEVEL loglevel)
{
	SimpleLog* log=(SimpleLog*)p;
	if(log)
	{
		log->SetLevel(loglevel);
	}
}
void LoggerInternalLog(PLOGGER p,LOGLEVEL level,const char* msg);


void     LoggerLogDebug(PLOGGER p,const char* msg)
{
	LoggerInternalLog(p,LOGL_DEBUG,msg);
}

void     LoggerLogInfo(PLOGGER p,const char* msg)
{
	LoggerInternalLog(p,LOGL_INFO,msg);;
}

void     LoggerLogWarning(PLOGGER p,const char* msg)
{
	LoggerInternalLog(p,LOGL_WARNING,msg);
}

void     LoggerLogError(PLOGGER p,const char* msg)
{
	LoggerInternalLog(p,LOGL_ERROR,msg);
}

void     LoggerLogFatal(PLOGGER p,const char* msg)
{
	LoggerInternalLog(p,LOGL_FATAL,msg);
}

void LoggerInternalLog(PLOGGER p,LOGLEVEL level,const char* msg)
{
	SimpleLog* log=(SimpleLog*)p;
	if(log)
	{
		log->logmsg(level,msg);
	}
}
