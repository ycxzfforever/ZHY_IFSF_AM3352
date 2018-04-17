#ifndef _common_H
#define _common_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>
#include <sys/ioctl.h>
#include <math.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <linux/socket.h>
#include <sys/stat.h>
#include <termios.h>
#include "globle.h"

#include "uart.h"
#include "udp.h"
#include "timer.h"
#include "tcp.h"
#include "log.h"
#include "file.h"
#include "crc.h"
#include "cmd.h"
#include "bio.h"





#define BIT0        0X00000001
#define BIT1        0X00000002
#define BIT2        0X00000004
#define BIT3        0X00000008
#define BIT4        0X00000010
#define BIT5        0X00000020
#define BIT6        0X00000040
#define BIT7        0X00000080
#define BIT8        0X00000100
#define BIT9        0X00000200
#define BIT10       0X00000400
#define BIT11       0X00000800
#define BIT12       0X00001000
#define BIT13       0X00002000
#define BIT14       0X00004000
#define BIT15       0X00008000
#define BIT16       0X00010000
#define BIT17       0X00020000
#define BIT18       0X00040000
#define BIT19       0X00080000
#define BIT20       0X00100000
#define BIT21       0X00200000
#define BIT22       0X00400000
#define BIT23       0X00800000
#define BIT24       0X01000000
#define BIT25       0X02000000
#define BIT26       0X04000000
#define BIT27       0X08000000
#define BIT28       0X10000000
#define BIT29       0X20000000
#define BIT30       0X40000000
#define BIT31       0X80000000



#define BACKLOG 10
#define LENGTH 512                     // Buffer length     
#define YANKATIMEOUT    30              //�ȴ�eps�鿨���س�ʱʱ��

#define Uchar   unsigned char
#define Uint    unsigned int
#define Ushort  unsigned short
#define Ulong   unsigned long


char Uart_To_EPS[LENGTH];               //���ڷ���EPS������
char Uart_To_FS[LENGTH];                //���ڷ���FS������
char EPS_To_Uart[LENGTH];               //EPS�������ڵ�����
char FS_To_Uart[LENGTH];                //FS�������ڵ�����
char Uart_Rx_Buff[LENGTH];              //���ڽ�������
char Uart_Tx_Buff[LENGTH];              //���ڷ�������

char    EPS_Bank_Buf[LENGTH];           //EPS�������ݣ���ֹ��EPS��������
Uchar   FaQiYanKaFlag;                  //����1ʱ,�����������鿨
Uchar   FaQiYanKaTime;                  //���������η����鿨�ļ��ʱ��
Uchar   YanKaFlag;                      //����1ʱ,eps�Ѿ������鿨����
Uchar   HuiSuoFlag;                     //����1ʱ,eps�Ѿ����ػ������
Uchar   RequestPayFlag;                 //����1ʱ,eps�Ѿ���������֧�����
Uchar   OverTransFlag;                  //����1ʱ,eps�Ѿ�����֪ͨ���׽��
Uchar   YanKaCount;
Uchar   HuiSuoCount;
Uchar   RequestPayCount;
Uchar   OverTransCount;




Uchar   BoartRequestFlag;               //����1ʱ�����忪ʼ����֧��

