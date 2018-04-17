#ifndef _CMD_H
#define _CMD_H

void Time_Process();


void FCS_To_OPT_Header_Send(Uchar cmdID);//֡ͷ
void FCS_To_OPT_Tail_Send(Uchar cmdID,Uchar len);//֡β

void FCS_To_JKOPT_51();
void FCS_To_JKOPT_52(Uchar no,Uchar * ParaNo,Uchar * paravalue);
void FCS_To_JKOPT_69(Uchar * UID);
void FCS_To_JKOPT_70(Uchar flag, Uchar * num);
void FCS_To_JKOPT_71();
void FCS_To_JKOPT_72();
void FCS_To_JKOPT_73();
void FCS_To_JKOPT_74();


#endif

