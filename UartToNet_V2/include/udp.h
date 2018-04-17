#ifndef _UDP_H
#define _UDP_H

unsigned char Udp_Send_Buf[16];
unsigned char Udp_Recv_Buf[16];
unsigned char Tmp_Buf[16];
int Udp_Send_Flag;
char Udp_fd;

void Str_Split(char *str);
void Udp_Send_Buf_Process();
int Udp_Server_Creat();
int Udp_Client_Creat();
int Udp_Pthread_Creat();
void Deal_recv_Udp_data();


#endif
