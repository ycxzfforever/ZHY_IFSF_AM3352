#include "common.h"


/********************************************************************\
* ������: Time_Process
* ˵��:
* ����:    ��ʱ��ת����7�ֽڵ�yyyymmddhhmmss
* ����:
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-9-18
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

void FCS_To_OPT_Header_Send(Uchar cmdID)//֡ͷ
{
    CmdFlag=cmdID;
    Uart_Tx_Buff[0] = 0xAA;       //��ʼ�� AAH
    Uart_Tx_Buff[1] = 0xFF;       //��ʼ�� FFH
    Uart_Tx_Buff[2] = 0x15;       //Ŀ���豸 ��00H PC������15H ����������20H ͨ������
    Uart_Tx_Buff[3] = cmdID;      //���� 01H~90H
    Uart_Tx_Buff[4] = para.NZN;   //���� 0~32
}

void FCS_To_OPT_Tail_Send(Uchar cmdID,Uchar len)//֡β
{
    char str[15];
    Uart_Tx_Buff[5] = len;
    Uart_Tx_Buff[len+6] = int_crc8((Uchar*)&Uart_Tx_Buff[2],len+4);
    Uart_Tx_Buff[len+7] = 0xCC; //������CC
    Uart_Tx_Buff[len+8] = 0xFF; //������FF
    Uart_Tx_Flag=1;
    Uart_Data_Len=len+9;
    sprintf(str,"FCS_To_OPT_%02X",cmdID);
    Print_HEX(str,Uart_Data_Len,Uart_Tx_Buff);
}

//********************************************************************
// ������: FCS_To_JKOPT_51
// ����:   FSC ��ѵ������
// ����:
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_51()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x51);
    Time_Process();
    for(i=0; i<6; i++)      //ʱ��
    {
        Uart_Tx_Buff[tt++]=para.FSC_TIME[i+1];
    }
    if(Stop_Opt==1)
        Uart_Tx_Buff[tt++]=0;
    else
        Uart_Tx_Buff[tt++]=FS_OK;                    //fs״̬
    Uart_Tx_Buff[tt++]=ReadNoPayFlag;                //��ȡδ֧�����ױ�ʶ
    Uart_Tx_Buff[tt++]=fp_id.Default_Fuelling_Mode;  //ģʽѡ��
    Uart_Tx_Buff[tt++]=BoartRequestFlag;
    for(i=0; i<6; i++)                               //����
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x51,tt-6);

    if(OPT_IS_Online)
        OPT_IS_Online--;
}

//********************************************************************
// ������: FCS_To_JKOPT_52
// ����:   FSC ���ü���������
// ����:    no:С��λ��(BCD)��ParaNo:��������(BCD)��paravalue:����ֵ(BCD)
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_52(Uchar no,Uchar * ParaNo,Uchar * paravalue)
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x52);
    Uart_Tx_Buff[tt++]=no;  //С��λ��
    for(i=0; i<2; i++)      //��������
    {
        Uart_Tx_Buff[tt++]=ParaNo[i];
    }
    for(i=0; i<6; i++)      //����ֵ
    {
        Uart_Tx_Buff[tt++]=paravalue[i];
    }
    for(i=0; i<6; i++)      //����
    {
        Uart_Tx_Buff[tt++]=0;
    }
    FCS_To_OPT_Tail_Send(0x52,tt-6);
}

//********************************************************************
// ������: FCS_To_JKOPT_69
// ����:   FSC ��Ӧ�������ϴ��ļ�¼
// ����:   UID:��ˮ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_69(Uchar * UID)
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x69);
    Uart_Tx_Buff[tt++]=0x11;//ֻ��Ӧ������ˮ��¼
    for(i=0; i<2; i++)      //����
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
// ������: FCS_To_JKOPT_70
// ����:    FSC �ظ�������������Ȩ
// ����:    flag:0=��Ȩ��1=��Ȩ����,num:��Ȩֵ
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_70(Uchar flag, Uchar * num)
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x70);
    Uart_Tx_Buff[tt++]=flag;            //��Ȩ����
    for(i=0; i<6; i++)                  //��Ȩֵ
    {
        Uart_Tx_Buff[tt++]=num[i];
    }
    FCS_To_OPT_Tail_Send(0x70,tt-6);
    F0_Time=1;//��ֹ�������η���Ȩ����
}

//********************************************************************
// ������: FCS_To_JKOPT_71
// ����:    FSC �ظ�EPS���ݸ�������
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_71()
{
    Uchar tt=6,i=0;
    FCS_To_OPT_Header_Send(0x71);
    for(i=0; i<EPS_Data_Len; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_To_Uart[i];
    }
    if((EPS_Data_Len==88)&&(HuiSuoFlag==0))//  EPS�鿨���س���
    {
        YanKaFlag=1;
        OverTransFlag=0;
        for(i=0; i<EPS_Data_Len; i++)
        {
            EPS_Bank_Buf[i]=EPS_To_Uart[i];
        }
    }
    else if((EPS_Data_Len==38)&&(YanKaFlag==1))//  EPS֪ͨ����������س���
    {
        HuiSuoFlag=1;
        YanKaFlag=0;
        for(i=0; i<EPS_Data_Len; i++)
        {
            EPS_Bank_Buf[i]=EPS_To_Uart[i];
        }
    }
    else if((EPS_Data_Len==152)&&(OverTransFlag==0))//  EPS���ؼ���������֧������
    {
        RequestPayFlag=1;
        HuiSuoFlag=0;
        for(i=0; i<EPS_Data_Len; i++)
        {
            EPS_Bank_Buf[i]=EPS_To_Uart[i];
        }
    }
    else if(EPS_Data_Len==36)//  EPS���ؼ�����֪ͨ���׽������
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

//�鿨�Ѿ�����ʱ��ֱ�ӵ��ô˺���
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

//�����Ѿ�����ʱ��ֱ�ӵ��ô˺���
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

//����֧���Ѿ�����ʱ��ֱ�ӵ��ô˺���
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

//֪ͨ���׽���Ѿ�����ʱ��ֱ�ӵ��ô˺���
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
// ������: FCS_To_JKOPT_72
// ����:    FSC ��ȡ�������ۼ���(�������ϴ����һ����ˮ)
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
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
// ������: FCS_To_JKOPT_73
// ����:    FSC ֹͣ������
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
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
// ������: FCS_To_JKOPT_74
// ����:    FSC �ظ��������ϴ���¼
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-08
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
// ������: FCS_To_JKOPT_75
// ����:    FSC�·����۵�������(�·��ĸ�����)
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-08
//*********************************************************************
void FCS_To_JKOPT_75()
{
    Uchar tt=6,i;
    FCS_To_OPT_Header_Send(0x75);
    Uart_Tx_Buff[tt++]=2;       //С��λ��  6
    Uart_Tx_Buff[tt++]=0x40;    //�������� 7 
    Uart_Tx_Buff[tt++]=0x31;    //��������  8        
    HEXtoBCD(price.Price[0],6); //����ֵ 
    for(i=0; i<6; i++)          //9
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    
    Uart_Tx_Buff[tt++]=2;       //С��λ��  15
    Uart_Tx_Buff[tt++]=0x40;    //��������  
    Uart_Tx_Buff[tt++]=0x32;    //��������           
    HEXtoBCD(price.Price[1],6); //����ֵ
    for(i=0; i<6; i++)          
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    
    Uart_Tx_Buff[tt++]=2;       //С��λ��  24
    Uart_Tx_Buff[tt++]=0x40;    //��������
    Uart_Tx_Buff[tt++]=0x33;    //��������          
    HEXtoBCD(price.Price[2],6); //����ֵ
    for(i=0; i<6; i++)         
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    
    Uart_Tx_Buff[tt++]=2;       //С��λ��  33
    Uart_Tx_Buff[tt++]=0x40;    //��������
    Uart_Tx_Buff[tt++]=0x34;    //��������           
    HEXtoBCD(price.Price[3],6); //����ֵ
    for(i=0; i<6; i++)          
    {
        Uart_Tx_Buff[tt++]=HEX_BCD[i];
    }
    FCS_To_OPT_Tail_Send(0x75,tt-6);
}


//********************************************************************
// ������: FCS_To_JKOPT_76
// ����:    FSC �ظ��������ϴ�����
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-12
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


