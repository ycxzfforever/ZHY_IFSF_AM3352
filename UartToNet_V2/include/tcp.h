#ifndef _TCP_H
#define _TCP_H


int Cli_FS_fd;
int Ser_FS_fd;
int Cli_EPS_fd;
int FS_Data_Len;
int EPS_Data_Len;
Uchar EPS_Error;

int Tcp_Pthread_Creat();            //TCP线程创建

int EPS_Tcp_Client_Creat();     //与EPS通讯客户端创建
int EPS_Tcp_Server_Creat();     //与EPS通讯服务器创建
int FS_Tcp_Client_Creat();      //与FS通讯客户端创建
int FS_Tcp_Server_Creat();      //与FS通讯服务器创建

void *Cli_FS_To_Uart_Thread(void *arg);     //与FS客户端 发送
void *Cli_Uart_To_FS_Thread(void *arg);     //与FS客户端 接收
void *Ser_FS_To_Uart_Thread(void *arg); //与FS服务端 接收
void *Ser_Uart_To_FS_Thread(void *arg); //与FS服务端 发送

void *Cli_EPS_To_Uart_Thread(void *arg);    //与EPS客户端 发送
void *Cli_Uart_To_EPS_Thread(void *arg);    //与EPS客户端 接收
void *Ser_EPS_To_Uart_Thread(void *arg);    //与EPS服务端 接收
void *Ser_Uart_To_EPS_Thread(void *arg);    //与EPS服务端 发送


#endif