struct Conf
{
    Uchar Gun_num;              //ǹ��
    char EPS_IP[16];            //EPS��ip��ַ
    Uint EPS_Port;              //EPS�˿ں�
    char FS_IP[16];             //fuel server ip��ַ
    Uint FS_Port;               //FS-TCP�˿ں�
    Uint FS_Udp_Port;           //FS-UDP�˿ں�
    Uint Time_Out;              //������ʱʱ��
    Uint Time_Reconnect;        //����ʱ����
    char My_IP[16];             //����ip��ַ
    Uint My_EPS_Port;           //����server ��EPS�˿ں�
    Uint My_FS_Port;            //����server ��FS�˿ں�
    char My_Mask[16];           //��������
    char My_Gateway[16];        //��������
    char My_Broadcast_IP[16];   //���ع㲥��ַ
    Uint My_Broadcast_Port;     //���ع㲥��ַ�˿�
    char IFSF_Node[8];          //IFSF�ڵ��ַ
    Uchar FS_NODE[2];           //IFSF�ڵ��ַʮ����
    char My_Node[8];            //���ؽڵ��ַ
    Uchar MY_NODE[2];           //���ؽڵ�
    char Pay_Terminal_NO[16];   //֧���ն˱��
    Uchar Pri_Ver;              //���۰汾��
    char Uart_NO[12];           //ѡ�񴮿ں�
    Uint Uart_Speed;            //���ô��ڲ�����
    Uchar Uart_Data_Bits;       //��������λ
    Uchar Uart_Stop_Bits;       //����ֹͣλ
    Uchar Uart_Parity;          //����У�鷽ʽN:��У�顢O����У�顢E��żУ��
};
typedef struct Cli_Arg
{
    int fd;
    char revbuf[LENGTH];                     // Receive buffer
} Cli_Arg;
typedef struct Ser_Arg
{
    int fd;
    char sdbuf[LENGTH];                      // Send buffer
} Ser_Arg;

typedef struct SubNet_Node              //�ڵ��ַ
{
    Uchar SubNet;   //device type 1-255
    Uchar Node;     //1-127
} SubNet_Node;

//����������50 51 52�����
typedef struct PARA
{
    Uchar   FSC_TIME[7];            //FSCʱ��
    Uchar   PRC_VER;                //���۰汾
    Uchar   STA_FC_SER;             //0=FS���ߣ�1=FS����
    Uchar   NZN;                    //ǹ��
    Uchar   ST;                     //״̬��1=������2=�ر�3=����4=����5=��Ȩ6=��ʼ����7=����վ
    Ushort  ER_ID;                  //���ϴ���
    Uchar   NO_RE_TR;               //δ�ϴ��ɽ���¼
    Ushort  OFF_TRAN_N;             //δ�ϴ��ɽ���¼����
    Uchar   MODE;                   //0=����ģʽ1=�ѻ�ģʽ
    Uchar   CLA_NU;                 //��κ�
    Uchar   EMP_NU;                 //Ա����
    Uint    NO_Payment_Trans;       //δ֧��������
    Uchar   Clear_Trans;            //�����������
    Uchar   CA_CH_ST;               //0=�忨δpin��֤��1=�忨��pin��֤
    Uchar   ASN[10];                //��Ӧ�ú�
    Ushort  CARD_ST;                //��״̬
    Uint    BAL;                    //���
    Uchar   CARD_TY;                //��Ƭ����
    Uchar   PL_NU[10];              //���ƺ�
    Uchar   MAH_S;                  //����״̬
    Ushort  CUR_PR;                 //��ǰѹ��
    Uint    VOL;                    //��ǰ����
    Uint    AMN;                    //��ǰ���
    Ushort  PRC;                    //��ǰ����
    Uchar   RESULT;                 //0=��Ȩ��1=�ܾ���Ȩ
    Uchar   READ_TY;                //0=��ȡ�����¼��1=��ȡ����¼2=����POS_TTC����¼
    Uint    POS_TTC;                //�ն˽��׺�hex ��ˮ��
    Uchar   T_TYPE;                 //�������� 0-�ѻ���1-����
    Uchar   TIME[7];                //�������ڼ�ʱ��bcd
    Uchar   END_TIME[7];            //�����������ڼ�ʱ��bcd
    Uchar   CTC[2];                 //���������
    Uchar   TAC[4];                 //����ǩ��
    Uchar   GMAC[4];                //�����֤��
    Uchar   PSAM_TAC[4];            //����ǩ��
    Uchar   PSAM_ASN[10];           //PSAMӦ�ú�
    Uchar   PSAM_TID[6];            //PSAM���
    Uchar   PSAM_TTC[4];            //PSAM�ն˽��׺�
    Uchar   G_CODE[2];              //��Ʒ����
    Uchar   UNIT;                   //������λ
    Uint    V_TOT;                  //��������
    Ushort  R_PRC;                  //Ӧ�յ���
    Uchar   CR_EM_NU;               //����Ա����
    Uchar   AUTH_WAY;               //��Ȩ��ʽ
    Ushort  STOP_REA;               //ͣ��ԭ��
    Ushort  STAR_PRE;               //����ѹ��
    Ushort  STOP_PRE;               //ͣ��ѹ��
    Ushort  DENSITY;                //�ܶ�
    Uchar   EQU[3];                 //����
    Uchar   TMP[5];                 //Ԥ��
    Uchar   VER;                    //�汾
    Uchar   T_MAC[4];               //�ն�������֤��
    Uint    V_AMN;                  //�ܽ��
} PARA;

