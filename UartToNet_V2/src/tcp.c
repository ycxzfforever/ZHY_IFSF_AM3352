#include "common.h"



/********************************************************************\
* ∫Ø ˝√˚£∫: Cli_FS_To_Uart_Thread
* Àµ√˜:
* π¶ƒ‹:     FS∑¢ÀÕ∏¯º”∆¯ª˙µƒ ˝æ›¥¶¿Ì
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
void *Cli_FS_To_Uart_Thread(void *arg)
{
    int num;
    /* Try to connect the server */
    while(1)       // Check remoter command
    {
        num = recv(Cli_FS_fd, FS_To_Uart, LENGTH, 0);
        switch(num)
        {
            case -1:
                P_Log("ERROR:Cli_FS_To_Uart_Thread Receive string error!\n");
                Cli_Connect_FS=0;
                break;
            case  0:
                Cli_Connect_FS=0;
                break;
            default:
                P_Log("OK:Cli_FS_To_Uart_Thread Receviced numbytes = %d\n", num);
                break;
        }
        if(Cli_Connect_FS==0)
        {
            close(Cli_FS_fd);
            Cli_FS_fd=-1;
            return(0);
        }
    }
    close(Cli_FS_fd);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: Cli_Uart_To_FS_Thread
* Àµ√˜:
* π¶ƒ‹:     º”∆¯ª˙∑¢ÀÕ∏¯FSµƒ ˝æ›¥¶¿Ì
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
void *Cli_Uart_To_FS_Thread(void *arg)
{
    int num;
    while(1)
    {
        if(Uart_To_FS_Flag==1)
        {
            if((num = send(Cli_FS_fd, Uart_To_FS, FS_Data_Len, 0)) == -1)
            {
                P_Log("ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
                Cli_Connect_FS=0;
            }
            else
            {
                Uart_To_FS_Flag=0;
                Print_HEX("Uart_To_FS Send>>>>>>>",num,Uart_To_FS);
                bzero(Uart_To_FS,LENGTH);
            }
        }
        if(Cli_Connect_FS==0)
        {
            close(Cli_FS_fd);
            Cli_FS_fd=-1;
            return(0);
        }
        usleep(100*1000);
    }
    close(Cli_FS_fd);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: Cli_EPS_To_Uart_Thread
* Àµ√˜:
* π¶ƒ‹:    EPS∑¢ÀÕ∏¯ º”∆¯ª˙µƒ ˝æ›¥¶¿Ì
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
void *Cli_EPS_To_Uart_Thread(void *arg)
{
    int num;
    /* Try to connect the server */
    while(1)       // Check remoter command
    {
        bzero(EPS_To_Uart,LENGTH);
        num = recv(Cli_EPS_fd, EPS_To_Uart, LENGTH, 0);
        switch(num)
        {
            case -1:
                P_Log("ERROR:Cli_EPS_To_Uart_Thread Receive string error!\n");
                Disconnect_EPS_Flag=1;
                break;
            case  0:
                P_Log("Info:EPS_Server close the socket!\n");
                Disconnect_EPS_Flag=1;
                break;
            default:
                P_Log("OK:Cli_EPS_To_Uart_Thread Receviced numbytes = %d\n", num);
                EPS_Data_Len=num;
                Lock(&Data_To_Uart);
                Print_HEX("Cli_EPS_To_Uart_Thread Recv",num,EPS_To_Uart);
                Deal_EPS_To_Uart_data();
                Unlock(&Data_To_Uart);
                break;
        }
        if(Disconnect_EPS_Flag==1)
        {
            Disconnect_EPS_Flag=0;
            Is_Connect_EPS=0;
            close(Cli_EPS_fd);           
            Cli_EPS_fd=-1;
            return(0);
        }
    }
    close(Cli_EPS_fd);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: Cli_Uart_To_EPS_Thread
* Àµ√˜:
* π¶ƒ‹:     º”∆¯ª˙∑¢ÀÕ∏¯EPSµƒ ˝æ›¥¶¿Ì
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
void *Cli_Uart_To_EPS_Thread(void *arg)
{
    int num;
    while(1)
    {
        if((Uart_To_EPS_Flag==1)&&(Cli_EPS_fd>0))
        {
            Uart_To_EPS_Flag=0;
            if((num = send(Cli_EPS_fd, Uart_To_EPS, EPS_Data_Len, 0)) == -1)
            {
                P_Log("ERROR:Cli_Uart_To_EPS_Thread Failed to sent string.\n");
                Disconnect_EPS_Flag=1;
            }
            Print_HEX("Uart_To_EPS send",num,Uart_To_EPS);
            bzero(Uart_To_EPS,LENGTH);
        }
        if(Disconnect_EPS_Flag==1)
        {
            Disconnect_EPS_Flag=0;
            Is_Connect_EPS=0;
            close(Cli_EPS_fd);
            Cli_EPS_fd=-1;
            return(0);
        }
        if(Cli_EPS_fd<0) return 0;//∑˛ŒÒ∆˜∂Àπÿ±’¡¨Ω”∫Û£¨–Ë“™ ÷∂ØÕÀ≥ˆ
        usleep(100*1000);
    }
    close(Cli_EPS_fd);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: EPS_Tcp_Client_Creat
* Àµ√˜:
* π¶ƒ‹:     EPSøÕªß∂À¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int EPS_Tcp_Client_Creat()
{
    const char *argv=(const char *)conf.EPS_IP;
    int sockfd,err;                        // Socket file descriptor
    pthread_t Cli_Send,Cli_Receive;
    pthread_attr_t attr;//œﬂ≥Ã Ù–‘
    struct sockaddr_in remote_addr;    // Host address information

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log("ERROR:EPS_Tcp_Client_Creat Failed to obtain Socket Descriptor!\n");
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET;                       // Protocol Family
    remote_addr.sin_port = htons(conf.EPS_Port);            // Port number
    err=inet_pton(AF_INET, argv, &remote_addr.sin_addr);    // Net Address
    if(err <= 0)
    {
        if(err == 0)
            fprintf(stderr, "EPS_Tcp_Client_Creat Not in presentation format\n");
        else
            perror("EPS_Tcp_Client_Creat inet_pton error\n");
        return (0);
    }
    bzero(&(remote_addr.sin_zero), 8);                  // Flush the rest of struct

    /* Try to connect the remote */
    if(connect(sockfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1)
    {
        EPS_Error++;
        if(EPS_Error>5){
            Reconnect_EPS_Flag=0;
        }
        P_Log("ERROR:EPS_Tcp_Client_Creat Failed to connect to the host!\n");
        sleep(5);
        return (0);
    }
    else
    {
        EPS_Error=0;
        P_Log("OK:EPS_Tcp_Client_Creat Have connected to the %s\n",argv);
    }
    //¥¥Ω®œﬂ≥Ã
    Cli_EPS_fd=sockfd;
    Reconnect_EPS_Flag=0;
    Is_Connect_EPS=1;//¡¨Ω”…œ¡Àeps
    pthread_attr_init(&attr);//œﬂ≥Ã Ù–‘≥ı ºªØ
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//…Ë÷√Œ™∑÷¿Îœﬂ≥Ã£¨’‚—˘ø…“‘‘⁄œﬂ≥ÃΩ· ¯ ±¬Ì…œ Õ∑≈◊ ‘¥
    err=pthread_create(&Cli_Send,&attr,(void*)Cli_Uart_To_EPS_Thread,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
    if(err != 0)
        P_Log("EPS_Tcp_Client_Creat can't create Cli_Uart_To_EPS_Thread thread: %s\n", strerror(err));
    else
        P_Log("EPS_Tcp_Client_Creat create Cli_Uart_To_EPS_Thread thread OK\n");
    pthread_attr_destroy(&attr);//»•µÙœﬂ≥Ã Ù–‘
    
    pthread_attr_init(&attr);//œﬂ≥Ã Ù–‘≥ı ºªØ
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//…Ë÷√Œ™∑÷¿Îœﬂ≥Ã£¨’‚—˘ø…“‘‘⁄œﬂ≥ÃΩ· ¯ ±¬Ì…œ Õ∑≈◊ ‘¥
    err=pthread_create(&Cli_Receive,&attr,(void*)Cli_EPS_To_Uart_Thread,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
    if(err != 0)
        P_Log("EPS_Tcp_Client_Creat can't create Cli_EPS_To_Uart_Thread thread: %s\n", strerror(err));
    else
        P_Log("EPS_Tcp_Client_Creat create Cli_EPS_To_Uart_Thread thread OK\n");
    
    pthread_attr_destroy(&attr);//»•µÙœﬂ≥Ã Ù–‘
    return (0);
}

/********************************************************************\
* ∫Ø ˝√˚£∫: FS_Tcp_Client_Creat
* Àµ√˜:
* π¶ƒ‹:     FSøÕªß∂À¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int FS_Tcp_Client_Creat()
{
    const char *argv=(const char *)conf.FS_IP;
    int sockfd,err;                        // Socket file descriptor
    pthread_t Cli_Send,Cli_Receive;
    struct sockaddr_in remote_addr;    // Host address information

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log("ERROR:FS_Tcp_Client_Creat Failed to obtain Socket Descriptor!\n");
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET;                       // Protocol Family
    remote_addr.sin_port = htons(conf.FS_Port);             // Port number
    err=inet_pton(AF_INET, argv, &remote_addr.sin_addr);    // Net Address
    if(err <= 0)
    {
        if(err == 0)
            fprintf(stderr, "FS_Tcp_Client_Creat Not in presentation format\n");
        else
            perror("FS_Tcp_Client_Creat inet_pton error\n");
        return (0);
    }
    bzero(&(remote_addr.sin_zero), 8);                  // Flush the rest of struct
    /* Try to connect the remote */
    if((Cli_Connect_FS==0)&&((FS_OK==1)||FS_OK==2))
    {
        if(connect(sockfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1)
        {
            P_Log("ERROR:FS_Tcp_Client_Creat Failed to connect to the host!\n");
        }
        else
        {
            P_Log("OK:FS_Tcp_Client_Creat Have connected to the %s\n",argv);
        }
        Cli_Connect_FS=1;//¡¨Ω”…œ¡Àfsª
        Reconnect_FS_Flag=0;
        //¥¥Ω®œﬂ≥Ã
        Cli_FS_fd=sockfd;
        err=pthread_create(&Cli_Send,NULL,(void*)Cli_Uart_To_FS_Thread,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
        if(err != 0)
            P_Log("FS_Tcp_Client_Creat can't create Cli_Uart_To_FS_Thread thread: %s\n", strerror(err));
        else
            P_Log("FS_Tcp_Client_Creat create Cli_Uart_To_FS_Thread thread OK\n");
        err=pthread_create(&Cli_Receive,NULL,(void*)Cli_FS_To_Uart_Thread,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
        if(err != 0)
            P_Log("FS_Tcp_Client_Creat can't create Cli_FS_To_Uart_Thread thread: %s\n", strerror(err));
        else
            P_Log("FS_Tcp_Client_Creat create Cli_FS_To_Uart_Thread thread OK\n");
    }
    return 0;
}

/********************************************************************\
* ∫Ø ˝√˚: FS_Tcp_Server_Creat
* Àµ√˜: ”√”⁄fs øÕªß∂À¡¨Ω”º”∆¯ª˙
* π¶ƒ‹:     FS∑˛ŒÒ∂À¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int FS_Tcp_Server_Creat()
{
    int sockfd;                         // Socket file descriptor
    int nsockfd;                        // New Socket file descriptor
    int num;
    int sin_size;                       // to store struct size
    int opt=1;                          //…Ë÷√∂Àø⁄÷ÿ”√(wait◊¥Ã¨)
    int keepalive = 1;                  // ø™∆Ùkeepalive Ù–‘
    int keepidle = 30;                  // »Á∏√¡¨Ω”‘⁄30√Îƒ⁄√ª”–»Œ∫Œ ˝æ›Õ˘¿¥,‘ÚΩ¯––ÃΩ≤‚
    int keepinterval = 5;               // ÃΩ≤‚ ±∑¢∞¸µƒ ±º‰º‰∏ÙŒ™5 √Î
    int keepcount = 3;                  // ÃΩ≤‚≥¢ ‘µƒ¥Œ ˝.»Áπ˚µ⁄1¥ŒÃΩ≤‚∞¸æÕ ’µΩœÏ”¶¡À,‘Ú∫Û2¥Œµƒ≤ª‘Ÿ∑¢.
    //pthread_t Ser_Send,Ser_Receive;
    struct sockaddr_in addr_local;
    struct sockaddr_in addr_remote;

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log("ERROR:FS_Tcp_Server_Creat Failed to obtain Socket Despcritor.\n");
        return (0);
    }
    else
    {
        P_Log("OK:FS_Tcp_Server_Creat Obtain Socket Despcritor sucessfully.\n");
    }
    setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR,(void *)&opt,sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount));
    /* Fill the local socket address struct */
    addr_local.sin_family = AF_INET;                // Protocol Family
    addr_local.sin_port = htons(conf.My_FS_Port);   // Port number
    addr_local.sin_addr.s_addr  = INADDR_ANY;       // AutoFill local address
    bzero(&(addr_local.sin_zero), 8);               // Flush the rest of struct

    /*  Blind a special Port */
    if(bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1)
    {
        P_Log("ERROR:FS_Tcp_Server_Creat Failed to bind Port %d.\n",conf.My_FS_Port);
        return (0);
    }
    else
    {
        P_Log("OK:FS_Tcp_Server_Creat Bind the Port %d sucessfully.\n",conf.My_FS_Port);
    }

    /*  Listen remote connect/calling */
    if(listen(sockfd,BACKLOG) == -1)
    {
        P_Log("ERROR:FS_Tcp_Server_Creat Failed to listen Port %d.\n", conf.My_FS_Port);
        return (0);
    }
    else
    {
        P_Log("OK:FS_Tcp_Server_Creat Listening the Port %d sucessfully.\n", conf.My_FS_Port);
    }
    OPT_Server_OK=1;
    while(1)
    {
        FS_OK=0;
        sin_size = sizeof(struct sockaddr_in);
        /*  Wait a connection, and obtain a new socket file despriptor for single connection */
        if((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, &sin_size)) == -1)
        {
            P_Log("ERROR:FS_Tcp_Server_Creat Obtain new Socket Despcritor error.\n");
            continue;
        }
        else
        {
            P_Log("OK:FS_Tcp_Server_Creat Server has got connect from %s.\n", inet_ntoa(addr_remote.sin_addr));
        }
        Ser_Connect_FS=1;
        Ser_FS_fd=nsockfd;
        FS_OK=2;
        FS_Tcp_Client_Creat();
        while(1)
        {
            num = recv(Ser_FS_fd, FS_To_Uart, LENGTH, 0);
            switch(num)
            {
                case -1:
                    P_Log("ERROR:Ser_FS_To_Uart_Thread Receive string error!\n");
                    Ser_Connect_FS=0;
                    break;
                case  0:
                    Ser_Connect_FS=0;
                    break;
                default:
                    P_Log("OK:Ser_FS_To_Uart_Thread Receviced numbytes = %d\n", num);
                    break;
            }
            if(Ser_Connect_FS==0)
            {
                close(Ser_FS_fd);
                Ser_FS_fd=-1;
                //OPT_Server_OK=0;
                break;
            }
            else
            {
                Print_HEX("Ser_FS_To_Uart Recv<<<<<<<",num,FS_To_Uart);
                Lock(&FS_Recv_Data);
                Deal_FS_To_Uart_data();
                Unlock(&FS_Recv_Data);
 //               bzero(FS_To_Uart,LENGTH);
            }
        }
    }
    close(Ser_FS_fd);
    Ser_FS_fd=-1;
    OPT_Server_OK=0;
    return -1;
}


/********************************************************************\
* ∫Ø ˝√˚£∫: Tcp_Pthread_Creat
* Àµ√˜:
* π¶ƒ‹:     øÕªß∂ÀTCPœﬂ≥Ã¥¥Ω®
*  ‰»Î:     Œﬁ
* ∑µªÿ÷µ:   Œﬁ
* ¥¥Ω®»À:   Yang Chao Xu
* ¥¥Ω® ±º‰: 2014-8-26
\*********************************************************************/
int Tcp_Pthread_Creat()
{
    int err;
    pthread_t Tcp_Client,Tcp_Server;
    err=pthread_create(&Tcp_Client,NULL,(void*)FS_Tcp_Server_Creat,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
    if(err != 0)
    {
        P_Log("can't create FS_Tcp_Server_Creat thread: %s\n", strerror(err));
        return 1;
    }
    else
        P_Log("create FS_Tcp_Server_Creat thread OK\n");
    /*err=pthread_create(&Tcp_Client,NULL,(void*)FS_Tcp_Client_Creat,NULL);//¥¥Ω®Ω” ’œﬂ≥Ã
    if (err != 0)
        P_Log("can't create FS_Tcp_Client_Creat thread: %s\n", strerror(err));
    else
        P_Log("create FS_Tcp_Client_Creat thread OK\n");
    */
    return 0;
}

