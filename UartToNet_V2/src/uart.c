#include     "common.h"
#define FALSE 1
#define TRUE 0

int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200};
int name_arr[] = {115200, 57600, 38400,  19200,  9600,  4800,  2400, 1200};


/********************************************************************\
* 函数名: set_speed
* 说明:
* 功能:    设置串口波特率
* 输入:     fd:文件描述符
            speed:波特率
* 返回值:   0:成功，非0:失败
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-22
\*********************************************************************/
void set_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for(i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if(speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if(status != 0)
                perror("tcsetattr fd1");
            return;
        }
        tcflush(fd, TCIOFLUSH);
    }
}

/********************************************************************\
* 函数名: set_Parity
* 说明:
* 功能:    设置串口数据位、停止位、校验方式
* 输入:     fd:文件描述符
            databits:数据位
            stopbits:停止位
            parity:校验方式
* 返回值:   0:成功，非0:失败
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-22
\*********************************************************************/
int set_Parity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if(tcgetattr(fd, &options)  !=  0)
    {
        perror("SetupSerial 1");
        return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch(databits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return (FALSE);
    }
    switch(parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        case 'S':
        case 's':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported parity\n");
            return (FALSE);
    }
    switch(stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return (FALSE);
    }

    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    /* Set input parity option */

    if(parity != 'n')
        options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */
    if(tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    return (TRUE);
}

//********************************************************************
// 函数名: Dispose_Recv11
// 功能:    FSC 处理加气机上传流水
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void Dispose_Recv11()
{
    Uint    len;
    Uchar   i,tt,Tmp[6], dst[30], tmp[3];
    bzero(dst, 30);
    bzero(tmp, 3);
    len = Uart_Rx_Buff[5] + 9;
    tt=6;
    P_Log("===========Dispose_Recv11 Recv===========\n");

    //UID加气记录枪流水号
    for(i=0; i<6; i++)
    {
        Tmp[i]=Uart_Rx_Buff[tt+i];
        Gun_UID[i]=Tmp[i];
    }
    para.POS_TTC=BCDtoHEX(Tmp,6);
    P_Log("POS_TTC=%d\n", para.POS_TTC);

    //启动时间
    strcat(dst, "20");
    para.TIME[0]=0X20;
    for(i = 0; i < 6; i++)
    {
        para.TIME[i+1] = Uart_Rx_Buff[i+13+tt];
        sprintf(tmp, "%02X", para.TIME[i+1]);
        strcat(dst, tmp);
    }
    P_Log("Start_time:%s\n", dst);

    //停止时间
    bzero(dst, 30);
    strcat(dst, "20");
    para.END_TIME[0]=0X20;
    for(i = 0; i < 6; i++)
    {
        para.END_TIME[i+1] = Uart_Rx_Buff[i+19+tt];
        sprintf(tmp, "%02X", para.END_TIME[i+1]);
        strcat(dst, tmp);
    }
    P_Log("End_time:%s\n", dst);

    //IC卡卡号
    bzero(dst, 30);
    for(i = 0; i < 10; i++)
    {
        para.ASN[i] = Uart_Rx_Buff[i+25+tt];
        sprintf(tmp, "%02X", para.ASN[i]);
        strcat(dst, tmp);
    }
    P_Log("Card ASN:%s\n", dst);

    //本次加气量
    for(i = 0; i < 4; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+40+tt];
    }
    para.VOL=BCDtoHEX(Tmp,4);
    P_Log("VOL:%.2f\n", para.VOL / 100.0);

    //本次加气单价
    for(i = 0; i < 2; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+44+tt];
    }
    para.PRC=(Ushort)BCDtoHEX(Tmp,2);
    P_Log("PRC:%.2f\n", para.PRC / 100.0);

    //本次加气金额
    for(i = 0; i < 4; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+46+tt];
    }
    para.AMN=BCDtoHEX(Tmp,4);
    P_Log("AMN:%.2f\n", para.AMN / 100.0);

    //累计消费金额
    for(i = 0; i < 6; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+72+tt];
    }
    para.V_AMN=BCDtoHEX(Tmp,6);
    P_Log("V_AMN:%.2f\n", para.V_AMN / 100.0);

    //累计消费气量
    for(i = 0; i < 6; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+98+tt];
    }
    para.V_TOT=BCDtoHEX(Tmp,6);
    P_Log("V_TOT:%.2f\n", para.V_TOT / 100.0);

    //加气前压力
    for(i = 0; i < 2; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+88+tt];
    }
    para.STAR_PRE=(Ushort)BCDtoHEX(Tmp,2);
    P_Log("STAR_PRE:%.2f\n", para.STAR_PRE / 100.0);

    //加气后压力
    for(i = 0; i < 2; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+90+tt];
    }
    para.STOP_PRE=(Ushort)BCDtoHEX(Tmp,2);
    P_Log("STOP_PRE:%.2f\n", para.STOP_PRE / 100.0);

    //记录类型
    para.T_TYPE = Uart_Rx_Buff[35+tt];
    if(para.T_TYPE==1)
        P_Log("Online Record\n");
    else
        P_Log("Offline Record\n");
    if(para.VOL==0)
    {
        HaveAuth=0;
        StartFuelling=9;
    }
    else if((ReadNoPayFlag==0)&&(para.T_TYPE==1))
    {
        HaveAuth=0;
        para.ST=3;
        Reply_FS_UploadOverFuel();//上传交易记录
        StartFuelling=9;//交易记录上传后清0
        Time_100ms=0;//交易记录上传后再上传空闲状态
    }
    else
    {
        Reply_FS_ReadNO_Pay_Trans();
    }
    Lock(&Data_To_Uart);
    FCS_To_JKOPT_69(Gun_UID);
    Unlock(&Data_To_Uart);

}

//********************************************************************
// 函数名: Dispose_Recv19
// 功能:    FSC 处理加气机上传加气实时记录
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void Dispose_Recv19()
{
    Uint    len;
    Uchar   i,tt,Tmp[6], dst[30], tmp[3];
    bzero(dst, 30);
    bzero(tmp, 3);
    len = Uart_Rx_Buff[5] + 9;
    tt=6;
    P_Log("===========Dispose_Recv19 Recv===========\n");

    P_Log("ST:Fueling...\n");
    //IC卡卡号
    for(i = 0; i < 10; i++)
    {
        para.ASN[i] = Uart_Rx_Buff[i + tt];
        sprintf(tmp, "%02X", para.ASN[i]);
        strcat(dst, tmp);
    }
    P_Log("Card ASN:%s\n", dst);

    //本次加气量
    for(i = 0; i < 4; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+34+tt];
    }
    para.VOL=BCDtoHEX(Tmp,4);

    //本次加气单价
    for(i = 0; i < 2; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+32+tt];
    }
    para.PRC=(Ushort)BCDtoHEX(Tmp,2);

    //本次加气金额
    for(i = 0; i < 4; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+38+tt];
    }
    para.AMN=BCDtoHEX(Tmp,4);

    P_Log("CUR_VOL:%.2f\n", para.VOL / 100.0);
    P_Log("CUR_PRC:%.2f\n", para.PRC / 100.0);
    P_Log("CUR_AMN:%.2f\n", para.AMN / 100.0);
    HuiSuoFlag=0;
#if 0
    if((StartFuelling==0)&&(Uart_To_FS_Flag==0))
    {
        Reply_FS_UploadStatus(5);
        StartFuelling=5;
    }
    else if((StartFuelling==5)&&(Uart_To_FS_Flag==0)&&(para.AMN>5))
    {
        Reply_FS_UploadStatus(6);
        StartFuelling=6;
    }
    else if((StartFuelling==6)&&(Uart_To_FS_Flag==0))
    {
        Reply_FS_UploadStatus(8);
        StartFuelling=8;
    }
    else if((StartFuelling==8)&&(Uart_To_FS_Flag==0))
    {
        para.ST=8;
        Reply_FS_UploadRealtimeData();
    }
#endif
#if 1
    if((StartFuelling==0)&&(Uart_To_FS_Flag==0)&&(para.VOL>1))
    {
        Reply_FS_Upload568Status();
        StartFuelling=8;
    }
    else if((StartFuelling==8)&&(Uart_To_FS_Flag==0))
    {
        para.ST=8;
        Reply_FS_UploadRealtimeData();
    }
#endif
}

