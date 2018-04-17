#ifndef _FILE_H
#define _FILE_H


char get_value[30];
typedef struct item_t
{
    char *key;
    char *value;
} ITEM;

/*
 *ȥ���ַ����Ҷ˿ո�
 */
char *strtrimr(char *pstr);
/*
 *ȥ���ַ�����˿ո�
 */
char *strtriml(char *pstr);
/*
 *ȥ���ַ������˿ո�
 */
char *strtrim(char *pstr);

/*
 *�������ļ���һ�ж���key��value,����itemָ��
 *line--�������ļ�������һ��
 */
int  get_item_from_line(char *line, ITEM *item);


int file_to_items(const char *file, ITEM *items, int *num);

/*
 *��ȡvalue
 */
int read_conf_value(const char *key,char *value,const char *file);

/*
 *дvalue
 */
int write_conf_value(const char *key,char *value,const char *file);


#endif