typedef struct PARA53
{
    Uint    BL_LEN;                 //���ص����ݳ���
    Uchar   CONTENT;                //�������� (ֻ��4-���۱���Ч)

} PARA53;

typedef struct PARA54
{
    Uchar   CONTENT;                //�������� (ֻ��4-���۱���Ч)
    Ushort  S_OFFSET;               //��ƫ��
    Uchar   SEG;                    //������

} PARA54;

typedef struct PARA55
{


} PARA55;

typedef struct PARA56
{


} PARA56;

typedef struct PARA57
{
    Uchar   NZN;            //ǹ��
    Uchar   UNIT;           //��λ0=ǧ�ˣ�1=����
    Uint    V_TOT;          //��       �ۼ���
    Uint    A_TOT;          //��� �ۻ���
    Uint    No_TR_Total;    //�߼���ǹ�ṩ�Ľ������ۼ�
} PARA57;

typedef struct PARA58
{


} PARA58;

typedef struct PRICE
{
    Uchar   VER;            //�ͼ۰汾��
    Uchar   V_D_T[6];       //�ͼ���Ч����bcd
    Uchar   FIE_NU;         //�ͼۼ�¼��һ��Ϊ1
    //���ۼ�¼��ʽ
    Uchar   NZN;            //ǹ��
    Uchar   O_TYPE[4];      //��Ʒ����
    Uchar   PRC_N;          //�۸���Ŀһ��8���ͼ�
    Ushort  Price[8];       //8���ͼ�
} PRICE;


typedef struct TIME
{
    Uchar year[2];
    Uchar mon;
    Uchar day;
    Uchar hour;
    Uchar min;
    Uchar sec;
} Time;

//1-ͨѶ�������ݿ�00H
typedef struct COM_SV
{
    //��������
    Uchar       Communication_Protocol_Ver[6];  //01H   bcd12
    SubNet_Node Local_Node_Address;             //02H   bin8, bin8
    SubNet_Node Recipient_Addr_Table[64];       //03H   64*(bin8, bin8)
    Uchar       Heartbeat_Interval;             //04H   bin8
    Uchar       Max_Block_Length;               //05H   bin8
    //����
    SubNet_Node Add_Recipient_Addr;             //0BH   bin8, bin8
    SubNet_Node Remove_Recipient_Addr;          //0CH   bin8, bin8
} COM_SV;