//********************************************************************
// 函数名: Dispose_Recv20
// 功能:    FSC 处理加气机上传状态
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void Dispose_Recv20()
{
    Uint    len;
    Uchar   i,tmp[3];
    len = Uart_Rx_Buff[5] + 9;
    P_Log("===========Dispose_Recv20 Recv===========\n");
    para.NZN=Uart_Rx_Buff[4];
    P_Log("Gun NO:%d\n", para.NZN);
    if(Uart_Rx_Buff[6]==0)//空闲
    {
        para.ST=3;
    }
    else if(Uart_Rx_Buff[6]==1)//加气中
    {
        para.ST=8;
    }
    else
    {
        para.ST = Uart_Rx_Buff[6];
    }

    if(FS_OK==2)
    {
        para.ST=2;
    }    
    tmp[0]=Uart_Rx_Buff[7];
    tmp[1]=Uart_Rx_Buff[8];
    para.NO_Payment_Trans=BCDtoHEX(tmp, 2);
    switch(para.ST)
    {
        case 1:
            P_Log("ST:Disable\n");
            break;
        case 2:
            P_Log("ST:Closed\n");
            break;
        case 3:
            HaveAuth=0;
            if(DownLoadPrice==1)
            {
                DownLoadPrice=0;
                F0_Time=0;
                Lock(&Data_To_Uart);
                FCS_To_JKOPT_75();
                Unlock(&Data_To_Uart);
            }
            P_Log("ST:Idle\n");
            break;
        case 4:
            P_Log("ST:Auth request\n");
            break;
        case 5:
            P_Log("ST:Has authorized\n");
            break;
        case 6:
            P_Log("ST:Start fuel\n");
            break;
        case 8:
            P_Log("ST:Fueling...\n");
            break;
        default:
            P_Log("ST:Error ST\n");
            break;
    }
    P_Log("NO_Payment_Trans:%d\n",para.NO_Payment_Trans);
}

/********************************************************************\
* 函数名: Dispose_Recv70
* 说明:
* 功能:    加气机申请授权
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2015-06-04
\*********************************************************************/
void Dispose_Recv70()
{
    Uchar   len, Flag, Type,i,tmp[6];
    long Value;
    len = Uart_Rx_Buff[5] + 9;
    P_Log("===========Dispose_Recv70 Recv===========\n");
    P_Log("Gun NO:%d\n", para.NZN);
    Flag = Uart_Rx_Buff[6];     //授权结果
    Auth_Flag=Flag;
    RecvFSAuth=0;
    if(Auth_Flag == 0)
    {
        P_Log("Auth Result: Success\n");
    }
    else
    {
        P_Log("Auth Result: Failed\n");
    }
    Type = Uart_Rx_Buff[7];          //授权类型
    if(Auth_Type == 0)
    {
        P_Log("Auth Type: Amount \n");
    }
    else if(Auth_Type == 1)
    {
        P_Log("Auth Type: Volume \n");
    }
    else
    {
        P_Log("Auth Type: Other \n");
    }
    for(i=0; i<6; i++)
    {
        tmp[i]=Uart_Rx_Buff[8+i];
    }
    Value = BCDtoHEX(tmp,6);
    P_Log("Auth Value:%ld\n", Auth_Value);
    P_Log("Flag=%d Value:%ld\n", Flag, Value);
    if((Flag == 0) && (Value == Auth_Value))
    {
        HaveAuth=1;
        Reply_FS_Auth_OPT();
    }
    else
    {
        Reply_FS_CantAuth_OPT();
    }
}



/********************************************************************\
* 函数名: Dispose_Recv71
* 说明:
* 功能:    加气机向EPS发命令( 只是转发)
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间:2015-06-04
\*********************************************************************/
void Dispose_Recv71()
{
    Uchar   i,len, dst[11], tmp[3];
    bzero(dst, 11);
    bzero(tmp, 3);

    if(((Cli_EPS_fd == -1) || (Cli_EPS_fd == 0)) && (Reconnect_EPS_Flag == 0))
    {
        Reconnect_EPS_Flag = 1;
    }

    len = Uart_Rx_Buff[5] + 9;
    P_Log("===========Dispose_Recv71 Recv===========\n");
    i = 0;
    EPS_Data_Len = Uart_Data_Len - 9;
    if(EPS_Data_Len==108)//发起验卡
    {
        if(YanKaFlag==1)//之前已经收到了验卡申请结果
        {
            YanKaReturn();
            return;
        }
        if((FaQiYanKaFlag==1)&&(FaQiYanKaTime<YANKATIMEOUT))//防止多次验卡后造成MAC2错误)
        {
            return;
        }        
        OverTransFlag=0;
        FaQiYanKaFlag=1;        
    }
    else if(EPS_Data_Len==80)//发起通知灰锁
    {
        FaQiYanKaFlag=0;
        FaQiYanKaTime=0;
        if(HuiSuoFlag==1)//之前收到了灰锁结果
        {
            HuiSuoReturn();
            return;
        }
    }
    else if(EPS_Data_Len==51)//发起支付申请
    {
        HuiSuoFlag=0;
        if(RequestPayFlag==1)//之前收到了申请支付结果
        {
            RequestPayReturn();
            return;
        }
    }
    else if(EPS_Data_Len==78)//发起通知交易结果
    {
        BoartRequestFlag=0;
        if(OverTransFlag==1)//之前收到了通知交易结果
        {
            OverTransReturn();
            return;
        }
    }
    while(i < EPS_Data_Len)
    {
        Uart_To_EPS[i] = Uart_Rx_Buff[i + 6];
        i++;
    }
    Uart_To_EPS_Flag = 1;
}

/********************************************************************\
* 函数名: Dispose_Recv72
* 说明:
* 功能:    FS读加气机累计
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间:2015-06-04
\*********************************************************************/
void Dispose_Recv72()
{
    Uint    len;
    Uchar   i,tt,Tmp[6];
    len = Uart_Rx_Buff[5] + 9;
    tt=6;
    P_Log("===========Dispose_Recv72 Recv===========\n");

    //UID加气记录枪流水号
    for(i=0; i<6; i++)
    {
        Tmp[i]=Uart_Rx_Buff[tt+i];
        Gun_UID[i]=Tmp[i];
    }
    para.POS_TTC=BCDtoHEX(Tmp,6);
    P_Log("POS_TTC=%d\n", para.POS_TTC);

    //累计消费气量
    for(i = 0; i < 6; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+6+tt];
    }
    para.V_TOT=BCDtoHEX(Tmp,6);
    P_Log("V_TOT:%.2f\n", para.V_TOT / 100.0);

    //累计消费金额
    for(i = 0; i < 6; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+12+tt];
    }
    para.V_AMN=BCDtoHEX(Tmp,6);
    P_Log("V_AMN:%.2f\n", para.V_AMN / 100.0);

    if(ReadTotal_Flag==1)
    {
        ReadTotal_Flag=0;        
        OPT_72_Flag=0;
        Reply_FS_ReadTotal();
    }
}

/********************************************************************\
* 函数名: Dispose_Recv73
* 说明:
* 功能:    加气机回复停止加气命令
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2015-06-04
\*********************************************************************/
void Dispose_Recv73()
{
    Uchar   len;
    len = Uart_Rx_Buff[5] + 9;
    P_Log("===========Dispose_Recv73 Recv===========\n");
    if(Uart_Rx_Buff[6] == 0)   //停止加气机成功
    {
        Reply_FS_Stop_OPT();
    }
    StartFuelling=0;
    //para.ST = 3; //停止加气机主动上传给fuel
}

/********************************************************************\
* 函数名: Dispose_Recv74
* 说明:
* 功能:    加气机上传交易
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2015-06-04
\*********************************************************************/
void Dispose_Recv74()
{
    Uint    len;
    Uchar   i,tt,Tmp[6];
    len = Uart_Rx_Buff[5] + 9;
    tt=6;
    P_Log("===========Dispose_Recv74 Recv===========\n");

    //UID加气记录枪流水号
    for(i=0; i<6; i++)
    {
        Tmp[i]=Uart_Rx_Buff[tt+i];
        Gun_UID[i]=Tmp[i];
    }
    para.POS_TTC=BCDtoHEX(Tmp,6);
    P_Log("POS_TTC=%d\n", para.POS_TTC);

    //消费气量
    for(i = 0; i < 6; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+6+tt];
    }
    para.VOL=BCDtoHEX(Tmp,6);
    P_Log("VOL:%.2f\n", para.V_TOT / 100.0);

    //消费金额
    for(i = 0; i < 6; i++)
    {
        Tmp[i] = Uart_Rx_Buff[i+12+tt];
    }
    para.AMN=BCDtoHEX(Tmp,6);
    P_Log("AMN:%.2f\n", para.V_AMN / 100.0);

    Reply_FS_UploadOverFuel();//上传交易记录
    Lock(&Data_To_Uart);
    FCS_To_JKOPT_74();
    Unlock(&Data_To_Uart);
}

