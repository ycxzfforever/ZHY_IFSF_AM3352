#include "common.h"


char temp[10];

//const char *Conf_Name = "/HQHP/cls.conf"; //配置文件路径和名字
//const char *Pri_ConfName = "/HQHP/price.conf";


void init_sys();
void init_Read_Conf();
void init_Uart();
void Print_Conf();
void Time_Task();

extern void Creat_Timer();



void Time_Task()
{
    /*while(1){
         if(Reconnect_FS_Flag==1){
             Reconnect_FS_Flag=0;
             FS_Tcp_Client_Creat();
         }
         if(Reconnect_EPS_Flag==1){
             Reconnect_EPS_Flag=0;
             EPS_Tcp_Client_Creat();
         }
         usleep(500*1000);
     }*/
}


void test()
{
    Uchar i, str[11], dst[11], tmp[3], *tp;
    Uint data = 0;
    bzero(str, 11);
    bzero(dst, 11);
    bzero(tmp, 3);

    str[0] = 0x12;
    str[1] = 0x34;
    str[2] = 0x56;
    str[3] = 0x78;
    tp = (Uchar *)&data;
    for(i = 4; i > 0; i--, tp++)
    {
        *tp = str[i - 1];
    }
    printf("data before=%d\n", data);
    Dispose_Small_CPU_Data(&data, 4);
    printf("data late=%d\n", data);

}

void Para_set()
{
    para.NZN = 1;
    price.FIE_NU = 1;                   //油价记录数
    para.NZN = conf.Gun_num;
    price.NZN = conf.Gun_num;
    price.PRC_N = 8;
    para.ST=3;
    para.T_TYPE=1;
    ReadNoPayFlag=0;
    ReadTotolFlag=0;
    Cli_Connect_FS = 0;      //等于1时客户端连接上了FS服务器
    Ser_Connect_FS = 0;      //等于1时FS连接上了加气机服务器
    Is_Connect_EPS = 0;      //等于1时连接上了EPS
    FS_OK = 0;               //等于1时，FS通讯良好
    OPT_Server_OK = 0;       //等于1时，加气机server服务创建成功
    Uart_To_EPS_Flag = 0;    //等于1时uart数据开始向eps发送
    Uart_To_FS_Flag = 0;     //等于1时uart数据开始向fs发送
    EPS_To_Uart_Flag = 0;    //等于1时eps数据开始向uart发送
    FS_To_Uart_Flag = 0;     //等于1时fs数据开始向uart发送
    Udp_Send_Flag=0;
    Udp_fd=-1;
    Uart_fd=-1;
    Gpio_fd=-1;
    Cli_FS_fd=-1;
    Ser_FS_fd=-1;
    Cli_EPS_fd=-1;
    FS_Data_Len=0;
    EPS_Data_Len=0;

    Check_Time=0;            //检查网卡是否正常连接
    Time_ms = 0;             //1ms计数
    Time_S = 0;              //秒计数
    Time_100ms = 0;          //100ms计数器
    F0_Time = 0;             //等于1时轮训
    Disconnect_EPS_Flag = 0; //等于1时断开与EPS的连接
    Reconnect_FS_Flag = 0;   //等于1时重新连接FS服务器
    Reconnect_EPS_Flag = 0;  //等于1时重新连接EPS

    StartFuelling=0;
    OPT_IS_Online=200;
    DownLoadPrice=0;
    HaveAuth=0;
    RecvFSAuth=0;
    YanKaCount=0;
    HuiSuoCount=0;
    RequestPayCount=0;
    OverTransCount=0;
    BoartRequestFlag=0;
    FaQiYanKaFlag=0;
    FaQiYanKaTime=0;
    ttc=0;
}

void Print_Conf()
{
    P_Log("EPS_IP=%s\n", conf.EPS_IP);
    P_Log("EPS_Port=%d\n", conf.EPS_Port);
    P_Log("FS_IP=%s\n", conf.FS_IP);
    P_Log("FS_Port=%d\n", conf.FS_Port);
    P_Log("FS_Udp_Port=%d\n", conf.FS_Udp_Port);
    P_Log("Time_Out=%d\n", conf.Time_Out);
    P_Log("Time_Reconnect=%d\n", conf.Time_Reconnect);
    P_Log("My_IP=%s\n", conf.My_IP);
    P_Log("My_EPS_Port=%d\n", conf.My_EPS_Port);
    P_Log("My_FS_Port=%d\n", conf.My_FS_Port);
    P_Log("My_Mask=%s\n", conf.My_Mask);
    P_Log("My_Gateway=%s\n", conf.My_Gateway);
    P_Log("My_Broadcast_IP=%s\n", conf.My_Broadcast_IP);
    P_Log("My_Broadcast_Port=%d\n", conf.My_Broadcast_Port);
    P_Log("IFSF_Node=%s\n", conf.IFSF_Node);
    P_Log("My_Node=%s\n", conf.My_Node);
    P_Log("Pay_Terminal_NO=%s\n", conf.Pay_Terminal_NO);
    P_Log("Price_VER=%d\n", price.VER);
    P_Log("Price[0]=%d\n", price.Price[0]);
    P_Log("Price[1]=%d\n", price.Price[1]);
    P_Log("Price[2]=%d\n", price.Price[2]);
    P_Log("Price[3]=%d\n", price.Price[3]);
    P_Log("Uart_Speed=%d\n", conf.Uart_Speed);
    P_Log("Uart_Data_Bits=%d\n", conf.Uart_Data_Bits);
    P_Log("Uart_Stop_Bits=%d\n", conf.Uart_Stop_Bits);
    P_Log("Uart_Parity=%c\n", conf.Uart_Parity);
}

