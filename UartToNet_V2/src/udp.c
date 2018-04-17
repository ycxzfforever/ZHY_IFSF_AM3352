#include "common.h"


/********************************************************************\
* 函数名：: Deal_recv_Udp_data
* 说明:
* 功能:     获取FS 分配的节点地址，UDP发送给加气机
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-17
\*********************************************************************/
void Deal_recv_Udp_data()
{
    char tmp[10];
    Uint tmp_port;
    bzero(tmp,10);
    sprintf(tmp,"%02d.%02d",Udp_Recv_Buf[6],Udp_Recv_Buf[7]);
    if(strcmp(tmp,conf.IFSF_Node)!=0) //获取FS的节点
    {
        strcpy(conf.IFSF_Node,tmp);
        write_conf_value("IFSF_Node",conf.IFSF_Node,Conf_Name);
    }
    tmp_port=(Uint)(Udp_Recv_Buf[4])<<8|Udp_Recv_Buf[5];
    if(conf.FS_Port!=tmp_port)
    {
        conf.FS_Port=tmp_port;//获取FS服务器端口号
        sprintf(tmp,"%d",conf.FS_Port);
        write_conf_value("FS_Port",tmp,Conf_Name);
    }
}

/********************************************************************\
* 函数名：: Udp_Send_Buf_Process
* 说明:
* 功能:     UDP心跳数据包组合
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-10
\*********************************************************************/
void Udp_Send_Buf_Process() //192.168.10.10
{
    int i=0;
    Str_Split(conf.My_IP);
    //四字节IP地址
    //strcat(Udp_Send_Buf,Tmp_Buf);//若ip地址中有字段0则会连接失败
    for(i=0;i<4;i++)
	{	
		Udp_Send_Buf[i]=Tmp_Buf[i];
	} 
    //P_Log("%X %X %X %X\n",Tmp_Buf[0],Tmp_Buf[1],Tmp_Buf[2],Tmp_Buf[3]);
    //二字节端口号
    Udp_Send_Buf[4]=conf.My_FS_Port>>8;
    Udp_Send_Buf[5]=conf.My_FS_Port;
    //二字节逻辑节点地址
    if((conf.My_Node[0]==0)&&(conf.My_Node[1]==0))
    {
        Udp_Send_Buf[6]=0;
        Udp_Send_Buf[7]=0;
    }
    else
    {
        Str_Split(conf.My_Node);
        //strcat(Udp_Send_Buf,Tmp_Buf);
        for(i=0;i<2;i++)
    	{	
    		Udp_Send_Buf[6+i]=Tmp_Buf[i];
    	} 
    }
    Udp_Send_Buf[8]=0x01;                   //一字节消息代码
    Udp_Send_Buf[9]=para.ST;                //一字节状态
    //Udp_Send_Buf[10]='\0';
}

/********************************************************************\
* 函数名：: Udp_Server_Creat
* 说明:
* 功能:     UDP服务端创建
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-10
\*********************************************************************/
int Udp_Server_Creat()
{
    time_t nowtime,lasttime;
    int sockfd;                     // Socket file descriptor
    int num;
    int sin_size;                   // to store struct size
    //char revbuf[LENGTH];                  // Send buffer
    struct sockaddr_in addr_local;
    struct sockaddr_in addr_remote;

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        P_Log("ERROR:Udp_Server Failed to obtain Socket Despcritor.\n");
        return (0);
    }
    else
    {
        P_Log("OK:Udp_Server Obtain Socket Despcritor sucessfully.\n");
    }

    /* Fill the local socket address struct */
    addr_local.sin_family = AF_INET;                             // Protocol Family
    addr_local.sin_port = htons(conf.My_Broadcast_Port);         // Port number
    addr_local.sin_addr.s_addr  = INADDR_ANY;                    // AutoFill local address
    bzero(&(addr_local.sin_zero), 8);                            // Flush the rest of struct

    /*  Blind a special Port */
    if(bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1)
    {
        P_Log("ERROR:Udp_Server Failed to bind Port %d.\n",conf.My_Broadcast_Port);
        return (0);
    }
    else
    {
        P_Log("OK:Udp_Server Bind the Port %d sucessfully.\n",conf.My_Broadcast_Port);
    }

    sin_size = sizeof(struct sockaddr);
    time(&lasttime);
    while(1)
    {
        time(&nowtime);
        if(nowtime-lasttime>30) //30S之内未收到FS心跳，认为fs掉线
        {
            FS_OK=0;
            Cli_Connect_FS=0;
            Ser_Connect_FS=0;
            Reconnect_EPS_Flag=0;
            Reconnect_FS_Flag=0;
            Disconnect_EPS_Flag=1;
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
        if(num = recvfrom(sockfd, Udp_Recv_Buf, 10, 0, (struct sockaddr *)&addr_remote, &sin_size) == -1)
        {
            P_Log("Udp_Server recvfrom ERROR!\n");
        }
        else
        {
            if(strcmp(inet_ntoa(addr_remote.sin_addr),conf.FS_IP)==0) //接收到fs udp心跳
            {
                if((Cli_Connect_FS==0)&&(Reconnect_FS_Flag==0))         //如果未连接FS
                {
                    Reconnect_FS_Flag=1;    //那么连接FS
                    Deal_recv_Udp_data();   //获取FS分配的节点信息
                }
                P_Log("Udp_Server Recv From:%s\n",inet_ntoa(addr_remote.sin_addr));
                Print_HEX("Udp_Server Recv",10,Udp_Recv_Buf);
                lasttime=nowtime;
            }
            bzero(Udp_Recv_Buf,sizeof(Udp_Recv_Buf));
        }
    }
    close(sockfd);
    return (0);
}


