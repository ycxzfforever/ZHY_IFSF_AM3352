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
#define YANKATIMEOUT    30              //等待eps验卡返回超时时间

#define Uchar   unsigned char
#define Uint    unsigned int
#define Ushort  unsigned short
#define Ulong   unsigned long


char Uart_To_EPS[LENGTH];               //串口发给EPS的数据
char Uart_To_FS[LENGTH];                //串口发给FS的数据
char EPS_To_Uart[LENGTH];               //EPS发给串口的数据
char FS_To_Uart[LENGTH];                //FS发给串口的数据
char Uart_Rx_Buff[LENGTH];              //串口接收数据
char Uart_Tx_Buff[LENGTH];              //串口发送数据

char    EPS_Bank_Buf[LENGTH];           //EPS备份数据，防止向EPS重新申请
Uchar   FaQiYanKaFlag;                  //等于1时,加气机发起验卡
Uchar   FaQiYanKaTime;                  //加气机两次发起验卡的间隔时间
Uchar   YanKaFlag;                      //等于1时,eps已经返回验卡申请
Uchar   HuiSuoFlag;                     //等于1时,eps已经返回灰锁结果
Uchar   RequestPayFlag;                 //等于1时,eps已经返回申请支付结果
Uchar   OverTransFlag;                  //等于1时,eps已经返回通知交易结果
Uchar   YanKaCount;
Uchar   HuiSuoCount;
Uchar   RequestPayCount;
Uchar   OverTransCount;




Uchar   BoartRequestFlag;               //等于1时，主板开始申请支付

