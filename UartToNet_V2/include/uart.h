#ifndef _UART_H_
#define _UART_H_


Uchar   Recv51_Flag;
Uchar   Recv57_Flag;
Uchar   ReadTotolFlag;//读总累标记，1:读总累，2:上传流水
Uchar   Recv5A_Flag;
Uchar   Status_Code;
int     Uart_Data_Len;
Uchar   Uart_Tx_Flag;
Uchar   Auth_Flag;//授权结果，0=成功，1=失败
Uchar   Auth_Type;//授权类型，0=金额，1=气量
long    Auth_Value;//授权量
Uchar   HaveAuth;   //0-未授权，1-已授权
Uchar   RecvFSAuth; //0-未收到fs授权，1-收到fs授权

Uchar   PointNo;        //小数位数
Uchar   ParaNo[2];      //参数索引号
Uchar   ParaValue[6];   //参数值
Uchar   Gun_UID[6];     //枪流水号

Uchar   StartFuelling;  //0:started 6状态开始加气，1:fuelling 8状态
Uint    ttc;
Ulong   totalnum;       //上次总气量，防止传两次


//extern const char *Conf_Name; //配置文件路径和名字
//extern const char *Pri_ConfName;


void set_speed(int fd, int speed);
int set_Parity(int fd, int databits, int stopbits, int parity);
void Reply_FS_UploadStatus(Uchar st);
void Deal_Uart_data();
void Deal_FS_To_Uart_data();
void Deal_EPS_To_Uart_data();
int Uart_Pthread_Creat();
void Reply_FS_UploadRealtimeData();
void Reply_FS_UploadOverFuel();
void Reply_FS_ReadTotal();
void Reply_FS_Auth_OPT();
void Reply_FS_Stop_OPT();
void Reply_FS_CantAuth_OPT();


#endif

