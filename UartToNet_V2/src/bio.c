#include "common.h"


/********************************************************************\
* ��������: Open_Uart
* ˵��:
* ����:     �򿪴���
* ����:     ���ں�
* ����ֵ:   0:�ɹ�-1:ʧ��
* ������:   Yang Chao Xu
* ����ʱ��:  2014-8-22
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
* ��������: Str_Split
* ˵��:
* ����:     ����ַ���
* ����:     ��Ҫ��ֵ��ַ���
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-10
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
* ������: Print_HEX
* ˵��:
* ����:    ���ַ���str����16���ƴ�ӡ
* ����:     len:  str����
             str:     ��Ҫ��ӡ���ַ���
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-9-12
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
* ������:   HEXtoBCD
* ˵��:         n�ֽ�ʮ������ת��Ϊn�ֽ�BCD��
* ����:
* ����:     hex_data:��ת������
*               n:�����ֽ���(�������λ����һλ)
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-19
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
* ������:   BCDtoHEX
* ˵��:         ���ֽ�BCD��ת��Ϊһ�ֽ�HEX
* ����:
* ����:     BCD_DATA:BCD����(��λ��ǰ),
*               n:�����ֽ���(�������λ����һλ)
* ����ֵ:   HEX����
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-19
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
* ������:   HEXtoFsData
* ˵��:         ʮ������ת��ΪFS����
* ����:
* ����:     hex_data=�����һ������n=С��λ����
*               Flag:0=������1=����
* ����ֵ:   ת������������鳤��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-19
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

//���ӵ�lengthΪ���鳤�ȣ�����ǰ�油0
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
* ������:   FSDataToLong
* ˵��:         FS����ת��Ϊ������
* ����:     ע��:ת�����ֵΪFSԭ���ݵ�100��
* ����:     num:FS���鳤�ȣ�
*               buf:FS��������
* ����ֵ:   ת�����ֵ
                ��: FS����06 12 34 56 78
ת�����ֵ��Ϊ:        12345678
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-19
\*********************************************************************/
long FSDataToLong(int num,char *buf)
{
    long data=0.0;
    Uchar i,j,Flag,len,tmp[20];
    if(((buf[0]&0xF0)>>4)==0)//����
        Flag=0;
    else if(((buf[0]&0xF0)>>4)==8)//����
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
* ������:   itoa
* ˵��:         ��һ����������ʽ����ָ�����Ƶ��ַ���
* ����:
* ����:     num:��������
*               str:ת������ַ���
                radix:ָ����ʽ���Ľ���
* ����ֵ:   ת������ַ���
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-19
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
* ������:   Dispose_Small_CPU_Data
* ˵��:         ��һ�����Ĵ�С�˵ߵ�
* ����:
* ����:     *data:�������ֵ�ַ
*               num:2�ֽڵ�����4�ֽڵ���
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-19
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
* ������:   Deal_Status_Code
* ˵��:         ��ӦFS״̬�ִ�����
* ����:
* ����:     num:FS�·�ʱ��״̬��
* ����ֵ:   �������ظ�ʱ��״̬��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-24
\*********************************************************************/
Uchar Deal_Status_Code(Uchar num)
{
    Uchar   i,j;
    i=0x1F&num;                     //�������λ
    j=(0xE0&num)>>5;                //�õ�����λ
    if(j==0)                        //����Ϣ
    {
        return (0x20|i);
    }
    else if(j==1)                   //Ӧ����Ϣ
    {
        return num;//�ݲ�����
    }
    else if(j==2)                   //д��Ϣ
    {
        return (0xE0|i);
    }
    else if(j==3)                   //��ȷ�ϵ�����������Ϣ
    {
        return num;//�ݲ�����
    }
    else if(j==4)                   //����ȷ�ϵ�����������Ϣ
    {
        return num;//�ݲ�����
    }
    else if(j==7)                   //ȷ����Ϣ
    {
        return num;//�ݲ�����
    }
    else
    {
        printf("Deal_Status_Code Error or Recv Data Error! Code=%X\n",num);
        return 0xDD;
    }
}


/********************************************************************\
* ������:   Check_Netlink
* ˵��:         �ж������Ƿ����Ӻ�
* ����:
* ����:     nic_name:�����豸����eth0
* ����ֵ:   0=�������Ӻã�-1=����δ����
* ������:   Yang Chao Xu
* ����ʱ��: 2014-10-02
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
            return 0; //�������Ӻ�
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