//2-���������ݿ�01H
typedef struct C_DAT
{
    //��������
    Uchar   Nb_Products;                //02H   bin8 ��Ʒ����
    Uchar   Nb_Fuelling_Modes;          //03H   bin8 ����ģʽ������
    Uchar   Nb_Meters;                  //04H   bin8 �����Ƶ�����
    Uchar   Nb_FP;                      //05H   bin8 ���������������Ƶļ����������
    Uchar   Country_Code[2];            //06H   bcd4 ���Ҵ��� Ĭ��ֵΪ 0086
    Uchar   Auth_State_Mode;            //0BH   bin8 FP �Ƿ�����Ԥ��Ȩ״̬��Ĭ��ֵΪ 1��0 = ���� AUTHORISED ״̬��1 = ������ AUTHORISED ״̬
    Uchar   Stand_Alone_Auth;           //0CH   bin8 �������Ƿ������������ģʽ�й�����0 = �����ԡ���ǹ����ʼ��1 = �ö��������ֶ��ͷ�FP
    Uchar   Max_Auth_Time;              //0DH   bin8 ���� FP ������Ȩ״̬ͣ������ȴ�ʱ�䣨�� 10 ��Ϊ��λ����Ĭ��ֵΪ 0,0 = ��������Ȩʱ��
    Uchar   Max_Time_W;                 //15H   bin8 ������������������ʱ��������λΪ�룩�����ʱ�䳬������������ֹͣ FP �õ���ת��Ĭ��ֵΪ 0��0 = �����
    Uchar   Min_Fuelling_Vol;           //16H   bin8
    Uchar   Min_Display_Vol;            //17H   bin8
    Uchar   Min_Guard_Time;             //18H   bin8
    Uchar   Pulser_Err_Tolerance;       //1AH   bin8
    Uchar   Digits_Vol_Layout;          //28H   Bcd2
    Uchar   Digits_Amount_Layout;       //29H   Bcd2
    Uchar   Digits_Unit_Price;          //2AH   Bcd2
} C_DAT;

//3-�������ʶ���ݿ�21H-24H
typedef struct FP_ID
{
    //��������
    Uchar   FP_Name[8];                 //01H   Asc8 ����Ϊ�������趨���ƺͺ���
    Uchar   Nb_Tran_Buffer_Not_Paid;    //02H   bin8 ����δ֧������������û�б� CD ������������ܱ�ÿ��FP �洢�����ֵĽ��ܷ�Χ�� 1-15��Ĭ��ֵ��2
    Uchar   Nb_Of_Historic_Trans;       //03H   bin8 �������Դ洢��FP �еı�������׵�������������ɾ���ɵĽ��ף��Ƚ��ȳ�����Ĭ��ֵ���������� 15 ��
    Uchar   Nb_Logical_Nozzle;          //04H   bin8 FP ���߼���ǹ�����������������ܵķ�Χ�� 1 �� 8��0 = δ�趨
    Uchar   Default_Fuelling_Mode;      //07H   bin8   1~8   0:δ�趨 Ĭ��ֵ1
    //��������
    Uchar   FP_State;                   //14H   bin8   1~9
    Uchar   Log_Noz_State;              //15H   bin8   1 = ��ǹδ���� 0 = ��ǹ����
    Uchar   Assign_Contr_Id[2];         //16H   bin16
    Uchar   Release_Mode;               //17H   bin8
    Uchar   ZeroTR_Mode;                //18H   bin8   0~1
    Uchar   Log_Noz_Mask;               //19H   bin8
    Uchar   Config_Lock[2];             //1AH   bin16
    Uchar   Remote_Amount_Prepay[5];    //1BH   bin8+bcd8  AMOUNT
    Uchar   Remote_Volume_Preset[5];    //1CH   bin8+bcd8  VOLUME
    Uchar   Release_Token;              //20H   bin8
    Uchar   Fuelling_Mode;              //21H   bin8
    Uchar   Transaction_Sequence_Nb[2]; //29H   bcd4      1~9999
    //��ǰ��������
    Uchar   Current_TR_Seq_Nb[2];       //1DH   bcd4      1~9999
    Uchar   Release_Contr_Id[2];        //1EH   bin16
    Uchar   Suspend_Contr_Id[2];        //1FH   bin16
    Uchar   Current_Amount[5];          //22H   bin8+bcd8  AMOUNT
    Uchar   Current_Volume[5];          //23H   bin8+bcd8  VOLUME
    Uchar   Current_Unit_Price[4];      //24H   bin8+bcd6  UINT_PRICE
    Uchar   Current_Log_Noz;            //25H   bin8
    Uchar   Current_Prod_Nb[4];         //26H   bcd8
    Uchar   Current_TR_Error_Code;      //27H   bin8
    //��������,FP ��������
    Uchar   Open_FP;                    //3CH   CMD
    Uchar   Close_FP;                   //3DH   CMD
    Uchar   Release_FP;                 //3EH   CMD
    Uchar   Terminate_FP;               //3FH   CMD
    Uchar   Suspend_FP;                 //40H   CMD
    Uchar   Resume_FP;                  //41H   CMD
    //��������
    Uchar   FP_Status_Message;                      //64H ����0     eg: 64 00 14 01 fps 15 01 ns 16 02 acd
    Uchar   FP_Running_Transaction_Message;         //66H ����0     eg: 66 00 22 05 amount 23 05 volume
} FP_ID;

