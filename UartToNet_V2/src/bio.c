#include "common.h"


/********************************************************************\
* 函数名：: Open_Uart
* 说明:
* 功能:     打开串口
* 输入:     串口号
* 返回值:   0:成功-1:失败
* 创建人:   Yang Chao Xu
* 创建时间:  2014-8-22
\*********************************************************************/
int Open_Uart(char *uart_no)
{
    Uart_fd= open(uart_no, O_RDWR);
    if(Uart_fd < 0)
    {
        P_Log("open device %s faild\n", uart_no);
        return(-1);
    }
    return 0;
}


/********************************************************************\
* 函数名：: Str_Split
* 说明:
* 功能:     拆分字符串
* 输入:     需要拆分的字符串
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-10
\*********************************************************************/
void Str_Split(char *str)
{

    char input[16];
    char *p;
    int i=0;
    strcpy(input,str);
    p=strtok(input,".");
    while(p)
    {
        Tmp_Buf[i++]=atoi(p);
        p=strtok(NULL,".");
    }
}

/********************************************************************\
* 函数名: Print_HEX
* 说明:
* 功能:    将字符串str按照16进制打印
* 输入:     len:  str长度
             str:     需要打印的字符串
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-9-12
\*********************************************************************/
void Print_HEX(const char *src, int len,char *str)
{
    char tmp[1024];
    char tmp1[4];
    int i=0;
    bzero(tmp,1024);
    while(i<len)
    {
        sprintf(tmp1,"%02X.",str[i++]);
        strcat(tmp,tmp1);
    }
    P_Log("%s Data:%s len=%d\n",src,tmp,len);
    bzero(tmp,1024);
}


/********************************************************************\
* 函数名:   HEXtoBCD
* 说明:         n字节十六进制转换为n字节BCD码
* 功能:
* 输入:     hex_data:待转换数据
*               n:数组字节数(决定最高位是哪一位)
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-19
\*********************************************************************/
void HEXtoBCD(Ulong hex_data,Uchar n)
{
    Uchar tmp_data;
    Uchar i;
    n%=11;
    for(i=0; i<n; i++)
    {
        tmp_data=(Uchar)(hex_data%100);
        HEX_BCD[n-i-1]=((Uchar)((tmp_data/10)<<4)) | ((Uchar)(tmp_data%10));
        hex_data/=100  ;
    }
}

/********************************************************************\
* 函数名:   BCDtoHEX
* 说明:         多字节BCD码转换为一字节HEX
* 功能:
* 输入:     BCD_DATA:BCD数组(高位在前),
*               n:数组字节数(决定最高位是哪一位)
* 返回值:   HEX数据
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-19
\*********************************************************************/
Uint    BCDtoHEX(Uchar *BCD_DATA,Uchar n)
{
    Uint Uint32Temp,i;
    for(i=0,Uint32Temp=0; i<n; i++)
    {
        Uint32Temp+=(Uint)(BCD_DATA[i]>>4)*pow(10,(n-1-i)*2+1);
        Uint32Temp+=(Uint)(BCD_DATA[i]&0x0f)*pow(10,(n-1-i)*2);
    }
    return(Uint32Temp);
}

/********************************************************************\
* 函数名:   HEXtoFsData
* 说明:         十六进制转换为FS数据
* 功能:
* 输入:     hex_data=输入的一个数，n=小数位数，
*               Flag:0=正数，1=负数
* 返回值:   转换后的整个数组长度
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-19
\*********************************************************************/
char HEXtoFsData(Ulong hex_data,Uchar n,Uchar Flag)
{
    Uchar i=0,j,len=0;
    char tmp[10],tmp1[10];
    if(hex_data)
    {
        while(1)
        {
            tmp[i]=hex_data%100;
            if(hex_data==0)
            {
                break;
            }
            hex_data/=100;
            i++;
        }
        for(j=0; j<i; j++)
        {
            tmp1[j+1]=tmp[i-j-1];
        }
        if(Flag==0)
        {
            HEX_FsData[0]=(2*j-n);
            len=j;
        }
        else
        {
            HEX_FsData[0]=(2*j-n)|0x80;
            if(n%2==0)
                len=j+n%2;
            else
                len=j+n%2+1;
        }
        for(i=1; i<j+1; i++)
        {
            HEX_FsData[i]=tmp1[i]/10*16+tmp1[i]%10;
        }
        return len;
    }
    else
    {
        HEX_FsData[0]=1;
        HEX_FsData[1]=0;
        return 2;
    }
}

//增加的length为数组长度，不够前面补0
char HEXtoFsData_New(Ulong hex_data,Uchar n,Uchar Flag,Uchar length)
{
    Uchar i=0,j,len=0,k;
    char tmp[10],tmp1[10];
    if(hex_data)
    {
        while(1)
        {
            tmp[i]=hex_data%100;
            if(hex_data==0)
            {
                break;
            }
            hex_data/=100;
            i++;
        }
        for(j=0; j<i; j++)
        {
            tmp1[j+1]=tmp[i-j-1];
        }
        if(Flag==0)
        {
            HEX_FsData[0]=(2*j-n);
            len=j;
        }
        else
        {
            HEX_FsData[0]=(2*j-n)|0x80;
            if(n%2==0)
                len=j+n%2;
            else
                len=j+n%2+1;
        }
        for(i=1; i<j+1; i++)
        {
            HEX_FsData[i]=tmp1[i]/10*16+tmp1[i]%10;
        }
        j=0;
        if(len<length)
        {
            k=length-len;
            HEX_FsData[0]+=2*k;
        }
        else
        {
            k=0;
        }
        for(i=0; i<len; i++)
        {
            HEX_FsData[len-i+k]=HEX_FsData[len-i];
        }
        for(i=0; i<k; i++)
        {
            HEX_FsData[i+1]=0;
        }
        return k==0?len:length;
    }
    else
    {
        HEX_FsData[0]=1;
        HEX_FsData[1]=0;
        HEX_FsData[2]=0;
        HEX_FsData[3]=0;
        HEX_FsData[4]=0;
        return 2;
    }
}

