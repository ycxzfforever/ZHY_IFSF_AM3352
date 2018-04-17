#include "common.h"


/********************************************************************\
* 函数名: Time_Process
* 说明:
* 功能:    将时间转换成7字节的yyyymmddhhmmss
* 输入:
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-9-18
\*********************************************************************/
void Time_Process()
{
    ftime(&tb);
    now=localtime(&tb.time);
    para.FSC_TIME[0]=((now->tm_year+1900)/1000)*16+((now->tm_year+1900)/100)%10;
    para.FSC_TIME[1]=((now->tm_year+1900)%100)/10*16+((now->tm_year+1900)%100)%10;
    para.FSC_TIME[2]=(now->tm_mon+1)/10*16+(now->tm_mon+1)%10;
    para.FSC_TIME[3]=(now->tm_mday)/10*16+(now->tm_mday)%10;
    para.FSC_TIME[4]=(now->tm_hour)/10*16+(now->tm_hour)%10;
    para.FSC_TIME[5]=(now->tm_min)/10*16+(now->tm_min)%10;
    para.FSC_TIME[6]=(now->tm_sec)/10*16+(now->tm_sec)%10;
}

void FCS_To_OPT_Header_Send(Uchar cmdID)//帧头
{
    CmdFlag=cmdID;
    Uart_Tx_Buff[0] = 0xAA;       //起始符 AAH
    Uart_Tx_Buff[1] = 0xFF;       //起始符 FFH
    Uart_Tx_Buff[2] = 0x15;       //目标设备 （00H PC机）（15H 加气机）（20H 通信器）
    Uart_Tx_Buff[3] = cmdID;      //命令 01H~90H
    Uart_Tx_Buff[4] = para.NZN;   //抢号 0~32
}

void FCS_To_OPT_Tail_Send(Uchar cmdID,Uchar len)//帧尾
{
    char str[15];
    Uart_Tx_Buff[5] = len;
    Uart_Tx_Buff[len+6] = int_crc8((Uchar*)&Uart_Tx_Buff[2],len+4);
    Uart_Tx_Buff[len+7] = 0xCC; //结束符CC
    Uart_Tx_Buff[len+8] = 0xFF; //结束符FF
    Uart_Tx_Flag=1;
    Uart_Data_Len=len+9;
    sprintf(str,"FCS_To_OPT_%02X",cmdID);
    Print_HEX(str,Uart_Data_Len,Uart_Tx_Buff);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_51
// 功能:   FSC 轮训加气机
// 输入:
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_51()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x51);
    Time_Process();
    for(i=0; i<6; i++)      //时间
    {
        Uart_Tx_Buff[tt++]=para.FSC_TIME[i+1];
    }
    if(Stop_Opt==1)
        Uart_Tx_Buff[tt++]=0;
    else
        Uart_Tx_Buff[tt++]=FS_OK;                    //fs状态
    Uart_Tx_Buff[tt++]=ReadNoPayFlag;                //读取未支付交易标识
    Uart_Tx_Buff[tt++]=fp_id.Default_Fuelling_Mode;  //模式选择
    Uart_Tx_Buff[tt++]=BoartRequestFlag;
    for(i=0; i<6; i++)                               //备用
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x51,tt-6);

    if(OPT_IS_Online)
        OPT_IS_Online--;
}

//********************************************************************
// 函数名: FCS_To_JKOPT_52
// 功能:   FSC 设置加气机参数
// 输入:    no:小数位数(BCD)，ParaNo:参数索引(BCD)，paravalue:参数值(BCD)
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_52(Uchar no,Uchar * ParaNo,Uchar * paravalue)
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x52);
    Uart_Tx_Buff[tt++]=no;  //小数位数
    for(i=0; i<2; i++)      //参数索引
    {
        Uart_Tx_Buff[tt++]=ParaNo[i];
    }
    for(i=0; i<6; i++)      //参数值
    {
        Uart_Tx_Buff[tt++]=paravalue[i];
    }
    for(i=0; i<6; i++)      //备用
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x52,tt-6);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_69
// 功能:   FSC 回应加气机上传的记录
// 输入:   UID:流水号
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_69(Uchar * UID)
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x69);
    Uart_Tx_Buff[tt++]=0x11;//只回应加气流水记录
    for(i=0; i<2; i++)      //备用
    {
        Uart_Tx_Buff[tt++]=0;
    }
    for(i=0; i<6; i++)      //UID bcd
    {
        Uart_Tx_Buff[tt++]=UID[i];
    }
    FCS_To_OPT_Tail_Send(0x69,tt-6);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_70
// 功能:    FSC 回复加气机申请授权
// 输入:    flag:0=授权金额，1=授权气量,num:授权值
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_70(Uchar flag, Uchar * num)
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x70);
    Uart_Tx_Buff[tt++]=flag;            //授权类型
    for(i=0; i<6; i++)                  //授权值
    {
        Uart_Tx_Buff[tt++]=num[i];
    }
    FCS_To_OPT_Tail_Send(0x70,tt-6);
    F0_Time=1;//防止接连两次发授权命令
}

