#ifndef _LOG_H
#define _LOG_H


#define     CRITICAL_SECTION    pthread_mutex_t
#define     _vsnprintf          vsnprintf
#define     MAXLOGSIZE          4*1024*1024     //log¥Û–°‘º4M
#define     MAXLINSIZE          1024

#define     logfilename1        "/JKJN/RunLog.log"
static char logstr[MAXLINSIZE+1];
char        datestr[16];
char        timestr[16];
char        mss[4];
FILE        *flog;

void P_Log(const char *pszFmt,...) ;
void Lock(CRITICAL_SECTION *l);
void Unlock(CRITICAL_SECTION *l) ;
void Deal_Log_Bak();
void LogV(const char *pszFmt,va_list argp) ;



#endif