//4-�߼���ǹ��ʶ���ݿ�21H-24H + 11H-18H
typedef struct FP_ID_LN_ID
{
    //��������
    Uchar   PR_Id;                      //01H   bin8  0~8  0=δ������Ʒ, 1->41H,2->42H ... 8->48H
    Uchar   Physical_Noz_Id;            //05H   bin8  1~8
    Uchar   Meter_1_Id;                 //07H   bin8  1~16
    //���ۼ���
    Uchar   Log_Noz_Vol_Total[7];       //14H   bin8+bcd12   Long_VOLUME
    Uchar   Log_Noz_Amount_Total[7];    //15H   bin8+bcd12   Long_AMOUNt
    Uchar   No_TR_Total[7];             //16H   bin8+bcd12   Long_NUMBER
} FP_ID_LN_ID;

//5-�����������ݿ�21H-24H + 21H + 0001~9999
typedef struct FP_ID_TR_DAT_TR_Seq_Nb
{
    //��������
    Uchar   TR_Seq_Nb[2];               //01H   bcd4       1~9999
    Uchar   TR_Contr_Id[2];             //02H   bin16
    Uchar   TR_Release_Token;           //03H   bin8
    Uchar   TR_Fuelling_Mode;           //04H   bin8
    Uchar   TR_Amount[5];               //05H   bin8+bcd8  AMOUNT
    Uchar   TR_Volume[5];               //06H   bin8+bcd8  VOLUME
    Uchar   TR_Unit_Price[4];           //07H   bin8+bcd6  UINT_PRICE
    Uchar   TR_Log_Noz;                 //08H   bin8   ָ������ʹ�õ��߼���ǹ
    Uchar   TR_Prod_Nb[4];              //0AH   bcd8  ָ��������ƷƷ�ŵĺ���
    Uchar   TR_Prod_Description[16];    //0BH   Asc16  ������Ʒ����Ʒ����
    Uchar   TR_Error_Code;              //0CH   bin8
    Uchar   TR_Security_Chksum[3];      //0EH   bin24
    //���׻�����״̬
    Uchar   Trans_State;                //15H   bin8
    Uchar   TR_Buff_Contr_Id[2];        //14H   bin16
    //��������
    Uchar   Clear_Transaction;          //1EH   CMD
    Uchar   Lock_Transaction;           //1FH   CMD
    Uchar   Unlock_Transaction;         //20H   CMD
    //��������
    Uchar   TR_Buff_Status_Message;     //64H ����0     eg: 64,00,1,2,trn,15,01,trs,14,2,trcd,05,5,tram,06,5,vol
    //�豸��������/ʯ�͹�˾����
    Uchar   Pressure_Before_Delivery[4];//EAH   bin8+bcd6  ����ǰǹѹ
    Uchar   Pressure_After_Delivery[4]; //EBH   bin8+bcd6  ������ǹѹ
} FP_ID_TR_DAT_TR_Seq_Nb;

//6-���������ݿ�21H-24H + 41H + 01H~40H
typedef struct FP_ID_ER_DAT_ER_ID
{
    //��������
    Uchar   FP_Error_Type;              //01H   bin8  1~64
    Uchar   FP_Err_Description[20];     //02H   Asc20
    Uchar   FP_Error_Total;             //03H   bin8  0~255
    Uchar   FP_Error_State;             //05H   bin8  1~9
    //��������
    Uchar   FP_Error_Type_Mes;          //64H ����0     eg: 64 00 01 01 fet 05 01 fes
} FP_ID_ER_DAT_ER_ID;