struct Conf
{
    Uchar Gun_num;              //枪号
    char EPS_IP[16];            //EPS的ip地址
    Uint EPS_Port;              //EPS端口号
    char FS_IP[16];             //fuel server ip地址
    Uint FS_Port;               //FS-TCP端口号
    Uint FS_Udp_Port;           //FS-UDP端口号
    Uint Time_Out;              //心跳超时时间
    Uint Time_Reconnect;        //重连时间间隔
    char My_IP[16];             //本地ip地址
    Uint My_EPS_Port;           //本地server 与EPS端口号
    Uint My_FS_Port;            //本地server 与FS端口号
    char My_Mask[16];           //本地掩码
    char My_Gateway[16];        //本地网关
    char My_Broadcast_IP[16];   //本地广播地址
    Uint My_Broadcast_Port;     //本地广播地址端口
    char IFSF_Node[8];          //IFSF节点地址
    Uchar FS_NODE[2];           //IFSF节点地址十进制
    char My_Node[8];            //本地节点地址
    Uchar MY_NODE[2];           //本地节点
    char Pay_Terminal_NO[16];   //支付终端编号
    Uchar Pri_Ver;              //单价版本号
    char Uart_NO[12];           //选择串口号
    Uint Uart_Speed;            //配置串口波特率
    Uchar Uart_Data_Bits;       //配置数据位
    Uchar Uart_Stop_Bits;       //配置停止位
    Uchar Uart_Parity;          //配置校验方式N:无校验、O：奇校验、E：偶校验
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

typedef struct SubNet_Node              //节点地址
{
    Uchar SubNet;   //device type 1-255
    Uchar Node;     //1-127
} SubNet_Node;

//加气机数据50 51 52命令的
typedef struct PARA
{
    Uchar   FSC_TIME[7];            //FSC时间
    Uchar   PRC_VER;                //气价版本
    Uchar   STA_FC_SER;             //0=FS离线，1=FS在线
    Uchar   NZN;                    //枪号
    Uchar   ST;                     //状态字1=不可用2=关闭3=空闲4=请求5=授权6=开始加气7=加气站
    Ushort  ER_ID;                  //故障代码
    Uchar   NO_RE_TR;               //未上传成交记录
    Ushort  OFF_TRAN_N;             //未上传成交记录数量
    Uchar   MODE;                   //0=联机模式1=脱机模式
    Uchar   CLA_NU;                 //班次号
    Uchar   EMP_NU;                 //员工号
    Uint    NO_Payment_Trans;       //未支付交易数
    Uchar   Clear_Trans;            //被清除交易数
    Uchar   CA_CH_ST;               //0=插卡未pin验证，1=插卡已pin验证
    Uchar   ASN[10];                //卡应用号
    Ushort  CARD_ST;                //卡状态
    Uint    BAL;                    //余额
    Uchar   CARD_TY;                //卡片类型
    Uchar   PL_NU[10];              //车牌号
    Uchar   MAH_S;                  //阀门状态
    Ushort  CUR_PR;                 //当前压力
    Uint    VOL;                    //当前气量
    Uint    AMN;                    //当前金额
    Ushort  PRC;                    //当前单价
    Uchar   RESULT;                 //0=授权，1=拒绝授权
    Uchar   READ_TY;                //0=读取最早记录，1=读取最后记录2=根据POS_TTC读记录
    Uint    POS_TTC;                //终端交易号hex 流水号
    Uchar   T_TYPE;                 //交易类型 0-脱机，1-在线
    Uchar   TIME[7];                //启动日期及时间bcd
    Uchar   END_TIME[7];            //加气结束日期及时间bcd
    Uchar   CTC[2];                 //卡交易序号
    Uchar   TAC[4];                 //电子签名
    Uchar   GMAC[4];                //解灰认证码
    Uchar   PSAM_TAC[4];            //灰锁签名
    Uchar   PSAM_ASN[10];           //PSAM应用号
    Uchar   PSAM_TID[6];            //PSAM编号
    Uchar   PSAM_TTC[4];            //PSAM终端交易号
    Uchar   G_CODE[2];              //油品代码
    Uchar   UNIT;                   //计量单位
    Uint    V_TOT;                  //数量总累
    Ushort  R_PRC;                  //应收单价
    Uchar   CR_EM_NU;               //当班员工号
    Uchar   AUTH_WAY;               //授权方式
    Ushort  STOP_REA;               //停机原因
    Ushort  STAR_PRE;               //启动压力
    Ushort  STOP_PRE;               //停机压力
    Ushort  DENSITY;                //密度
    Uchar   EQU[3];                 //当量
    Uchar   TMP[5];                 //预留
    Uchar   VER;                    //版本
    Uchar   T_MAC[4];               //终端数据认证码
    Uint    V_AMN;                  //总金额
} PARA;

typedef struct PARA53
{
    Uint    BL_LEN;                 //下载的数据长度
    Uchar   CONTENT;                //数据内容 (只有4-气价表有效)

} PARA53;

typedef struct PARA54
{
    Uchar   CONTENT;                //数据内容 (只有4-气价表有效)
    Ushort  S_OFFSET;               //段偏移
    Uchar   SEG;                    //段数量

} PARA54;

typedef struct PARA55
{


} PARA55;

typedef struct PARA56
{


} PARA56;

typedef struct PARA57
{
    Uchar   NZN;            //枪号
    Uchar   UNIT;           //单位0=千克，1=立方
    Uint    V_TOT;          //气       累计量
    Uint    A_TOT;          //金额 累积量
    Uint    No_TR_Total;    //逻辑油枪提供的交易数累计
} PARA57;

typedef struct PARA58
{


} PARA58;

typedef struct PRICE
{
    Uchar   VER;            //油价版本号
    Uchar   V_D_T[6];       //油价生效日期bcd
    Uchar   FIE_NU;         //油价记录数一般为1
    //气价记录格式
    Uchar   NZN;            //枪号
    Uchar   O_TYPE[4];      //气品代码
    Uchar   PRC_N;          //价格数目一般8个油价
    Ushort  Price[8];       //8个油价
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

//1-通讯服务数据库00H
typedef struct COM_SV
{
    //配置数据
    Uchar       Communication_Protocol_Ver[6];  //01H   bcd12
    SubNet_Node Local_Node_Address;             //02H   bin8, bin8
    SubNet_Node Recipient_Addr_Table[64];       //03H   64*(bin8, bin8)
    Uchar       Heartbeat_Interval;             //04H   bin8
    Uchar       Max_Block_Length;               //05H   bin8
    //命令
    SubNet_Node Add_Recipient_Addr;             //0BH   bin8, bin8
    SubNet_Node Remove_Recipient_Addr;          //0CH   bin8, bin8
} COM_SV;

//2-计量器数据库01H
typedef struct C_DAT
{
    //配置数据
    Uchar   Nb_Products;                //02H   bin8 油品种类
    Uchar   Nb_Fuelling_Modes;          //03H   bin8 加气模式的种类
    Uchar   Nb_Meters;                  //04H   bin8 流量计的种类
    Uchar   Nb_FP;                      //05H   bin8 加气机计量器控制的加气点的数量
    Uchar   Country_Code[2];            //06H   bcd4 国家代码 默认值为 0086
    Uchar   Auth_State_Mode;            //0BH   bin8 FP 是否允许预授权状态，默认值为 1。0 = 允许 AUTHORISED 状态，1 = 不允许 AUTHORISED 状态
    Uchar   Stand_Alone_Auth;           //0CH   bin8 加气机是否可在自主加气模式中工作，0 = 交易以“提枪”开始，1 = 用独立按键手动释放FP
    Uchar   Max_Auth_Time;              //0DH   bin8 描述 FP 将在授权状态停留的最长等待时间（以 10 秒为单位），默认值为 0,0 = 不限制授权时间
    Uchar   Max_Time_W;                 //15H   bin8 描述加气脉冲允许的最长时间间隔（单位为秒）。如果时间超出，计量器将停止 FP 泵的运转，默认值为 0。0 = 不检查
    Uchar   Min_Fuelling_Vol;           //16H   bin8
    Uchar   Min_Display_Vol;            //17H   bin8
    Uchar   Min_Guard_Time;             //18H   bin8
    Uchar   Pulser_Err_Tolerance;       //1AH   bin8
    Uchar   Digits_Vol_Layout;          //28H   Bcd2
    Uchar   Digits_Amount_Layout;       //29H   Bcd2
    Uchar   Digits_Unit_Price;          //2AH   Bcd2
} C_DAT;

//3-加气点标识数据库21H-24H
typedef struct FP_ID
{
    //配置数据
    Uchar   FP_Name[8];                 //01H   Asc8 允许为加气点设定名称和号码
    Uchar   Nb_Tran_Buffer_Not_Paid;    //02H   bin8 描述未支付交易数。（没有被 CD 清除），它可能被每个FP 存储。数字的接受范围是 1-15。默认值：2
    Uchar   Nb_Of_Historic_Trans;       //03H   bin8 描述可以存储在FP 中的被清除交易的数量。总是先删除旧的交易（先进先出）。默认值：不能少于 15 笔
    Uchar   Nb_Logical_Nozzle;          //04H   bin8 FP 上逻辑油枪的数量。数量被接受的范围是 1 到 8。0 = 未设定
    Uchar   Default_Fuelling_Mode;      //07H   bin8   1~8   0:未设定 默认值1
    //控制数据
    Uchar   FP_State;                   //14H   bin8   1~9
    Uchar   Log_Noz_State;              //15H   bin8   1 = 油枪未提起 0 = 油枪提起
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
    //当前交易数据
    Uchar   Current_TR_Seq_Nb[2];       //1DH   bcd4      1~9999
    Uchar   Release_Contr_Id[2];        //1EH   bin16
    Uchar   Suspend_Contr_Id[2];        //1FH   bin16
    Uchar   Current_Amount[5];          //22H   bin8+bcd8  AMOUNT
    Uchar   Current_Volume[5];          //23H   bin8+bcd8  VOLUME
    Uchar   Current_Unit_Price[4];      //24H   bin8+bcd6  UINT_PRICE
    Uchar   Current_Log_Noz;            //25H   bin8
    Uchar   Current_Prod_Nb[4];         //26H   bcd8
    Uchar   Current_TR_Error_Code;      //27H   bin8
    //配置数据,FP 控制命令
    Uchar   Open_FP;                    //3CH   CMD
    Uchar   Close_FP;                   //3DH   CMD
    Uchar   Release_FP;                 //3EH   CMD
    Uchar   Terminate_FP;               //3FH   CMD
    Uchar   Suspend_FP;                 //40H   CMD
    Uchar   Resume_FP;                  //41H   CMD
    //主动数据
    Uchar   FP_Status_Message;                      //64H 总是0     eg: 64 00 14 01 fps 15 01 ns 16 02 acd
    Uchar   FP_Running_Transaction_Message;         //66H 总是0     eg: 66 00 22 05 amount 23 05 volume
} FP_ID;

//4-逻辑油枪标识数据库21H-24H + 11H-18H
typedef struct FP_ID_LN_ID
{
    //配置数据
    Uchar   PR_Id;                      //01H   bin8  0~8  0=未分配油品, 1->41H,2->42H ... 8->48H
    Uchar   Physical_Noz_Id;            //05H   bin8  1~8
    Uchar   Meter_1_Id;                 //07H   bin8  1~16
    //总累计数
    Uchar   Log_Noz_Vol_Total[7];       //14H   bin8+bcd12   Long_VOLUME
    Uchar   Log_Noz_Amount_Total[7];    //15H   bin8+bcd12   Long_AMOUNt
    Uchar   No_TR_Total[7];             //16H   bin8+bcd12   Long_NUMBER
} FP_ID_LN_ID;

//5-加气交易数据库21H-24H + 21H + 0001~9999
typedef struct FP_ID_TR_DAT_TR_Seq_Nb
{
    //交易数据
    Uchar   TR_Seq_Nb[2];               //01H   bcd4       1~9999
    Uchar   TR_Contr_Id[2];             //02H   bin16
    Uchar   TR_Release_Token;           //03H   bin8
    Uchar   TR_Fuelling_Mode;           //04H   bin8
    Uchar   TR_Amount[5];               //05H   bin8+bcd8  AMOUNT
    Uchar   TR_Volume[5];               //06H   bin8+bcd8  VOLUME
    Uchar   TR_Unit_Price[4];           //07H   bin8+bcd6  UINT_PRICE
    Uchar   TR_Log_Noz;                 //08H   bin8   指定加气使用的逻辑油枪
    Uchar   TR_Prod_Nb[4];              //0AH   bcd8  指定加入油品品号的号码
    Uchar   TR_Prod_Description[16];    //0BH   Asc16  加入油品的油品描述
    Uchar   TR_Error_Code;              //0CH   bin8
    Uchar   TR_Security_Chksum[3];      //0EH   bin24
    //交易缓冲区状态
    Uchar   Trans_State;                //15H   bin8
    Uchar   TR_Buff_Contr_Id[2];        //14H   bin16
    //交易命令
    Uchar   Clear_Transaction;          //1EH   CMD
    Uchar   Lock_Transaction;           //1FH   CMD
    Uchar   Unlock_Transaction;         //20H   CMD
    //主动数据
    Uchar   TR_Buff_Status_Message;     //64H 总是0     eg: 64,00,1,2,trn,15,01,trs,14,2,trcd,05,5,tram,06,5,vol
    //设备生产厂家/石油公司特性
    Uchar   Pressure_Before_Delivery[4];//EAH   bin8+bcd6  加气前枪压
    Uchar   Pressure_After_Delivery[4]; //EBH   bin8+bcd6  加气后枪压
} FP_ID_TR_DAT_TR_Seq_Nb;

//6-错误码数据库21H-24H + 41H + 01H~40H
typedef struct FP_ID_ER_DAT_ER_ID
{
    //错误数据
    Uchar   FP_Error_Type;              //01H   bin8  1~64
    Uchar   FP_Err_Description[20];     //02H   Asc20
    Uchar   FP_Error_Total;             //03H   bin8  0~255
    Uchar   FP_Error_State;             //05H   bin8  1~9
    //主动数据
    Uchar   FP_Error_Type_Mes;          //64H 总是0     eg: 64 00 01 01 fet 05 01 fes
} FP_ID_ER_DAT_ER_ID;

//7-油品数据库41H-48H
typedef struct PR_ID
{
    //配置数据
    Uchar   Prod_Nb[4];                 //02H   bcd8
    Uchar   Prod_description[16];       //03H   Asc16
} PR_ID;

//8-每种加气模式下的油品数据库 61H+00000001~99999999+11H-18H
typedef struct PR_DAT_Prod_Nb_FM_ID
{
    //配置数据
    Uchar   Fuelling_Mode_Name[8];      //01H   Asc8
    Uchar   Prod_Price[4];              //02H   bin8+bcd6  UINT_PRICE
    Uchar   Max_Vol[5];                 //03H   bin8+bcd8  VOLUME
    Uchar   Max_Fill_Time;              //04H   bin8     0~255
    Uchar   Max_Auth_Time;              //05H   bin8     0~255
    Uchar   User_Max_Volume[5];         //06H   bin8+bcd8  VOLUME
} PR_DAT_Prod_Nb_FM_ID;

//9-流量计数据库81H-90H
typedef struct M_ID
{
    //配置数据
    Uchar   Meter_Type;                 //01H   bin8   0~2
    Uchar   PR_Id;                      //04H   bin8   1~8
} M_ID;

//10-数据和软件下载数据库A1H    (  没用)
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

int Time_ms; //1ms计数
int Time_S; //秒计数
int Time_100ms;
int F0_Time;
Uchar Disconnect_EPS_Flag; //等于1时断开与EPS的连接
Uchar Reconnect_FS_Flag; //等于1时重新连接FS服务器
Uchar Reconnect_EPS_Flag; //等于1时重新连接EPS
Uchar Cli_Connect_FS; //等于1时客户端连接上了FS服务器
Uchar Ser_Connect_FS; //等于1时FS连接上了加气机服务器
Uchar Is_Connect_EPS; //等于1时连接上了EPS
Uchar FS_OK;      //等于1时，FS通讯良好
Uchar Stop_Opt;      //等于1时，认为fs不正常
Uchar OPT_Server_OK; //等于1时，加气机server服务创建成功
Uchar Uart_To_EPS_Flag; //等于1时uart数据开始向eps发送
Uchar Uart_To_FS_Flag; //等于1时uart数据开始向fs发送
Uchar DownLoadPrice; //等于1时在空闲时在下发一次单价
Uchar EPS_To_Uart_Flag; //等于1时eps数据开始向uart发送
Uchar FS_To_Uart_Flag; //等于1时fs数据开始向uart发送
Uchar CmdFlag;//发送给加气的命令字
Uchar ReadNoPayFlag;//等于1时读取未支付交易
Uchar OPT_IS_Online;//等于0时断开FS和转接板的链接
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