/********************************************************************\
* 函数名:   FSDataToLong
* 说明:         FS数据转换为长整形
* 功能:     注意:转换后的值为FS原数据的100倍
* 输入:     num:FS数组长度，
*               buf:FS数据数组
* 返回值:   转换后的值
                如: FS数组06 12 34 56 78
转换后的值就为:        12345678
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-19
\*********************************************************************/
long FSDataToLong(int num,char *buf)
{
    long data=0.0;
    Uchar i,j,Flag,len,tmp[20];
    if(((buf[0]&0xF0)>>4)==0)//正数
        Flag=0;
    else if(((buf[0]&0xF0)>>4)==8)//负数
        Flag=1;
    len=buf[0]&0x0F;
    for(i=0,j=1; i<2*(num-1); i++,j++)
    {
        tmp[i]=(buf[j]&0xF0)>>4;
        tmp[++i]=buf[j]&0x0F;
    }
    for(j=0; j<i; j++)
    {
        data += tmp[j]*pow(10,len-j+1);
    }
    if(Flag==0)
        return data;
    else if(Flag==1)
        return 0-data;
}

/********************************************************************\
* 函数名:   itoa
* 说明:         将一个整型数格式化成指定进制的字符串
* 功能:
* 输入:     num:整形数字
*               str:转化后的字符串
                radix:指定格式化的进制
* 返回值:   转化后的字符串
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-19
\*********************************************************************/
char *itoa(int num, char *str, int radix)
{
    const char table[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *ptr = str;
    Uchar   negative = 0;
    if(num == 0)               //num=0
    {
        *ptr++='0';
        *ptr='\0';                // don`t forget the end of the string is '\0'!!!!!!!!!
        return str;
    }
    if(num<0)                //if num is negative ,the add '-'and change num to positive
    {
        *ptr++='-';
        num*=-1;
        negative = 1;
    }
    while(num)
    {
        *ptr++ = table[num%radix];
        num/=radix;
    }
    *ptr = '\0';            //if num is negative ,the add '-'and change num to positive
    // in the below, we have to converse the string
    char *start =(negative ? str+1:str); //now start points the head of the string
    ptr--;                           //now prt points the end of the string
    while(start<ptr)
    {
        char temp = *start;
        *start = *ptr;
        *ptr = temp;
        start++;
        ptr--;
    }
    return str;
}

/********************************************************************\
* 函数名:   Dispose_Small_CPU_Data
* 说明:         将一个数的大小端颠倒
* 功能:
* 输入:     *data:整形数字地址
*               num:2字节的数或4字节的数
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-19
\*********************************************************************/
void Dispose_Small_CPU_Data(void *data,int num)
{
    Uchar i, *tp,*t,temp;
    if((num!=2)&&(num!=4))
        return;
    tp=data;
    t=tp+num-1;
    i=num/2;
    while(i--)
    {
        temp=*tp;
        *tp=*t;
        *t=temp;
        tp++;
        t--;
    }
}

/********************************************************************\
* 函数名:   Deal_Status_Code
* 说明:         回应FS状态字处理函数
* 功能:
* 输入:     num:FS下发时的状态字
* 返回值:   加气机回复时的状态字
* 创建人:   Yang Chao Xu
* 创建时间: 2014-09-24
\*********************************************************************/
Uchar Deal_Status_Code(Uchar num)
{
    Uchar   i,j;
    i=0x1F&num;                     //清除高三位
    j=(0xE0&num)>>5;                //得到高三位
    if(j==0)                        //读消息
    {
        return (0x20|i);
    }
    else if(j==1)                   //应答消息
    {
        return num;//暂不处理
    }
    else if(j==2)                   //写消息
    {
        return (0xE0|i);
    }
    else if(j==3)                   //带确认的主动数据消息
    {
        return num;//暂不处理
    }
    else if(j==4)                   //不带确认的主动数据消息
    {
        return num;//暂不处理
    }
    else if(j==7)                   //确认消息
    {
        return num;//暂不处理
    }
    else
    {
        printf("Deal_Status_Code Error or Recv Data Error! Code=%X\n",num);
        return 0xDD;
    }
}


/********************************************************************\
* 函数名:   Check_Netlink
* 说明:         判断网络是否连接好
* 功能:
* 输入:     nic_name:网络设备名如eth0
* 返回值:   0=网络连接好，-1=网络未连好
* 创建人:   Yang Chao Xu
* 创建时间: 2014-10-02
\*********************************************************************/
int Check_Netlink(char *nic_name)
{
    struct  ifreq   ifr;
    int skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd >= 0)
    {
        strcpy(ifr.ifr_name, nic_name);
        if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
        {
            close(skfd);
            return -1;
        }
        if(ifr.ifr_flags & IFF_RUNNING)
        {
            close(skfd);
            return 0; //网络连接好
        }
        else
        {
            close(skfd);
            return -1;
        }
        close(skfd);
    }
    else
    {
        printf("socket create fail...GetLocalIp!, %s", strerror(errno));
        return -1;
    }
}