//7-��Ʒ���ݿ�41H-48H
typedef struct PR_ID
{
    //��������
    Uchar   Prod_Nb[4];                 //02H   bcd8
    Uchar   Prod_description[16];       //03H   Asc16
} PR_ID;

//8-ÿ�ּ���ģʽ�µ���Ʒ���ݿ� 61H+00000001~99999999+11H-18H
typedef struct PR_DAT_Prod_Nb_FM_ID
{
    //��������
    Uchar   Fuelling_Mode_Name[8];      //01H   Asc8
    Uchar   Prod_Price[4];              //02H   bin8+bcd6  UINT_PRICE
    Uchar   Max_Vol[5];                 //03H   bin8+bcd8  VOLUME
    Uchar   Max_Fill_Time;              //04H   bin8     0~255
    Uchar   Max_Auth_Time;              //05H   bin8     0~255
    Uchar   User_Max_Volume[5];         //06H   bin8+bcd8  VOLUME
} PR_DAT_Prod_Nb_FM_ID;

//9-���������ݿ�81H-90H
typedef struct M_ID
{
    //��������
    Uchar   Meter_Type;                 //01H   bin8   0~2
    Uchar   PR_Id;                      //04H   bin8   1~8
} M_ID;

//10-���ݺ�����������ݿ�A1H    (  û��)
typedef struct SW_DAT
{

} SW_DAT;


struct Conf conf;
struct tm *now;
struct timeb tb;



pthread_mutex_t Data_To_Uart;
pthread_mutex_t FS_Recv_Data;
pthread_mutex_t cs_log;
pthread_mutex_t Uart_Recv_Data;

Uchar Check_Time;

int Time_ms; //1ms����
int Time_S; //�����
int Time_100ms;
int F0_Time;
Uchar Disconnect_EPS_Flag; //����1ʱ�Ͽ���EPS������
Uchar Reconnect_FS_Flag; //����1ʱ��������FS������
Uchar Reconnect_EPS_Flag; //����1ʱ��������EPS
Uchar Cli_Connect_FS; //����1ʱ�ͻ�����������FS������
Uchar Ser_Connect_FS; //����1ʱFS�������˼�����������
Uchar Is_Connect_EPS; //����1ʱ��������EPS
Uchar FS_OK;      //����1ʱ��FSͨѶ����
Uchar Stop_Opt;      //����1ʱ����Ϊfs������
Uchar OPT_Server_OK; //����1ʱ��������server���񴴽��ɹ�
Uchar Uart_To_EPS_Flag; //����1ʱuart���ݿ�ʼ��eps����
Uchar Uart_To_FS_Flag; //����1ʱuart���ݿ�ʼ��fs����
Uchar DownLoadPrice; //����1ʱ�ڿ���ʱ���·�һ�ε���
Uchar EPS_To_Uart_Flag; //����1ʱeps���ݿ�ʼ��uart����
Uchar FS_To_Uart_Flag; //����1ʱfs���ݿ�ʼ��uart����
Uchar CmdFlag;//���͸�������������
Uchar ReadNoPayFlag;//����1ʱ��ȡδ֧������
Uchar OPT_IS_Online;//����0ʱ�Ͽ�FS��ת�Ӱ������
Uchar OPT_72_Flag;
Uchar ReadTotal_Flag;



PARA                    para;
PARA53                  para53;
PARA54                  para54;
PARA55                  para55;
PARA56                  para56;
PARA57                  para57;
PARA58                  para58;
PRICE                   price;

COM_SV                  com_sv;
C_DAT                   c_dat;
FP_ID                   fp_id;
FP_ID_LN_ID             fp_id_ln_id;
FP_ID_TR_DAT_TR_Seq_Nb  fp_id_tr_dat_tr_seq_nb;
FP_ID_ER_DAT_ER_ID      fp_id_er_dat_er_id;
PR_ID                   pr_id;
PR_DAT_Prod_Nb_FM_ID    pr_dat_prod_nb_fm_id;
M_ID                    m_id;


#endif