/********************************************************************\
* 函数名：: Udp_Client_Creat
* 说明:
* 功能:     UDP客户端创建
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-10
\*********************************************************************/
int Udp_Client_Creat()
{
    time_t nowtime,lasttime;
    int sockfd;                        // Socket file descriptor
    int num;                           // Counter of received bytes
    //char sdbuf[LENGTH];                // Receive buffer
    struct sockaddr_in addr_remote;    // Host address information
    const char *argv=(const char *)conf.FS_IP;

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        P_Log("ERROR:Udp_Client Failed to obtain Socket Descriptor!\n");
        return (0);
    }
    else
    {
        Udp_fd=sockfd;
        P_Log("OK:Udp_Client Obtain Socket Despcritor sucessfully.\n");
    }

    /* Fill the socket address struct */
    addr_remote.sin_family = AF_INET;                   // Protocol Family
    addr_remote.sin_port = htons(conf.FS_Udp_Port);     // Port number
    num=inet_pton(AF_INET, argv, &addr_remote.sin_addr);    // Net Address
    if(num!=1)
    {
        P_Log("Udp inet_pton error!\n");
    }
    bzero(&(addr_remote.sin_zero), 8);                  // Flush the rest of struct
    time(&lasttime);
    /* Try to connect the server */
    while(1)
    {
        /*获取当前时间*/
        time(&nowtime);
        if((nowtime-lasttime>conf.Time_Out-1)&&(OPT_IS_Online>0))
        {
            bzero(Udp_Send_Buf,sizeof(Udp_Send_Buf));
            Udp_Send_Buf_Process();
#ifdef Debug
            P_Log("FS_OK=%d\n",FS_OK);
            P_Log("Cli_FS_fd=%d\n",Cli_FS_fd);
            P_Log("Ser_FS_fd=%d\n",Ser_FS_fd);
            P_Log("Cli_EPS_fd=%d\n",Cli_EPS_fd);
            P_Log("OPT_Server_OK=%d\n",OPT_Server_OK);
            P_Log("Cli_Connect_FS=%d\n",Cli_Connect_FS);
            P_Log("Ser_Connect_FS=%d\n",Ser_Connect_FS);
            P_Log("Is_Connect_EPS=%d\n",Is_Connect_EPS);
            P_Log("Reconnect_FS_Flag=%d\n",Reconnect_FS_Flag);
            P_Log("Reconnect_EPS_Flag=%d\n",Reconnect_EPS_Flag);
            P_Log("OPT_IS_Online=%d\n",OPT_IS_Online);
            P_Log("YanKaFlag=%d\n",YanKaFlag);
            P_Log("HuiSuoFlag=%d\n",HuiSuoFlag);
            P_Log("RequestPayFlag=%d\n",RequestPayFlag);
            P_Log("OverTransFlag=%d\n",OverTransFlag);
            P_Log("YanKaCount=%d\n",YanKaCount);
            P_Log("HuiSuoCount=%d\n",HuiSuoCount);
            P_Log("RequestPayCount=%d\n",RequestPayCount);
            P_Log("OverTransCount=%d\n",OverTransCount);
#endif
            num = sendto(sockfd, Udp_Send_Buf, 10, 0, (struct sockaddr *)&addr_remote, sizeof(struct sockaddr_in));
            if(num < 0)
            {
                P_Log("ERROR:Udp_Client_Creat Failed to send your data!\n", argv, num);
            }
            else
            {
                Print_HEX("Udp_Client Send",num,Udp_Send_Buf);
            }
            lasttime=nowtime;
        }
        usleep(10*1000);
    }
    close(sockfd);
    return (0);
}

/********************************************************************\
* 函数名：: UDP_Pthread_Creat
* 说明:
* 功能:     客户端UDP线程创建
* 输入:     无
* 返回值:   0:成功，非0:失败
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-10
\*********************************************************************/
int Udp_Pthread_Creat()
{
    int err;
    pthread_t Udp_Client,Udp_Server;
    err=pthread_create(&Udp_Client,NULL,(void*)Udp_Client_Creat,NULL);//创建接收线程
    if(err != 0)
    {
        P_Log("can't create Udp_Client_Creat thread: %s\n", strerror(err));
        return 1;
    }
    else
        P_Log("create Udp_Client_Creat thread OK\n");
    err=pthread_create(&Udp_Server,NULL,(void*)Udp_Server_Creat,NULL);//创建接收线程
    if(err != 0)
    {
        P_Log("can't create Udp_Server_Creat thread: %s\n", strerror(err));
        return 1;
    }
    else
        P_Log("create Udp_Server_Creat thread OK\n");

    return 0;
}

