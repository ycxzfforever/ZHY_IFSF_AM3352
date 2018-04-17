#ifndef _UART_H_
#define _UART_H_


Uchar   Recv51_Flag;
Uchar   Recv57_Flag;
Uchar   ReadTotolFlag;//�����۱�ǣ�1:�����ۣ�2:�ϴ���ˮ
Uchar   Recv5A_Flag;
Uchar   Status_Code;
int     Uart_Data_Len;
Uchar   Uart_Tx_Flag;
Uchar   Auth_Flag;//��Ȩ�����0=�ɹ���1=ʧ��
Uchar   Auth_Type;//��Ȩ���ͣ�0=��1=����
long    Auth_Value;//��Ȩ��
Uchar   HaveAuth;   //0-δ��Ȩ��1-����Ȩ
Uchar   RecvFSAuth; //0-δ�յ�fs��Ȩ��1-�յ�fs��Ȩ

Uchar   PointNo;        //С��λ��
Uchar   ParaNo[2];      //����������
Uchar   ParaValue[6];   //����ֵ
Uchar   Gun_UID[6];     //ǹ��ˮ��

Uchar   StartFuelling;  //0:started 6״̬��ʼ������1:fuelling 8״̬
Uint    ttc;
Ulong   totalnum;       //�ϴ�����������ֹ������


//extern const char *Conf_Name; //�����ļ�·��������
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

