#ifndef _FILE_H
#define _FILE_H


char get_value[30];
typedef struct item_t
{
    char *key;
    char *value;
} ITEM;

/*
 *去除字符串右端空格
 */
char *strtrimr(char *pstr);
/*
 *去除字符串左端空格
 */
char *strtriml(char *pstr);
/*
 *去除字符串两端空格
 */
char *strtrim(char *pstr);

/*
 *从配置文件的一行读出key或value,返回item指针
 *line--从配置文件读出的一行
 */
int  get_item_from_line(char *line, ITEM *item);


int file_to_items(const char *file, ITEM *items, int *num);

/*
 *读取value
 */
int read_conf_value(const char *key,char *value,const char *file);

/*
 *写value
 */
int write_conf_value(const char *key,char *value,const char *file);


#endif