/********************************************************************\
* 函数名: Dispose_Recv75
* 说明:
* 功能:    加气机回复下发单价命令
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2015-06-04
\*********************************************************************/
void Dispose_Recv75()
{
    P_Log("===========Dispose_Recv75 Recv===========\n");
    if(Uart_Rx_Buff[6] != 0)   //成功
    {
        FCS_To_JKOPT_75();
        P_Log("===========DownLoad Price Failed===========\n");
    }
    else
    {
        Reply_FS_DownLoad_Price_Success();
        P_Log("===========DownLoad Price Success===========\n");
    }
}


/********************************************************************\
* 函数名: Dispose_Recv76
* 说明:
* 功能:    加气机上传参数
* 输入:    无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2015-06-12
\*********************************************************************/
void Dispose_Recv76()
{
//    unsigned char   FS_IP[4];           //FS IP地址4
//    unsigned int    FS_Port;            //FS 端口号6
//    unsigned char   FS_NODE[2];         //FS节点8
//    unsigned char   EPS_IP[4];          //EPS IP地址12
//    unsigned int    EPS_Port;           //EPS端口号14
//    unsigned char   Local_NODE[2];      //本地节点16
//    unsigned char   Local_IP[4];        //本地 IP 地址 20
//    unsigned char   Local_Mask[4];      //本地掩码24
//    unsigned char   Local_Gateway[4];   //本地网关28
//    unsigned char   Local_Broad_IP[4];  //本地广播 IP 地址 32
//    unsigned int    Local_Broad_Port;   //本地广播 IP 端口34

    Uint    len,tmp_port;
    Uchar   i,tt,Tmp[6],dst[20],Strlen,Strlen1,reboot_flag=0;
    len = Uart_Rx_Buff[5] + 9;

    tt=6;
    P_Log("===========Dispose_Recv76 Recv===========\n");

    //FS IP地址
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[6+i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[6+i]);
        strcat(dst, Tmp);
    }
    Strlen = strlen(dst);
    Strlen1 = strlen(conf.FS_IP);
    if(strcmp(conf.FS_IP,dst)!=0)
    {
        if(Strlen<Strlen1)
        {
            bzero(conf.FS_IP,sizeof(conf.FS_IP));
        }
        strncpy(conf.FS_IP,dst,Strlen);
        write_conf_value("FS_IP",conf.FS_IP ,Conf_Name);
        reboot_flag=1;
    }
    P_Log("FS_IP=%s\n", dst);

    //fs port
    bzero(Tmp, 6);
    tmp_port=(Uint)(Uart_Rx_Buff[10])<<8|Uart_Rx_Buff[11];
    sprintf(Tmp,"%d",tmp_port);
    if(conf.FS_Port!=tmp_port)
    {
        conf.FS_Port=tmp_port;
        write_conf_value("FS_Port",Tmp,Conf_Name);
        reboot_flag=1;
    }
    P_Log("FS_Port=%d\n",tmp_port);

    //FS NODE
    bzero(Tmp,6);
    sprintf(Tmp,"%02d.%02d",Uart_Rx_Buff[12],Uart_Rx_Buff[13]);
    if(strncmp(Tmp,conf.IFSF_Node,5)!=0) //获取FS的节点
    {
        strcpy(conf.IFSF_Node,Tmp);
        write_conf_value("IFSF_Node",conf.IFSF_Node,Conf_Name);
        reboot_flag=1;
    }
    P_Log("IFSF_Node=%s\n", Tmp);

    // unsigned char   EPS_IP[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[14+i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[14+i]);
        strcat(dst, Tmp);
    }
    Strlen = strlen(dst);
    Strlen1 = strlen(conf.EPS_IP);
    if(strcmp(conf.EPS_IP,dst)!=0)
    {
        if(Strlen<Strlen1)
        {
            bzero(conf.EPS_IP,sizeof(conf.EPS_IP));
        }
        strncpy(conf.EPS_IP,dst,Strlen);
        write_conf_value("EPS_IP",conf.EPS_IP ,Conf_Name);
        reboot_flag=1;
    }
    P_Log("EPS_IP=%s\n", dst);

    //unsigned int    EPS_Port
    bzero(Tmp, 6);
    tmp_port=(Uint)(Uart_Rx_Buff[18])<<8|Uart_Rx_Buff[19];
    sprintf(Tmp,"%d",tmp_port);
    if(conf.EPS_Port!=tmp_port)
    {
        conf.EPS_Port=tmp_port;
        write_conf_value("EPS_Port",Tmp,Conf_Name);
        reboot_flag=1;
    }
    P_Log("EPS_Port=%d\n",tmp_port);

    //unsigned char   Local_NODE[2];
    bzero(Tmp,6);
    sprintf(Tmp,"%02d.%02d",Uart_Rx_Buff[20],Uart_Rx_Buff[21]);
    if(strncmp(Tmp,conf.My_Node,5)!=0) //获取FS的节点
    {
        strcpy(conf.My_Node,Tmp);
        write_conf_value("My_Node",conf.My_Node,Conf_Name);
        reboot_flag=1;
    }
    P_Log("My_Node=%s\n", Tmp);

    //unsigned char   Local_IP[4];
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[22+i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[22+i]);
        strcat(dst, Tmp);
    }
    Strlen = strlen(dst);
    Strlen1 = strlen(conf.My_IP);
    if(strcmp(conf.My_IP,dst)!=0)
    {
        if(Strlen<Strlen1)
        {
            bzero(conf.My_IP,sizeof(conf.My_IP));
        }
        strncpy(conf.My_IP,dst,Strlen);
        write_conf_value("My_IP",conf.My_IP ,Conf_Name);
        reboot_flag=1;
    }
    P_Log("My_IP=%s\n", dst);

    // unsigned char   Local_Mask[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[26+i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[26+i]);
        strcat(dst, Tmp);
    }
    Strlen = strlen(dst);
    Strlen1 = strlen(conf.My_Mask);
    if(strcmp(conf.My_Mask,dst)!=0)
    {
        if(Strlen<Strlen1)
        {
            bzero(conf.My_Mask,sizeof(conf.My_Mask));
        }
        strncpy(conf.My_Mask,dst,Strlen);
        write_conf_value("My_Mask",conf.My_Mask ,Conf_Name);
        reboot_flag=1;
    }
    P_Log("My_Mask=%s\n", dst);

    // unsigned char   Local_Gateway[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[30+i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[30+i]);
        strcat(dst, Tmp);
    }
    Strlen = strlen(dst);
    Strlen1 = strlen(conf.My_Gateway);
    if(strcmp(conf.My_Gateway,dst)!=0)
    {
        if(Strlen<Strlen1)
        {
            bzero(conf.My_Gateway,sizeof(conf.My_Gateway));
        }
        strncpy(conf.My_Gateway,dst,Strlen);
        write_conf_value("My_Gateway",conf.My_Gateway ,Conf_Name);
        reboot_flag=1;
    }
    P_Log("My_Gateway=%s\n", dst);

    //unsigned char   Local_Broad_IP[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[34+i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[34+i]);
        strcat(dst, Tmp);
    }
    Strlen = strlen(dst);
    Strlen1 = strlen(conf.My_Broadcast_IP);
    if(strcmp(conf.My_Broadcast_IP,dst)!=0)
    {
        if(Strlen<Strlen1)
        {
            bzero(conf.My_Broadcast_IP,sizeof(conf.My_Broadcast_IP));
        }
        strncpy(conf.My_Broadcast_IP,dst,Strlen);
        write_conf_value("My_Broadcast_IP",conf.My_Broadcast_IP ,Conf_Name);
        reboot_flag=1;
    }
    P_Log("My_Broadcast_IP=%s\n", dst);

    //unsigned int    Local_Broad_Port
    bzero(Tmp, 6);
    tmp_port=(Uint)(Uart_Rx_Buff[38])<<8|Uart_Rx_Buff[39];
    sprintf(Tmp,"%d",tmp_port);
    if(conf.My_Broadcast_Port!=tmp_port)
    {
        conf.My_Broadcast_Port=tmp_port;
        write_conf_value("My_Broadcast_Port",Tmp,Conf_Name);
        reboot_flag=1;
    }
    P_Log("My_Broadcast_Port=%d\n",tmp_port);

    //gun_num
    tmp_port = Uart_Rx_Buff[40];

    if(conf.Gun_num!=tmp_port)
    {
        para.NZN = tmp_port;
        conf.Gun_num = tmp_port;
        bzero(Tmp, 6);
        sprintf(Tmp, "%d", tmp_port);
        write_conf_value("Gun_num",Tmp,Conf_Name);
        reboot_flag=1;
    }
    P_Log("Gun_num=%d\n",tmp_port);

    if(Uart_Tx_Flag==1)
    {
        Uart_Tx_Flag=0;
    }
    Lock(&Data_To_Uart);
    FCS_To_JKOPT_76();
    Unlock(&Data_To_Uart);
    if(reboot_flag==1)
    {
        sleep(2);
        system("reboot");
    }
}
/********************************************************************\
* 函数名: Deal_Uart_Recv_Data
* 说明:     处理串口接收的数据(解析后发送到FS还是EPS)
* 功能:     串口数据处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-01
\*********************************************************************/
void Deal_Uart_Recv_Data()
{
    Uchar  len,crc,recv_crc;   
    if((OPT_IS_Online==0)&&(Uart_Rx_Buff[3]==0x19))//脱机加气时直接返回
    {
        return;
    }
    OPT_IS_Online=200;
    len = Uart_Rx_Buff[5] + 9;//消息总长度
    if(len != Uart_Data_Len)
    {
        P_Log("Data recv length error! Uart Recv len=%d,OPT send len=%d\n", Uart_Data_Len, len);
    }
    crc = int_crc8(Uart_Rx_Buff + 2, len - 5);//计算CRC
    recv_crc =  Uart_Rx_Buff[len - 3];        //收到的CRC

    if((crc != recv_crc)||(len != Uart_Data_Len))
    {
        P_Log("Data recv CRC error! Uart Recv CRC=%02X,Local CRC=%02X\n", recv_crc, crc);
        switch(CmdFlag)
        {
            case 0x52:
                FCS_To_JKOPT_52(PointNo,ParaNo,ParaValue);
                break;
            case 0x69:
                FCS_To_JKOPT_69(Gun_UID);
                break;
            case 0x70:
                HEXtoBCD(Auth_Value,6);
                FCS_To_JKOPT_70(Auth_Type,HEX_BCD);
                break;
            /*case 0x71:
                                        FCS_To_JKOPT_71();
                                        break;*/
            case 0x72:
                FCS_To_JKOPT_72();
                break;
            case 0x73:
                FCS_To_JKOPT_73();
                break;
            case 0x74:
                FCS_To_JKOPT_74();
                break;
            case 0x75:
                FCS_To_JKOPT_75();
                break;
            case 0x76:
                FCS_To_JKOPT_76();
                break;
            default:
                break;
        }
    }
    else
    {
        switch(Uart_Rx_Buff[3])
        {
            case 0x11:
                Dispose_Recv11();
                break;
            case 0x19:
                Dispose_Recv19();
                break;
            case 0x20:
                Dispose_Recv20();
                break;
            case 0x70:
                Dispose_Recv70();
                break;
            case 0x71:
                Dispose_Recv71();
                break;
            case 0x72:
                Dispose_Recv72();
                break;
            case 0x73:
                Dispose_Recv73();
                break;
            case 0x74:
                para.ST=3;
                Dispose_Recv74();
                break;
            case 0x75:
                Dispose_Recv75();
                break;
            case 0x76:
                Dispose_Recv76();
                break;
            default:
                break;
        }
    }

}


