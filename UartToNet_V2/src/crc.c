#include "common.h"

Ushort const crc16_table[256] =
{
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

const unsigned char  CRC8_TAB[]=     //多项式X^8 + X^5 + X^4 + 1
{
    0x00,0x5e,0xbc,0xe2,0x61,0x3f,0xdd,0x83,//0~7
    0xc2,0x9c,0x7e,0x20,0xa3,0xfd,0x1f,0x41,//8~15
    0x9d,0xc3,0x21,0x7f,0xfc,0xa2,0x40,0x1e,//16_23
    0x5f,0x01,0xe3,0xbd,0x3e,0x60,0x82,0xdc,//24~31
    0x23,0x7d,0x9f,0xc1,0x42,0x1c,0xfe,0xa0,//32~39
    0xe1,0xbf,0x5d,0x03,0x80,0xde,0x3c,0x62,//40~47
    0xbe,0xe0,0x02,0x5c,0xdf,0x81,0x63,0x3d,//48~55
    0x7c,0x22,0xc0,0x9e,0x1d,0x43,0xa1,0xff,//56~63
    0x46,0x18,0xfa,0xa4,0x27,0x79,0x9b,0xc5,//64~71
    0x84,0xda,0x38,0x66,0xe5,0xbb,0x59,0x07,//72~79
    0xdb,0x85,0x67,0x39,0xba,0xe4,0x06,0x58,//80~87
    0x19,0x47,0xa5,0xfb,0x78,0x26,0xc4,0x9a,//88~95
    0x65,0x3b,0xd9,0x87,0x04,0x5a,0xb8,0xe6,//96~103
    0xa7,0xf9,0x1b,0x45,0xc6,0x98,0x7a,0x24,//104~111
    0xf8,0xa6,0x44,0x1a,0x99,0xc7,0x25,0x7b,//112~119
    0x3a,0x64,0x86,0xd8,0x5b,0x05,0xe7,0xb9,//120~127
    0x8c,0xd2,0x30,0x6e,0xed,0xb3,0x51,0x0f,//128~135
    0x4e,0x10,0xf2,0xac,0x2f,0x71,0x93,0xcd,//136~143
    0x11,0x4f,0xad,0xf3,0x70,0x2e,0xcc,0x92,//144~151
    0xd3,0x8d,0x6f,0x31,0xb2,0xec,0x0e,0x50,//152~159
    0xaf,0xf1,0x13,0x4d,0xce,0x90,0x72,0x2c,//160~167
    0x6d,0x33,0xd1,0x8f,0x0c,0x52,0xb0,0xee,//168~175
    0x32,0x6c,0x8e,0xd0,0x53,0x0d,0xef,0xb1,//176~183
    0xf0,0xae,0x4c,0x12,0x91,0xcf,0x2d,0x73,//184~191
    0xca,0x94,0x76,0x28,0xab,0xf5,0x17,0x49,//192~199
    0x08,0x56,0xb4,0xea,0x69,0x37,0xd5,0x8b,//200~207
    0x57,0x09,0xeb,0xb5,0x36,0x68,0x8a,0xd4,//208~215
    0x95,0xcb,0x29,0x77,0xf4,0xaa,0x48,0x16,//216~223
    0xe9,0xb7,0x55,0x0b,0x88,0xd6,0x34,0x6a,//224~231
    0x2b,0x75,0x97,0xc9,0x4a,0x14,0xf6,0xa8,//232~239
    0x74,0x2a,0xc8,0x96,0x15,0x4b,0xa9,0xf7,//240~247
    0xb6,0xe8,0x0a,0x54,0xd7,0x89,0x6b,0x35,//248~255
};


/*****************************************************************************
** 函数名:
** 描述:
*****************************************************************************/
Ushort crc16_byte(Ushort crc, Uchar data)
{
    return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

/*****************************************************************************
** 函数名:
** 描述:  crc:crc寄存器的初值
            buffer：指向数组的指针
            len：数组长度
*****************************************************************************/
Ushort ComputeCRC(Uchar *buffer,Ushort len)
{
    Ushort crc=0;       //初值
    while(len--)
    {
        crc = crc16_byte(crc, *buffer++);
    }
    return crc;
}

Uchar int_crc8(Uchar *p,Ushort m)/////////////////////CRC8
{

    Ushort  i;
    Uchar   CRC8=0;

    for(i=0; i<m; i++)    CRC8 = CRC8_TAB[p[i]^CRC8];
    return(CRC8);
}