//********************************************************************
// 函数名: FCS_To_JKOPT_71
// 功能:    FSC 回复EPS数据给加气机
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_71()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x71);
    for(i=0; i<EPS_Data_Len; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_To_Uart[i];
    }
    if((EPS_Data_Len==88)&&(HuiSuoFlag==0))//  EPS验卡返回长度
    {
        YanKaFlag=1;
        OverTransFlag=0;
        for(i=0; i<EPS_Data_Len; i++)
        {
            EPS_Bank_Buf[i]=EPS_To_Uart[i];
        }
    }
    else if((EPS_Data_Len==38)&&(YanKaFlag==1))//  EPS通知灰锁结果返回长度
    {
        HuiSuoFlag=1;
        YanKaFlag=0;
        for(i=0; i<EPS_Data_Len; i++)
        {
            EPS_Bank_Buf[i]=EPS_To_Uart[i];
        }
    }
    else if((EPS_Data_Len==152)&&(OverTransFlag==0))//  EPS返回加气机申请支付长度
    {
        RequestPayFlag=1;
        HuiSuoFlag=0;
        for(i=0; i<EPS_Data_Len; i++)
        {
            EPS_Bank_Buf[i]=EPS_To_Uart[i];
        }
    }
    else if(EPS_Data_Len==36)//  EPS返回加气机通知交易结果长度
    {
        OverTransFlag=1;
        RequestPayFlag=0;
        for(i=0; i<EPS_Data_Len; i++)
        {
            EPS_Bank_Buf[i]=EPS_To_Uart[i];
        }
    }
    FCS_To_OPT_Tail_Send(0x71,tt-6);
}

//验卡已经返回时，直接调用此函数
void YanKaReturn()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x71);
    for(i=0; i<88; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_Bank_Buf[i];
    }
    FCS_To_OPT_Tail_Send(0x71,tt-6);
}

//灰锁已经返回时，直接调用此函数
void HuiSuoReturn()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x71);
    for(i=0; i<38; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_Bank_Buf[i];
    }
    FCS_To_OPT_Tail_Send(0x71,tt-6);
}

//申请支付已经返回时，直接调用此函数
void RequestPayReturn()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x71);
    for(i=0; i<152; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_Bank_Buf[i];
    }
    FCS_To_OPT_Tail_Send(0x71,tt-6);
}

//通知交易结果已经返回时，直接调用此函数
void OverTransReturn()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x71);
    for(i=0; i<36; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_Bank_Buf[i];
    }
    FCS_To_OPT_Tail_Send(0x71,tt-6);
}


//********************************************************************
// 函数名: FCS_To_JKOPT_72
// 功能:    FSC 读取加气机累计数(加气机上传最后一条流水)
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_72()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x72);
    for(i=0; i<6; i++)
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x72,tt-6);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_73
// 功能:    FSC 停止加气机
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_73()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x73);
    for(i=0; i<6; i++)
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x73,tt-6);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_74
// 功能:    FSC 回复加气机上传记录
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-08
//*********************************************************************
void FCS_To_JKOPT_74()
{
    Uchar tt=6,i;
    FCS_To_OPT_Header_Send(0x74);
    for(i=0; i<6; i++)
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x74,tt-6);
}

//********************************************************************
// 函数名: FCS_To_JKOPT_75
// 功能:    FSC下发单价到加气机(下发四个单价)
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-08
//*********************************************************************
void FCS_To_JKOPT_75()
{
    Uchar tt=6,i;
    FCS_To_OPT_Header_Send(0x75);
    Uart_Tx_Buff[tt++]=2;       //小数位数  6
    Uart_Tx_Buff[tt++]=0x40;    //参数索引 7 
    Uart_Tx_Buff[tt++]=0x31;    //参数索引  8        
    HEXtoBCD(price.Price[0],6); //参数值 
    for(i=0; i<6; i++)          //9
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    
    Uart_Tx_Buff[tt++]=2;       //小数位数  15
    Uart_Tx_Buff[tt++]=0x40;    //参数索引  
    Uart_Tx_Buff[tt++]=0x32;    //参数索引           
    HEXtoBCD(price.Price[1],6); //参数值
    for(i=0; i<6; i++)          
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    
    Uart_Tx_Buff[tt++]=2;       //小数位数  24
    Uart_Tx_Buff[tt++]=0x40;    //参数索引
    Uart_Tx_Buff[tt++]=0x33;    //参数索引          
    HEXtoBCD(price.Price[2],6); //参数值
    for(i=0; i<6; i++)         
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    
    Uart_Tx_Buff[tt++]=2;       //小数位数  33
    Uart_Tx_Buff[tt++]=0x40;    //参数索引
    Uart_Tx_Buff[tt++]=0x34;    //参数索引           
    HEXtoBCD(price.Price[3],6); //参数值
    for(i=0; i<6; i++)          
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    FCS_To_OPT_Tail_Send(0x75,tt-6);
}


//********************************************************************
// 函数名: FCS_To_JKOPT_76
// 功能:    FSC 回复加气机上传参数
// 输入:   无
// 返回值:   无
// 创建人:   Yang Chao Xu
// 创建时间: 2015-06-12
//*********************************************************************
void FCS_To_JKOPT_76()
{
    Uchar tt=6,i;
    FCS_To_OPT_Header_Send(0x76);
    for(i=0; i<6; i++)
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x76,tt-6);
}


