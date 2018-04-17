#include "common.h"

void P_Log(const char *pszFmt,...) ;

void Lock(CRITICAL_SECTION *l)
{
    pthread_mutex_lock(l);
}
void Unlock(CRITICAL_SECTION *l)
{
    pthread_mutex_unlock(l);
}

void Deal_Log_Bak()
{
    char    file_name[100];
    sprintf(file_name,"/JKJN/log/RunLog%s%s.log",datestr,timestr);
    if(access(file_name,F_OK)==-1)  //文件不存在
    {
        if(rename(logfilename1,file_name))      //将logfilename1名称改为file_name
        {
            remove(file_name);                  //如果更名不成功直接删除file_name
            rename(logfilename1,file_name);     //然后再次更改名字
        }
    }
    system("deallogback");//保留40个文件 
}
void LogV(const char *pszFmt,va_list argp)
{
    ftime(&tb);
    now=localtime(&tb.time);
    if(NULL==pszFmt||0==pszFmt[0]) return;
    _vsnprintf(logstr,MAXLINSIZE,pszFmt,argp);
    sprintf(datestr,"%04d-%02d-%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
    sprintf(timestr,"%02d:%02d:%02d",now->tm_hour     ,now->tm_min  ,now->tm_sec);
    sprintf(mss,"%03d",tb.millitm);
    //printf("%s %s.%s %s",datestr,timestr,mss,logstr);
    flog=fopen(logfilename1,"a");
    if(NULL!=flog)
    {
        fprintf(flog,"%s %s.%s %s",datestr,timestr,mss,logstr);        
        if(ftell(flog)>MAXLOGSIZE)
        {
            fclose(flog);
            Deal_Log_Bak();
        }       
        else
            fclose(flog);
    }
}

void P_Log(const char *pszFmt,...)
{
    va_list argp;
    Lock(&cs_log);
    va_start(argp,pszFmt);
    LogV(pszFmt,argp);
    va_end(argp);
    Unlock(&cs_log);
    //pthread_mutex_destroy(&cs_log);
}
