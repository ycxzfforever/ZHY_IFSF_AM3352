#include "common.h"

#define CLOCKID CLOCK_REALTIME


void ShutDownServer()
{
    FS_OK=0;
    Cli_Connect_FS=0;
    Ser_Connect_FS=0;
    Reconnect_EPS_Flag=0;
    Reconnect_FS_Flag=0;
    Disconnect_EPS_Flag=1;
    if(Cli_FS_fd>0)
    {
        close(Cli_FS_fd);
        Cli_FS_fd=-1;
    }
    if(Ser_FS_fd>0)
    {
        close(Ser_FS_fd);
        Ser_FS_fd=-1;
    }
    if(Cli_EPS_fd>0)
    {
        close(Cli_EPS_fd);
        Cli_EPS_fd=-1;
    }
}

/********************************************************************\
* ������: timer_thread
* ˵��:
* ����:    ��ʱ������
* ����:
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-9-12
\*********************************************************************/
void timer_thread(union sigval v)
{
    static Uchar st=3;
    Time_100ms++;
    F0_Time++;
    
    if(((Time_S%conf.Time_Reconnect)==0)&&(Cli_Connect_FS==0)&&(FS_OK==1)&&(Reconnect_FS_Flag==0))
    {
        Reconnect_FS_Flag=1;
    }

    if(OPT_IS_Online==0)
    {
        P_Log("===============Shutdown Server!=============\n");
        ShutDownServer();
    }

    if(Time_100ms%10==0)
    {
        Time_S++;
        Check_Time++;
        if(FaQiYanKaFlag==1)
        {
            FaQiYanKaTime++;
            if(FaQiYanKaTime>YANKATIMEOUT)
            {
                FaQiYanKaFlag=0;
                FaQiYanKaTime=0;
            }
        }
        if(YanKaFlag==1)
        {
            YanKaCount++;
            if(YanKaCount>30)
            {
                YanKaFlag=0;
                YanKaCount=0;
            }
        }
        else
        {
            YanKaCount=0;
        }
        
        if(HuiSuoFlag==1)
        {
            HuiSuoCount++;
            if(HuiSuoCount>20)
            {
                HuiSuoFlag=0;
                HuiSuoCount=0;
            }
        }
        else
        {
            HuiSuoCount=0;
        }
        
        if(RequestPayFlag==1)
        {
            RequestPayCount++;
            if(RequestPayCount>20)
            {
                RequestPayFlag=0;
                RequestPayCount=0;
            }
        }
        else
        {
            RequestPayCount=0;
        }
        
        if(OverTransFlag==1)
        {
            OverTransCount++;
            if(OverTransCount>20)
            {
                OverTransFlag=0;
                OverTransCount=0;
            }
        }
        else
        {
            OverTransCount=0;
        }
        
        if(StartFuelling==9)
        {
            StartFuelling=0;
            st=3;
            //Reply_FS_UploadStatus(3); 
        }

        if((st!=para.ST)&&(para.ST!=0)&&((StartFuelling==0))) //״̬�ı䣬�����ϴ�
        {
            st=para.ST;
            Reply_FS_UploadStatus(para.ST);
        }
  
        if(Check_Netlink("eth0") == -1) //����δ���Ӻ�
        {
            Stop_Opt=1;
            if(Check_Time>30)
                ShutDownServer();
        }
        else //��������
        {
            Check_Time=0;
            Stop_Opt=0;
            if((Ser_Connect_FS==1)&&(Cli_Connect_FS==0)&&(Cli_FS_fd<1))
            {
                FS_Tcp_Client_Creat();
            }
        } 
    }
    if(F0_Time%8==0)
    {
        if(OPT_72_Flag==1)
        {
            OPT_72_Flag=0;
            Lock(&Data_To_Uart);
            FCS_To_JKOPT_72();
            Unlock(&Data_To_Uart);
        }
        else
        {
            if((RecvFSAuth==1)&&(HaveAuth==0))
            {
                Lock(&Data_To_Uart);
                FCS_To_JKOPT_70(Auth_Type, HEX_BCD);
                Unlock(&Data_To_Uart);
            }
            
            if((CmdFlag!=0x70)&&(CmdFlag!=0x71))
            {
                Lock(&Data_To_Uart);
                FCS_To_JKOPT_51();
                Unlock(&Data_To_Uart);
            }
            else
            {
                CmdFlag=0;
            }
        }
    }
    if((Reconnect_EPS_Flag==1)&&(Check_Netlink("eth0")==0))
    {
        EPS_Tcp_Client_Creat();
    }

    if(Time_S==8000)
    {
        Time_S=0;
    }
    if(Time_100ms==80000)
    {
        Time_100ms=0;
        F0_Time=1;
    }
}

void Creat_Timer()
{
    // XXX int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);
    // clockid--ֵ��CLOCK_REALTIME,CLOCK_MONOTONIC��CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
    // evp--��Ż���ֵ�ĵ�ַ,�ṹ��Ա˵���˶�ʱ�����ڵ�֪ͨ��ʽ�ʹ���ʽ��
    // timerid--��ʱ����ʶ��
    timer_t timerid;
    struct sigevent evp;
    memset(&evp, 0, sizeof(struct sigevent));   //�����ʼ��

    evp.sigev_value.sival_int = 111;            //Ҳ�Ǳ�ʶ��ʱ���ģ����timerid��ʲô���𣿻ص��������Ի��
    evp.sigev_notify = SIGEV_THREAD;            //�߳�֪ͨ�ķ�ʽ����פ���߳�
    evp.sigev_notify_function = timer_thread;   //�̺߳�����ַ

    if(timer_create(CLOCKID, &evp, &timerid) == -1)
    {
        perror("fail to timer_create");
        exit(-1);
    }

    // XXX int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value,struct itimerspec *old_value);
    // timerid--��ʱ����ʶ
    // flags--0��ʾ���ʱ�䣬1��ʾ����ʱ��
    // new_value--��ʱ�����³�ʼֵ�ͼ�����������it
    // old_value--ȡֵͨ��Ϊ0�������ĸ�������ΪNULL,����ΪNULL���򷵻ض�ʱ����ǰһ��ֵ

    //��һ�μ��it.it_value��ô��,�Ժ�ÿ�ζ���it.it_interval��ô��,����˵it.it_value��0��ʱ���װ��it.it_interval��ֵ
    struct itimerspec it;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_nsec = 100000000;//100ms
    it.it_value.tv_sec = 1;
    it.it_value.tv_nsec = 0;

    if(timer_settime(timerid, 0, &it, NULL) == -1)
    {
        perror("fail to timer_settime");
        exit(-1);
    }
    pause();
}
/*
 * int timer_gettime(timer_t timerid, struct itimerspec *curr_value);
 * ��ȡtimeridָ���Ķ�ʱ����ֵ������curr_value
 *
 */