/*开始时读加气机状态20*/
void Reply_FS_ReadStatus()
{
    //01.03.02.01.00.0E.00.03.01.20.14.
    //02.01.01.03.00.2E.00.05.01.21.14.01.02.02.01.01.03.00.EE.00.03.01.20.00.
    Uchar tt = 7;
    Uart_To_FS[tt++] = 0x05;                        //长度低位
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x14;
    Uart_To_FS[tt++] = 0x01;
    //para.ST=0x02;//test
    Uart_To_FS[tt++] = para.ST;                     //加气机状态
    fp_id.FP_State = para.ST;
    Uart_To_FS[tt++] = FS_To_Uart[2];
    Uart_To_FS[tt++] = FS_To_Uart[3];
    Uart_To_FS[tt++] = FS_To_Uart[0];
    Uart_To_FS[tt++] = FS_To_Uart[1];
    Uart_To_FS[tt++] = 0x0;
    Uart_To_FS[tt++] = (0xE0 | (0x1F & FS_To_Uart[5]));
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x20;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/*读加气机状态是否改变*/
void Reply_FS_StaIsChanged()
{
    Uchar i, tt = 7;
    //01.03.02.01.00.1E.00.03.01.21.14.
    //02.01.01.03.00.3E.00.05.01.21.14.01.02 状态未改变
    /*
       状态改变
       02.01.01.03.00.80.00.18.01.21.
       64.00.14.01.06.15.01.01.16.02.00.00.50.08.00.00.00.00.00.00.00.00
     */
    if(HaveAuth==1)
    {
        para.ST=5;
    }
    if((para.ST == 0x03) || (para.ST == 0x05)||(para.ST == 0x02))   //状态未改变
    {
        Uart_To_FS[tt++] = 0x05;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x14;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = para.ST;                     //加气机状态
    }
    else     //状态改变
    {
        Uart_To_FS[tt++] = 0x18;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x64;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x14;                        //加气机状态ID
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = para.ST;                     //加气机状态
        fp_id.FP_State = para.ST;
        Uart_To_FS[tt++] = 0x15;                        //油枪状态ID
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x01;                        //油枪状态
        Uart_To_FS[tt++] = 0x16;                        //fp控制器ID
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x50;
        Uart_To_FS[tt++] = 0x08;
        for(i = 0; i < 8; i++)
        {
            Uart_To_FS[tt++] = 0x00;
        }
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*主动上传状态*/
void Reply_FS_UploadStatus(Uchar st)
{
    Uchar i, tt = 0;
    //02.01.01.03.00.80.00.18.01.21.64.00.14.01.03.15.01.00.16.02.00.00.50.08.00.00.00.00.00.00.00.00
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //长度低位
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //加气机状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = st;                          //加气机状态
    Uart_To_FS[tt++] = 0x15;                        //油枪状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //油枪状态
    Uart_To_FS[tt++] = 0x16;                        //fp控制器ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*主动上传5,6,8状态*/
void Reply_FS_Upload568Status()
{
    Uchar i, tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //长度低位
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //加气机状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x05;                        //加气机状态
    Uart_To_FS[tt++] = 0x15;                        //油枪状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //油枪状态
    Uart_To_FS[tt++] = 0x16;                        //fp控制器ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //长度低位
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //加气机状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x06;                        //加气机状态
    Uart_To_FS[tt++] = 0x15;                        //油枪状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //油枪状态
    Uart_To_FS[tt++] = 0x16;                        //fp控制器ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //长度低位
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //加气机状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x08;                        //加气机状态
    Uart_To_FS[tt++] = 0x15;                        //油枪状态ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //油枪状态
    Uart_To_FS[tt++] = 0x16;                        //fp控制器ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }   
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/*选择油品地址*/
void Reply_FS_SelectOilAddr()
{
    //01.03.02.01.00.5A.00.05.01.81.04.01.01.    01对应41H...08对应48H
    //02.01.01.03.00.FA.00.05.01.81.05.04.02.
    Uchar tt = 7;
    if(c_dat.Auth_State_Mode == 1)   //选择了
    {
        Uart_To_FS[tt++] = 0x05;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = FS_To_Uart[9];
        Uart_To_FS[tt++] = 0x05;
        Uart_To_FS[tt++] = 0x04;
        Uart_To_FS[tt++] = 0x02;
    }
    else
    {
        //0201010100FC0003018100
        if(FS_To_Uart[10] == 0x01)
        {
            m_id.Meter_Type = FS_To_Uart[12];
        }
        else if(FS_To_Uart[10] == 0x04)
        {
            m_id.PR_Id = FS_To_Uart[12]; //1对应41H...8对应48H
        }
        Uart_To_FS[tt++] = 0x03;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = FS_To_Uart[9];
        Uart_To_FS[tt++] = 0x00;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*写C_DAT数据库中的0B、16、17*/
void Reply_FS_Write0B1617()
{
    //01.03.02.01.00.5F.00.0B.01.01.0B.01.00.16.01.00.17.01.00.
    //02.01.01.03.00.FF.00.09.01.01.05.0B.00.16.02.17.02.
    //02.01.01.01.00.E2.00.03.01.01.00
    Uchar tt = 7;
    if(c_dat.Auth_State_Mode == 1)
    {
        Uart_To_FS[tt++] = 0x09;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x05;                        //消息确认状态
        Uart_To_FS[tt++] = 0x0B;
        Uart_To_FS[tt++] = c_dat.Auth_State_Mode;
        Uart_To_FS[tt++] = 0x16;
        Uart_To_FS[tt++] = c_dat.Min_Fuelling_Vol;
        Uart_To_FS[tt++] = 0x17;
        Uart_To_FS[tt++] = c_dat.Min_Display_Vol;
    }
    else
    {
        c_dat.Auth_State_Mode = FS_To_Uart[12];
        c_dat.Min_Fuelling_Vol = FS_To_Uart[15];
        c_dat.Min_Display_Vol = FS_To_Uart[18];
        Uart_To_FS[tt++] = 0x03;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x00;                        //消息确认状态
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*读累计*/
void Reply_FS_ReadTotal()
{
    Uchar i,tt = 0;
    //01.03.02.01.00.0F.00.06.02.21.11.14.15.16.
    //02.01.01.03.00.2F.00.1E.02.21.11.14.07.0A.00.00.04.47.17.78.15.07.0A.00.00.00.08.65.58.16.07.0C.00.00.00.00.00.94.
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv57_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x1E;                        //长度低位
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x11;
    Uart_To_FS[tt++] = 0x14;
    Uart_To_FS[tt++] = 0x07;
    HEXtoFsData_New(para.V_TOT, 2, 0, 6);
    for(i = 0; i < 7; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x15;
    Uart_To_FS[tt++] = 0x07;
    HEXtoFsData_New(para.V_AMN, 2, 0, 6);
    for(i = 0; i < 7; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x16;
    Uart_To_FS[tt++] = 0x07;
    HEXtoFsData_New(para.POS_TTC, 0, 0, 6);
    for(i = 0; i < 7; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*从FP中读取未支付交易*/
void Reply_FS_ReadNO_Pay_Trans()
{
    //01.03.02.01.00.10.00.0F.04.21.20.00.00.01.06.05.07.08.0A.15.02.EA.EB
    /*
        02.01.01.03.00.35.00.2D.
        04.21.21.16.30.
        01.02.16.30.
        06.05.06.00.00.00.60.
        05.05.06.00.00.05.00.
        07.04.04.00.08.30.
        08.01.01.
        0A.04.00.00.00.08.
        15.01.02.
        02.02.02.01.
        02.01.01.03.00.F5.00.06.04.21.20.00.00.00

        02.01.01.03.00.26.00.31.
        04.21.21.30.20.
        01.02.30.20.
        06.05.06.00.00.02.08.
        05.05.06.00.00.17.26.
        07.04.04.00.08.30.
        08.01.01.
        0A.04.00.00.00.08.
        15.01.02.
        02.02.02.01.
        EA.00.EB.00.
        */
    Uchar i, tt = 0;

    if(((para.NO_Payment_Trans!=0)||(FS_OK==1))&&
        (ttc!=para.POS_TTC)&&
        (((totalnum!=para.V_TOT)&&(para.VOL!=0))||(para.T_TYPE==0)))//有未交易数
    {
        P_Log("totalnum=%ld\n",totalnum);
        P_Log("para.VOL=%d\n",para.VOL);
        P_Log("para.T_TYPE=%d\n",para.T_TYPE);
        P_Log("para.V_TOT=%d\n",para.V_TOT);
        P_Log("para.NO_Payment_Trans=%d\n",para.NO_Payment_Trans);
        
        if(para.NO_Payment_Trans>0)
            para.NO_Payment_Trans--;
        ttc=para.POS_TTC;
        totalnum=para.V_TOT;
        Uart_To_FS[tt++] = conf.FS_NODE[0];
        Uart_To_FS[tt++] = conf.FS_NODE[1];
        Uart_To_FS[tt++] = conf.MY_NODE[0];
        Uart_To_FS[tt++] = conf.MY_NODE[1];
        Uart_To_FS[tt++] = 0;                               //消息代码
        Uart_To_FS[tt++] = Deal_Status_Code(FS_To_Uart[5]); //消息状态字
        Uart_To_FS[tt++] = 0;                               //长度高位

        Uart_To_FS[tt++] = 0x39;    //长度tt=7
        Uart_To_FS[tt++] = 0x04;    //数据库个数tt=8
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x21;
        HEXtoBCD(para.POS_TTC, 2);
        Uart_To_FS[tt++] = HEX_BCD[0];
        Uart_To_FS[tt++] = HEX_BCD[1];
        Uart_To_FS[tt++] = 0x01;    //fp生成的唯一序列号
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = HEX_BCD[0];
        Uart_To_FS[tt++] = HEX_BCD[1];
        Uart_To_FS[tt++] = 0x06;    //交易体积
        Uart_To_FS[tt++] = 0x05;
        HEXtoFsData_New(para.VOL, 2, 0, 4);
        for(i = 0; i < 5; i++)
        {
            Uart_To_FS[tt++] = HEX_FsData[i];
        }
        Uart_To_FS[tt++] = 0x05;    //交易金额
        Uart_To_FS[tt++] = 0x05;
        HEXtoFsData_New(para.AMN, 2, 0, 4);
        for(i = 0; i < 5; i++)
        {
            Uart_To_FS[tt++] = HEX_FsData[i];
        }
        Uart_To_FS[tt++] = 0x07;    //交易单价
        Uart_To_FS[tt++] = 0x04;
        HEXtoFsData_New(para.PRC, 2, 0, 3);
        for(i = 0; i < 4; i++)
        {
            Uart_To_FS[tt++] = HEX_FsData[i];
        }
        Uart_To_FS[tt++] = 0x08;    //交易枪号
        Uart_To_FS[tt++] = 0x01;
        //Uart_To_FS[tt++] = para.NZN;//枪号大于17时有问题
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x0A;    //交易油品号码
        Uart_To_FS[tt++] = 0x04;
        for(i = 0; i < 4; i++)
        {
            Uart_To_FS[tt++] = price.O_TYPE[i];
        }
        Uart_To_FS[tt++] = 0x15;    //缓冲区的状态
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x02;    //指定释放的控制器
        Uart_To_FS[tt++] = 0x02;

        if(para.T_TYPE==0)
        {
            Uart_To_FS[tt++] = 0xFF;
            Uart_To_FS[tt++] = 0xFF;
        }
        else
        {
            Uart_To_FS[tt++] = conf.FS_NODE[0];
            Uart_To_FS[tt++] = conf.FS_NODE[1];
        }
        Uart_To_FS[tt++] = 0xEA;
        Uart_To_FS[tt++] = 0x04;
        HEXtoFsData_New(para.STAR_PRE, 2, 0, 3);
        for(i = 0; i < 4; i++)
        {
            Uart_To_FS[tt++] = HEX_FsData[i];
        }
        Uart_To_FS[tt++] = 0xEB;
        Uart_To_FS[tt++] = 0x04;
        HEXtoFsData_New(para.STOP_PRE, 2, 0, 3);
        for(i = 0; i < 4; i++)
        {
            Uart_To_FS[tt++] = HEX_FsData[i];
        }
        if(para.NO_Payment_Trans==0)
        {
            ReadNoPayFlag=0;
            Uart_To_FS[tt++] = conf.FS_NODE[0];
            Uart_To_FS[tt++] = conf.FS_NODE[1];
            Uart_To_FS[tt++] = conf.MY_NODE[0];
            Uart_To_FS[tt++] = conf.MY_NODE[1];
            Uart_To_FS[tt++] = 0x00;
            Uart_To_FS[tt++] = 0xE0 | (0x1F & FS_To_Uart[5]);
            Uart_To_FS[tt++] = 0x00;
            Uart_To_FS[tt++] = 0x06;
            Uart_To_FS[tt++] = 0x04;
            Uart_To_FS[tt++] = 0x21;
            Uart_To_FS[tt++] = 0x20;
            Uart_To_FS[tt++] = 0x00;
            Uart_To_FS[tt++] = 0x00;
            Uart_To_FS[tt++] = 0x00; 
        }
        FS_Data_Len = tt;
        Uart_To_FS_Flag = 1;
    }
    //else if(para.NO_Payment_Trans==0) //没有未交易记录
    else
    {
        //02.01.01.03.00.F0.00.06.04.21.20.00.00.00
        tt=0;
        ReadNoPayFlag=0;
        totalnum=para.V_TOT;

        Uart_To_FS[tt++] = conf.FS_NODE[0];
        Uart_To_FS[tt++] = conf.FS_NODE[1];
        Uart_To_FS[tt++] = conf.MY_NODE[0];
        Uart_To_FS[tt++] = conf.MY_NODE[1];
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++]=0xE0|(0x1F&FS_To_Uart[5]);
        Uart_To_FS[tt++]=0x00;
        Uart_To_FS[tt++]=0x06;
        Uart_To_FS[tt++]=0x04;
        Uart_To_FS[tt++]=0x21;
        Uart_To_FS[tt++]=0x20;
        Uart_To_FS[tt++]=0x00;
        Uart_To_FS[tt++]=0x00;
        Uart_To_FS[tt++]=0x00;
        FS_Data_Len = tt;
        Uart_To_FS_Flag = 1;
    }
}

/*写油枪数据库*/
void Reply_FS_WriteOilDataBase()
{
    Uchar tt = 7;
    //01.03.02.01.00.5B.00.09.02.21.11.01.01.01.07.01.01.
    //02.01.01.03.00.FB.00.08.02.21.11.05.01.00.07.02.拒绝
    //02.01.01.03.00.FB.00.05.02.21.11.00 接受
    fp_id_ln_id.Meter_1_Id=0;
    if(fp_id_ln_id.Meter_1_Id != 0)
    {
        Uart_To_FS[tt++] = 0x08;                        //长度低位
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x11;
        Uart_To_FS[tt++] = 0x05;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = fp_id_ln_id.PR_Id;
        Uart_To_FS[tt++] = 0x07;
        Uart_To_FS[tt++] = fp_id_ln_id.Meter_1_Id;
    }
    else
    {
        fp_id_ln_id.PR_Id = FS_To_Uart[13];
        fp_id_ln_id.Meter_1_Id = FS_To_Uart[16];
        Uart_To_FS[tt++] = 0x04;                        //长度低位
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x11;
        Uart_To_FS[tt++] = 0x00;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;

}

/*打开一个关闭的FP*/
void Reply_FS_OpenFP_old()
{
    Uchar i, tt = 7;
    static Uchar flag = 0;
    //01.03.02.01.00.42.00.04.01.21.3C.00.
    /*
    02.01.01.03.00.E2.00.03.01.21.00.
    02.01.01.03.00.80.00.18.01.21.64.00.14.01.03.15.01.00.16.02.00.00.50.08.00.00.00.00.00.00.00.00.
    */
    if(flag == 0)
    {
        Uart_To_FS[tt++] = 0x03;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x00;
        flag = 1;
    }
    else
    {
        tt = 0;
        Uart_To_FS[tt++] = FS_To_Uart[2];
        Uart_To_FS[tt++] = FS_To_Uart[3];
        Uart_To_FS[tt++] = FS_To_Uart[0];
        Uart_To_FS[tt++] = FS_To_Uart[1];
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x80;
        Uart_To_FS[tt++] = 0x18;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x64;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x14;                        //加气机状态ID
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = para.ST;                     //加气机状态
        fp_id.FP_State = para.ST;
        Uart_To_FS[tt++] = 0x15;                        //油枪状态ID
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x00;                        //油枪状态
        Uart_To_FS[tt++] = 0x16;                        //fp控制器ID
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x50;
        Uart_To_FS[tt++] = 0x08;
        for(i = 0; i < 8; i++)
        {
            Uart_To_FS[tt++] = 0x00;
        }
        flag = 0;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/*下载指定油品\加气模式下的单价*/
void Reply_FS_DownLoad_Price()
{
    Uchar i, j, tt = 7, tmp[20], tmp1[20],tmp2[2];

    PRICE pri;
    //01.03.02.01.00.43.00.0D.06.61.00.00.00.08.11.02.04.04.00.04.90.
    //02.01.01.03.00.E3.00.08.06.61.00.00.00.08.11.00.
    Uart_To_FS[tt++] = 0x08;                        //长度低位
    for(i = 0; i < 7; i++)
    {
        Uart_To_FS[tt++] = FS_To_Uart[8 + i];
    }
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
    //得到FS数据buf
    bzero(tmp, 20);
    for(i = 0; i < 4; i++)
    {
        tmp[i] = FS_To_Uart[17 + i];
    }
    //将fs数据转为整形数
    j = FS_To_Uart[14] & 0x0F;
    pri.Price[j - 1] = FSDataToLong(4, tmp);
    P_Log("===========price[%d]=%d============\n", j - 1, pri.Price[j - 1]);
    if(pri.Price[j - 1] != price.Price[j - 1])
    {
        price.VER++;//油价版本号加1
        if(price.VER > 100)
        {
            price.VER = 1;
        }
        conf.Pri_Ver = price.VER;
        price.Price[j - 1] = pri.Price[j - 1];

        bzero(tmp, 20);
        sprintf(tmp, "%d", price.VER);
        write_conf_value("Pri_Ver", tmp, Pri_ConfName);
        bzero(tmp, 20);
        sprintf(tmp, "price%d", j - 1);
        bzero(tmp1, 20);
        sprintf(tmp1, "%d", pri.Price[j - 1]);
        write_conf_value(tmp, tmp1, Pri_ConfName);
    }

    tmp2[0]=0x40;
    tmp2[1]=0x30+j;
    HEXtoBCD(pri.Price[j - 1],6);
//    if(Uart_Tx_Flag==1)
//    {
//        Uart_Tx_Flag=0;
//    }
//    Lock(&Data_To_Uart);
//    FCS_To_JKOPT_52(2,tmp2,HEX_BCD);
//    Unlock(&Data_To_Uart);
    if(j==4)
    {
        if(Uart_Tx_Flag==1)
        {
            Uart_Tx_Flag=0;
        }
        DownLoadPrice=1;
        Lock(&Data_To_Uart);
        FCS_To_JKOPT_75();
        Unlock(&Data_To_Uart);
    }

    Time_Process();
    for(i = 0; i < 6; i++)              //FSC时间
    {
        price.V_D_T[i] = para.FSC_TIME[i];
    }
    price.FIE_NU = 1;                   //油价记录数
    price.NZN = para.NZN;
    price.PRC_N = 8;
}

/*回复fs变价成功*/
void Reply_FS_DownLoad_Price_Success()
{
    Uchar tt = 0;
    /*02.01.01.01.00.80.00.0C.03.21.41.99.64.00.01.01.99.05.01.02*/
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;                //长度高位
    Uart_To_FS[tt++] = 0x0C;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x41;
    Uart_To_FS[tt++] = 0x99;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x99;
    Uart_To_FS[tt++] = 0x05;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = para.ST;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*上传实时数据*/
void Reply_FS_UploadRealtimeData()
{
    Uchar tt = 0, i;
    /*
    02.01.01.03.00.80.00.12.01.21.
    66.00.
    22.05.06.00.00.01.41.
    23.05.06.00.00.00.17.
    */
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;                //长度高位
    Uart_To_FS[tt++] = 0x12;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x66;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x22;
    HEXtoFsData_New(para.AMN, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x23;
    HEXtoFsData_New(para.VOL, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*上传交易记录*/
void Reply_FS_UploadOverFuel()
{
    Uchar i, tt = 0;
    /*
    //加气完成
    02.01.01.03.00.80.00.20.04.21.21.29.97.
    64.00.
    01.02.29.97.
    15.01.02.
    14.02.00.00.
    05.05.06.00.00.71.13.
    06.05.06.00.00.08.57
    */
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;                //长度高位
    Uart_To_FS[tt++] = 0x20;
    Uart_To_FS[tt++] = 0x04;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x21;
    HEXtoBCD(para.POS_TTC, 2);
    Uart_To_FS[tt++] = HEX_BCD[0];
    Uart_To_FS[tt++] = HEX_BCD[1];
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = HEX_BCD[0];
    Uart_To_FS[tt++] = HEX_BCD[1];
    Uart_To_FS[tt++] = 0x15;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x02;        //传输状态，可能需要修改
    Uart_To_FS[tt++] = 0x14;
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x05;
    P_Log("==============AMN=%d===============\n", para.AMN);
    HEXtoFsData_New(para.AMN, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x06;
    P_Log("===============VOL=%d===============\n", para.VOL);
    HEXtoFsData_New(para.VOL, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*授权加气机*/
void Reply_FS_Auth_OPT()
{
    //01.03.02.01.00.4C.00.12.01.21.1B.05.05.00.10.00.00.1E.02.02.01.19.01.01.3E.00
    //01.03.02.01.00.47.00.0B.01.21.1E.02.02.01.19.01.01.3E.00
    //02.01.01.04.00.F1.00.03.01.21.00
    Uchar tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv51_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*拒绝授权加气机*/
void Reply_FS_CantAuth_OPT()
{
    //01.03.02.01.00.4C.00.12.01.21.1B.05.05.00.10.00.00.1E.02.02.01.19.01.01.3E.00
    //02.01.01.03.00.EC.00.0B.01.21.05.1B.00.1E.00.19.00.3E.03
    Uchar tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv51_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x0B;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x05;
    Uart_To_FS[tt++] = 0x1B;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x1E;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x19;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x3E;
    Uart_To_FS[tt++] = 0x03;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/*停止加气机*/
void Reply_FS_Stop_OPT()
{
    //01.03.02.01.00.5B.00.04.01.21.40.00
    //02.01.01.03.00.FB.00.03.01.21.00
    Uchar tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv5A_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*下载油品代码*/
void Reply_FS_DownloadOilType(Uchar flag)
{
    //01.03.02.01.00.47.00.08.01.41.02.04.00.00.00.08
    //02.01.01.03.00.E7.00.03.01.41.00.
    Uchar i, tt = 7;

    for(i = 0; i < 4; i++)
    {
        fp_id.Current_Prod_Nb[i] = FS_To_Uart[i + 12];
    }
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;

}

/*清除交易记录*/
void Reply_FS_ClearTran()
{
    /*
    01.03.02.01.00.47.00.09.04.21.21.30.20.1F.00.1E.00
    02.01.01.03.00.E7.00.06.04.21.21.30.20.00.
    02.01.01.03.00.80.00.20.04.21.21.30.20.
    64.00.
    01.02.30.20.
    15.01.01.
    14.02.02.01.
    05.05.06.00.00.17.26.
    06.05.06.00.00.02.08.
    */
    Uchar i, tt = 7;
    Uart_To_FS[tt++] = 0x06;
    Uart_To_FS[tt++] = 0x04;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = FS_To_Uart[11];
    Uart_To_FS[tt++] = FS_To_Uart[12];
    Uart_To_FS[tt++] = 0x00;

    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;                //长度高位
    Uart_To_FS[tt++] = 0x20;
    Uart_To_FS[tt++] = 0x04;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = FS_To_Uart[11];
    Uart_To_FS[tt++] = FS_To_Uart[12];
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = FS_To_Uart[11];
    Uart_To_FS[tt++] = FS_To_Uart[12];
    Uart_To_FS[tt++] = 0x15;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;        //传输状态，可能需要修改
    Uart_To_FS[tt++] = 0x14;
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = 0x05;
    P_Log("=================AMN=%d================\n", para.AMN);
    HEXtoFsData_New(para.AMN, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x06;
    P_Log("=================VOL=%d================\n", para.VOL);
    HEXtoFsData_New(para.VOL, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    para.VOL=0;//清除交易后将其清零，防止第二次传了第一次的值
    para.AMN=0;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;

}

/*默认回复fuel的其他消息*/
void Reply_FS_OtherMsg()
{
    //01.02.02.01.00.44.00.04.01.21.3D.00
    //01.03.02.01.00.46.00.09.02.21.11.01.01.01.07.01.01
    //02.01.01.02.00.E4.00.03.01.21.00
    //02.01.01.03.00.E6.00.04.02.21.11.00
    Uchar i, len = 0, tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Status_Code;
    Uart_To_FS[tt++] = 0x00;                        //长度高位
    len = FS_To_Uart[8];
    Uart_To_FS[tt++] = 2 + len;                     //长度低位
    Uart_To_FS[tt++] = len;
    for(i = 0; i < len; i++)
    {
        Uart_To_FS[tt++] = FS_To_Uart[9 + i];
    }
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/********************************************************************\
* 函数名: Deal_FS_To_Uart_data
* 说明:     处理FS发送给加气机的数据
* 功能:     FS到串口数据处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-01
\*********************************************************************/
void Deal_FS_To_Uart_data()
{
    Uchar i, tt, data_len, database_len, data_base_name, tmp[10];
    tt = 0;
    Uart_To_FS[tt++] = FS_To_Uart[2];
    Uart_To_FS[tt++] = FS_To_Uart[3];
    Uart_To_FS[tt++] = FS_To_Uart[0];
    Uart_To_FS[tt++] = FS_To_Uart[1];
    Uart_To_FS[tt++] = 0;                               //消息代码
    Uart_To_FS[tt++] = Deal_Status_Code(FS_To_Uart[5]); //消息状态字
    Status_Code = Deal_Status_Code(FS_To_Uart[5]);      //消息状态字
    Uart_To_FS[tt++] = 0;                               //长度高位
    data_len = FS_To_Uart[7];                           //数据长度
    database_len = FS_To_Uart[8];                       //数据库长度
    data_base_name = FS_To_Uart[9];                     //第一个数据库名称
    fp_id.FP_State = para.ST;
    conf.FS_NODE[0] = FS_To_Uart[2];
    conf.FS_NODE[1] = FS_To_Uart[3];
    conf.MY_NODE[0] = FS_To_Uart[0];
    conf.MY_NODE[1] = FS_To_Uart[1];
    if((data_len == 0x06) && (database_len == 0x01) && (data_base_name == 0))   //下载通讯数据库
    {
        //01.03.02.01.02.4B.00.06.01.00.0B.02.02.01.
        //02.01.01.03.00.EB.00.03.01.00.00
        HaveAuth=0;//重新连接时清除之前可能已授权的标识
        Uart_To_FS[tt++] = 0x03;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x00;
//        conf.FS_NODE[0] = FS_To_Uart[2];
//        conf.FS_NODE[1] = FS_To_Uart[3];
//        conf.MY_NODE[0] = FS_To_Uart[0];
//        conf.MY_NODE[1] = FS_To_Uart[1];
        FS_Data_Len = tt;
        Uart_To_FS_Flag = 1;
    }
    else if((data_len == 0x03) && (database_len == 0x01) && (data_base_name == 0x20))     //读加气机状态
    {
        Reply_FS_ReadStatus();
    }
    else if((data_len == 0x03) && (database_len == 0x01) && (data_base_name == 0x21))     //读加气机状态
    {
        Reply_FS_StaIsChanged();
    }
    else if((data_len == 0x08) && (database_len == 0x01))     //清除和下载油品代码
    {
        //01.03.02.01.00.51.00.08.01.41.02.04.00.00.00.00. (41-48)
        //02.01.01.03.00.F1.00.03.01.41.00.
        if(FS_To_Uart[15] != 0)   //下载油品代码
        {
            for(i = 0; i < 4; i++)
            {
                price.O_TYPE[i] = FS_To_Uart[12 + i];
            }
        }
        Uart_To_FS[tt++] = 0x03;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = FS_To_Uart[9];
        Uart_To_FS[tt++] = 0x00;
        FS_Data_Len = tt;
        Uart_To_FS_Flag = 1;
    }
    else if((data_len == 0x05) && (database_len == 0x01) && (data_base_name == 0x81))     //选择油品地址
    {
        Reply_FS_SelectOilAddr();
    }
    else if((data_len == 0x0B) && (database_len == 0x01) && (data_base_name == 0x01) && (FS_To_Uart[10] == 0x0B))     //写C_DAT数据库中的0B、16、17
    {
        Reply_FS_Write0B1617();
    }
    else if((data_len == 0x06) && (database_len == 0x02) && (data_base_name == 0x21) && (FS_To_Uart[10] == 0x11))     //读累计
    {
        Recv57_Flag = Deal_Status_Code(FS_To_Uart[5]);  //消息状态字
        Lock(&Data_To_Uart);
        OPT_72_Flag=1;
        ReadTotal_Flag=1;
        FCS_To_JKOPT_72();
        Unlock(&Data_To_Uart);
    }
    else if((database_len == 0x04) && (data_base_name == 0x21) && (FS_To_Uart[10] == 0x20))     //从FP中读取未支付交易
    {
        //Reply_FS_ReadNO_Pay_Trans();
        if((ReadNoPayFlag==0)&&(para.NO_Payment_Trans!=0)&&(FS_OK!=1))
        {
            ReadNoPayFlag=1;
        }
        else if((para.T_TYPE!=0)||(para.NO_Payment_Trans<2))//已经上传交易后才能让fs读取
        {
            Reply_FS_ReadNO_Pay_Trans();
        }
    }
    else if((data_len == 0x09) && (database_len == 0x02) && (data_base_name == 0x21) && (FS_To_Uart[10] == 0x11))     //写油枪数据库
    {
        Reply_FS_WriteOilDataBase();
    }
    else if((data_len == 0x05) && (database_len == 0x01) && (data_base_name == 0x21) && (FS_To_Uart[10] == 0x07))     //下次交易模式
    {
        //01.03.02.01.00.5C.00.05.01.21.07.01.02.
        //02.01.01.03.00.FC.00.03.01.21.00.
        fp_id.Default_Fuelling_Mode = FS_To_Uart[12];
        Uart_To_FS[tt++] = 0x03;                        //长度低位
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x00;
        FS_Data_Len = tt;
        Uart_To_FS_Flag = 1;
    }
    else if((data_len == 0x04) && (database_len == 0x01) && (data_base_name == 0x21))
    {
        if(FS_To_Uart[10] == 0x3C)   //打开一个关闭的FP
        {
            Reply_FS_OtherMsg();
            FS_OK = 1;
            usleep(300 * 1000);
            Reply_FS_UploadStatus(3);
        }
        else if(FS_To_Uart[10] == 0x3D)     //关闭一个FP
        {
            Reply_FS_OtherMsg();
            usleep(300 * 1000);
            //Reply_FS_UploadStatus(2);
            FS_OK = 2;
        }
        else if(FS_To_Uart[10] == 0x40)     //停止加气机加气
        {
            Recv5A_Flag = Deal_Status_Code(FS_To_Uart[5]);  //消息状态字
//            if(Uart_Tx_Flag==1)
//            {
//                Uart_Tx_Flag=0;
//            }
//            usleep(600 * 1000);
            if(F0_Time%8<3)
               usleep(200 * 1000);
            Lock(&Data_To_Uart);
            FCS_To_JKOPT_73();
            Unlock(&Data_To_Uart);
        }
    }
    else if((database_len == 0x06) && (data_base_name == 0x61))     //下载单价
    {
        Reply_FS_DownLoad_Price();
    }
    else if((database_len == 0x01) && (data_base_name == 0x21) && ((FS_To_Uart[10] == 0x1B) || (FS_To_Uart[10] == 0x1C) || (FS_To_Uart[10] == 0x1E)))     //授权金额/授权气量/加气机直接加气
    {
        if(FS_To_Uart[10] != 0x1E)
        {
            if(FS_To_Uart[10] == 0x1B)
            {
                Auth_Type = 0;
            }
            else
            {
                Auth_Type = 1;
            }
            for(i = 0; i < FS_To_Uart[11]; i++)
            {
                tmp[i] = FS_To_Uart[12 + i];
            }
            Auth_Value = FSDataToLong(FS_To_Uart[11], tmp);
        }
        else
        {
            Auth_Type = 2; //加气机直接加气
            Auth_Value = 0;
        }
        Recv51_Flag = Deal_Status_Code(FS_To_Uart[5]);  //消息状态字
        RecvFSAuth=1;
        Lock(&Data_To_Uart);
        HEXtoBCD(Auth_Value,6);
        if(F0_Time%8<3)
           usleep(200 * 1000);
        FCS_To_JKOPT_70(Auth_Type, HEX_BCD);
        Unlock(&Data_To_Uart);
    }
    else if((data_len == 0x09) && (database_len == 0x04) && (data_base_name == 0x21) && (FS_To_Uart[10] == 0x21))     //清除交易记录
    {
        BoartRequestFlag=1;//等于1时，主板开始申请支付
        Reply_FS_ClearTran();
    }
    else
    {
        Reply_FS_OtherMsg();
    }
}

/********************************************************************\
* 函数名: Deal_EPS_To_Uart_data
* 说明:     处理EPS发送给加气机的数据
* 功能:     EPS到串口数据处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-01
\*********************************************************************/
void Deal_EPS_To_Uart_data()
{
    if(F0_Time%8<3)
       usleep(200 * 1000);
    FCS_To_JKOPT_71();
    Uart_Tx_Flag = 1;
}

/********************************************************************\
* 函数名：: Uart_Receive_Thread
* 说明:
* 功能:     串口数据接收处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间:  2014-8-22
\*********************************************************************/
void Uart_Receive_Thread(void)
{
    int nread;
    while(1)
    {
        bzero(Uart_Rx_Buff, LENGTH);
        if((nread = read(Uart_fd, Uart_Rx_Buff, LENGTH)) > 0)   //接收数据
        {
            Uart_Data_Len = nread;
            Print_HEX("Uart_Receive_Thread Recv", nread, Uart_Rx_Buff);
            Lock(&Uart_Recv_Data);
            Deal_Uart_Recv_Data();
            Unlock(&Uart_Recv_Data);
        }
        //usleep(10*1000);
    }
    return;
}
void Uart_Receive_Thread_Select()
{
    int nread, ret;
    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = 0;         //无阻塞
    timeout.tv_usec = 0;
    while(1)
    {
        FD_ZERO(&readfds);       //每次循环都要清空集合，select检测描述文件中是否有可读的，从而能检测描述符变化
        FD_SET(Uart_fd, &readfds);
        ret = select(Uart_fd + 1, &readfds, NULL, NULL, &timeout); //select检测描述文件中是否有可读的
        usleep(30 * 1000); //等待数据收完
        if(ret < 0)
        {
            printf("select error!\n");
        }
        //检测读文件描述符集合，一直在循环，监视描述符的变化
        ret = FD_ISSET(Uart_fd, &readfds);
        if(ret > 0)
        {
            bzero(Uart_Rx_Buff, LENGTH);
            if((nread = read(Uart_fd, Uart_Rx_Buff, LENGTH)) > 0)   //接收数据
            {
                Uart_Data_Len = nread;
                Print_HEX("Uart_Receive_Thread Recv", nread, Uart_Rx_Buff);
                Lock(&Uart_Recv_Data);
                Deal_Uart_Recv_Data();
                Unlock(&Uart_Recv_Data);
            }
        }
    }
    return;
}



/********************************************************************\
* 函数名：: Uart_send_Thread
* 说明:
* 功能:     串口数据发送处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间:  2014-8-22
\*********************************************************************/
void Uart_send_Thread(void)
{
    while(1)
    {
        //scanf("%s", Uart_TX_Buff);
        if(Uart_Tx_Flag == 1)
        {
            //发送数据
//            if(F0_Time%8<4)
//                usleep(200 * 1000);
            Uart_Tx_Flag = 0;
            Lock(&Data_To_Uart);
            write(Uart_fd, Uart_Tx_Buff, Uart_Data_Len);
//            F0_Time=1;
            bzero(Uart_Tx_Buff, LENGTH);
            Unlock(&Data_To_Uart);
        }
        usleep(100 * 1000);
    }
    return;
}


/********************************************************************\
* 函数名: Uart_Pthread_Creat
* 说明:
* 功能:  串口的接受和发送线程创建
* 输入:
* 返回值:   0:成功非0:失败
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-22
\*********************************************************************/
int Uart_Pthread_Creat()
{
    int err;
    pthread_t receiveid, send;
    err = pthread_create(&receiveid, NULL, (void*)Uart_Receive_Thread_Select, NULL); //创建接收线程
    if(err != 0)
    {
        P_Log("can't create Uart_Receive thread thread: %s\n", strerror(err));
        return 1;
    }
    else
        P_Log("create Uart_Receive thread thread OK\n");
    err = pthread_create(&send, NULL, (void*)Uart_send_Thread, NULL); //创建发送线程
    if(err != 0)
    {
        P_Log("can't create Uart_send thread: %s\n", strerror(err));
        return 1;
    }
    else
        P_Log("create Uart_send thread OK\n");

    return 0;
}
