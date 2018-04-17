#include"common.h"

/*
 *ȥ���ַ����Ҷ˿ո�
 */
char *strtrimr(char *pstr)
{
    int i;
    i = strlen(pstr) - 1;
    while(isspace(pstr[i]) && (i >= 0))
        pstr[i--] = '\0';
    return pstr;
}
/*
 *ȥ���ַ�����˿ո�
 */
char *strtriml(char *pstr)
{
    int i = 0,j;
    j = strlen(pstr) - 1;
    while(isspace(pstr[i]) && (i <= j))
        i++;
    if(0<i)
        strcpy(pstr, &pstr[i]);
    return pstr;
}
/*
 *ȥ���ַ������˿ո�
 */
char *strtrim(char *pstr)
{
    char *p;
    p = strtrimr(pstr);
    return strtriml(p);
}


/*
 *�������ļ���һ�ж���key��value,����itemָ��
 *line--�������ļ�������һ��
 */
int  get_item_from_line(char *line, ITEM *item)
{
    char *p = strtrim(line);
    int len = strlen(p);
    if(len <= 0)
    {
        return 1;//����
    }
    else if(p[0]=='#')
    {
        return 2;
    }
    else
    {
        char *p2 = strchr(p, '=');
        *p2++ = '\0';
        item->key = (char *)malloc(strlen(p) + 1);
        item->value = (char *)malloc(strlen(p2) + 1);
        strcpy(item->key,p);
        strcpy(item->value,p2);
    }
    return 0;//��ѯ�ɹ�
}

int file_to_items(const char *file, ITEM *items, int *num)
{
    char line[1024];
    FILE *fp;
    fp = fopen(file,"r");
    if(fp == NULL)
        return 1;
    int i = 0;
    while(fgets(line, 1023, fp))
    {
        char *p = strtrim(line);
        int len = strlen(p);
        if(len <= 0)
        {
            continue;
        }
        else if(p[0]=='#')
        {
            continue;
        }
        else
        {
            char *p2 = strchr(p, '=');
            /*������Ϊֻ��keyûʲô����*/
            if(p2 == NULL)
                continue;
            *p2++ = '\0';
            items[i].key = (char *)malloc(strlen(p) + 1);
            items[i].value = (char *)malloc(strlen(p2) + 1);
            strcpy(items[i].key,p);
            strcpy(items[i].value,p2);
            i++;
        }
    }
    (*num) = i;
    fclose(fp);
    return 0;
}

/*
 *��ȡvalue
 */
int read_conf_value(const char *key,char *value,const char *file)
{
    char line[1024];
    FILE *fp;
    fp = fopen(file,"r");
    if(fp == NULL)
        return 1;//�ļ��򿪴���
    while(fgets(line, 1023, fp))
    {
        ITEM item;
        if(get_item_from_line(line,&item)==2)//����ע������
            continue;
        if(!strcmp(item.key,key))
        {
            strcpy(value,item.value);
            fclose(fp);
            break;
        }
        free(item.key);
        free(item.value);
    }
    return 0;//�ɹ�

}
int write_conf_value(const char *key,char *value,const char *file)
{
    ITEM items[40];// �ٶ������������20��
    int num;//�洢���ļ���ȡ����Ч��Ŀ
    char tmp[50];

    sprintf(tmp,"==========Write %s==========",key);
    P_Log("%s\n",tmp);
    
    file_to_items(file, items, &num);

    int i=0;
    //����Ҫ�޸ĵ���
    for(i=0; i<num; i++)
    {
        if(!strcmp(items[i].key, key))
        {
            items[i].value = value;
            break;
        }
    }

    // ���������ļ�,Ӧ���б��ݣ�����Ĳ����Ὣ�ļ��������
    FILE *fp;
    fp = fopen(file, "w");
    if(fp == NULL)
        return 1;
    i=0;
    for(i=0; i<num; i++)
    {
        fprintf(fp,"%s=%s\n",items[i].key, items[i].value);
        //free(items[i].key);
        //free(items[i].value);
    }
    fclose(fp);
    return 0;
}

