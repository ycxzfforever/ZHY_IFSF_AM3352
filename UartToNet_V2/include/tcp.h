#ifndef _TCP_H
#define _TCP_H


int Cli_FS_fd;
int Ser_FS_fd;
int Cli_EPS_fd;
int FS_Data_Len;
int EPS_Data_Len;
Uchar EPS_Error;

int Tcp_Pthread_Creat();            //TCP�̴߳���

int EPS_Tcp_Client_Creat();     //��EPSͨѶ�ͻ��˴���
int EPS_Tcp_Server_Creat();     //��EPSͨѶ����������
int FS_Tcp_Client_Creat();      //��FSͨѶ�ͻ��˴���
int FS_Tcp_Server_Creat();      //��FSͨѶ����������

void *Cli_FS_To_Uart_Thread(void *arg);     //��FS�ͻ��� ����
void *Cli_Uart_To_FS_Thread(void *arg);     //��FS�ͻ��� ����
void *Ser_FS_To_Uart_Thread(void *arg); //��FS����� ����
void *Ser_Uart_To_FS_Thread(void *arg); //��FS����� ����

void *Cli_EPS_To_Uart_Thread(void *arg);    //��EPS�ͻ��� ����
void *Cli_Uart_To_EPS_Thread(void *arg);    //��EPS�ͻ��� ����
void *Ser_EPS_To_Uart_Thread(void *arg);    //��EPS����� ����
void *Ser_Uart_To_EPS_Thread(void *arg);    //��EPS����� ����


#endif