void Timer_thread_Creat()
{
    int err;
    pthread_t timer1, timer2;
    err = pthread_create(&timer1, NULL, (void*)Creat_Timer, NULL); //创建定时器
    if(err != 0)
        P_Log("Can't create Creat_Timer thread: %s\n", strerror(err));
    else
        P_Log("Create Creat_Timer thread OK\n");
    err = pthread_create(&timer2, NULL, (void*)Time_Task, NULL); //创建周期性处理函数线程
    if(err != 0)
        P_Log("Can't create Time_Task thread: %s\n", strerror(err));
    else
        P_Log("Create Time_Task thread OK\n");
}

void init_Read_Conf()
{
    read_conf_value("Gun_num", temp, Conf_Name);
    conf.Gun_num = atoi(temp);
    para.NZN = conf.Gun_num;
    read_conf_value("EPS_IP", conf.EPS_IP, Conf_Name);
    read_conf_value("EPS_Port", temp, Conf_Name);
    conf.EPS_Port = atoi(temp);
    read_conf_value("FS_IP", conf.FS_IP, Conf_Name);
    read_conf_value("FS_Port", temp, Conf_Name);
    conf.FS_Port = atoi(temp);
    read_conf_value("FS_Udp_Port", temp, Conf_Name);
    conf.FS_Udp_Port = atoi(temp);
    read_conf_value("Time_Out", temp, Conf_Name);
    conf.Time_Out = atoi(temp);
    read_conf_value("Time_Reconnect", temp, Conf_Name);
    conf.Time_Reconnect = atoi(temp);
    read_conf_value("My_IP", conf.My_IP, Conf_Name);
    read_conf_value("My_EPS_Port", temp, Conf_Name);
    conf.My_EPS_Port = atoi(temp);
    read_conf_value("My_FS_Port", temp, Conf_Name);
    conf.My_FS_Port = atoi(temp);
    read_conf_value("My_Mask", conf.My_Mask, Conf_Name);
    read_conf_value("My_Gateway", conf.My_Gateway, Conf_Name);
    read_conf_value("My_Broadcast_IP", conf.My_Broadcast_IP, Conf_Name);
    read_conf_value("My_Broadcast_Port", temp, Conf_Name);
    conf.My_Broadcast_Port = atoi(temp);
    read_conf_value("IFSF_Node", conf.IFSF_Node, Conf_Name);
    read_conf_value("My_Node", conf.My_Node, Conf_Name);
    read_conf_value("Pay_Terminal_NO", conf.Pay_Terminal_NO, Conf_Name);
    read_conf_value("Pri_Ver", temp, Pri_ConfName);
    price.VER = atoi(temp);
    conf.Pri_Ver = price.VER;
    read_conf_value("price0", temp, Pri_ConfName);
    price.Price[0] = atoi(temp);
    read_conf_value("price1", temp, Pri_ConfName);
    price.Price[1] = atoi(temp);
    read_conf_value("price2", temp, Pri_ConfName);
    price.Price[2] = atoi(temp);
    read_conf_value("price3", temp, Pri_ConfName);
    price.Price[3] = atoi(temp);
    read_conf_value("Uart_NO", conf.Uart_NO, Conf_Name);
    read_conf_value("Uart_Speed", temp, Conf_Name);
    conf.Uart_Speed = atoi(temp);
    read_conf_value("Uart_Data_Bits", temp, Conf_Name);
    conf.Uart_Data_Bits = atoi(temp);
    read_conf_value("Uart_Stop_Bits", temp, Conf_Name);
    conf.Uart_Stop_Bits = atoi(temp);
    read_conf_value("Uart_Parity", temp, Conf_Name);
    conf.Uart_Parity = temp[0];
}

void init_Uart()
{
    Open_Uart(conf.Uart_NO);
    set_speed(Uart_fd, conf.Uart_Speed); //设置串口波特率
    set_Parity(Uart_fd, conf.Uart_Data_Bits, conf.Uart_Stop_Bits, conf.Uart_Parity); //设置8位数据位，1位停止位，无校验等其他设置。
    Uart_Pthread_Creat();//串口接收和发送线程创建
}

void init_sys()
{
    pthread_mutex_init(&Data_To_Uart, NULL);
    pthread_mutex_init(&FS_Recv_Data, NULL);
    pthread_mutex_init(&cs_log, NULL);
    pthread_mutex_init(&Uart_Recv_Data, NULL);

    //读取配置文件
    init_Read_Conf();
#ifdef Debug
    Print_Conf();
#endif

    //串口初始化
    init_Uart();

    //定时器线程创建
    Timer_thread_Creat();

    //TCP线程创建
    Tcp_Pthread_Creat();

    //UDP线程创建
    Udp_Pthread_Creat();
}

int main(int argc, char *argv[])
{
    daemon(1, 0);
    char sh[40];
    Para_set();
    init_sys();
    sprintf(sh, "ntpdate %s", conf.FS_IP); //向FS请求网络对时
#ifdef Debug
    P_Log("network time sync:%s\n", sh);
#endif
    system(sh);
    pause();
    return 0;
}
