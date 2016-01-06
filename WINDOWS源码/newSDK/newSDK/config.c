#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
*�������ļ��ж�ȡ��Ӧ��ֵ
*���������1�������ļ�·�� 2��ƥ���� 3������洢�ռ�
*�����ų��˿��У���=��ǰ�������ݣ��ޡ�=�������
*/
int CFG_GetProfileString(char *conf_name, char *config_buff)
{
	char config_linebuf[256];
	char line_name[40];
	char exchange_buf[256];
	char *config_sign = "=";
	char *leave_line;
	FILE *f;
	char *conf_path = CONFIG_FILE_NAME;
	f = fopen(conf_path, "r");
	if (f == NULL)
	{
		//printf("OPEN CONFIG FALID/n");
		return -1;
	}
	fseek(f, 0, SEEK_SET);
	while (fgets(config_linebuf, 256, f) != NULL)
	{
		if (strlen(config_linebuf) < 3)														//�ж��Ƿ��ǿ���
		{
			continue;
		}
		if (config_linebuf[strlen(config_linebuf) - 1] == 10)								//ȥ�����һλ��/n�����
		{

			memset(exchange_buf, 0, sizeof(exchange_buf));
			strncpy(exchange_buf, config_linebuf, strlen(config_linebuf) - 1);
			memset(config_linebuf, 0, sizeof(config_linebuf));
			strcpy(config_linebuf, exchange_buf);
		}
		memset(line_name, 0, sizeof(line_name));
		leave_line = strstr(config_linebuf, config_sign);
		if (leave_line == NULL)																//ȥ����"="�����
		{
			continue;
		}
		int leave_num = leave_line - config_linebuf;
		strncpy(line_name, config_linebuf, leave_num);
		if (strcmp(line_name, conf_name) == 0)
		{
			strncpy(config_buff, config_linebuf + (leave_num + 1), strlen(config_linebuf) - leave_num - 1);
			break;
		}
		if (fgetc(f) == EOF)
		{
			break;
		}
		fseek(f, -1, SEEK_CUR);
		memset(config_linebuf, 0, sizeof(config_linebuf));
	}
	fclose(f);
	return 0;
}
/*
*����޸��ļ����������ļ��д��ڱ���ֶΣ�������޸ģ����������������ӣ�
*
*���������1�������ļ�·�� 2��ƥ���� 3���滻����ӵ�����
*
*/
int CFG_SetProfileString(char *conf_name, char *config_buff)
{
	char config_linebuf[256];
	char line_name[40];
	char *config_sign = "=";
	char *leave_line;
	int  alter_sign = 0;
	char *conf_path = CONFIG_FILE_NAME;

	FILE *f;
	f = fopen(conf_path, "r+");
	if (f == NULL)
	{
		//printf("OPEN CONFIG FALID\n");
		return -1;
	}
	fseek(f, 0, SEEK_END);
	long congig_lenth = ftell(f);
	int configbuf_lenth = strlen(config_buff);
	configbuf_lenth = configbuf_lenth + 5;
	char sum_buf[MAX_CFG_FILELEN];
	memset(sum_buf, 0, sizeof(sum_buf));
	fseek(f, 0, SEEK_SET);
	while (fgets(config_linebuf, 256, f) != NULL)
	{
		if (strlen(config_linebuf) < 3)														//�ж��Ƿ��ǿ���
		{
			strcat(sum_buf, config_linebuf);
			continue;
		}
		leave_line = NULL;
		leave_line = strstr(config_linebuf, config_sign);
		if (leave_line == NULL)													            //ȥ����"="�����
		{
			strcat(sum_buf, config_linebuf);
			continue;
		}
		int leave_num = leave_line - config_linebuf;
		memset(line_name, 0, sizeof(line_name));
		strncpy(line_name, config_linebuf, leave_num);
		if (strcmp(line_name, conf_name) == 0)
		{
			strcat(sum_buf, config_buff);
			strcat(sum_buf, NEWLINE);
			alter_sign = 1;
		}
		else
		{
			strcat(sum_buf, config_linebuf);
		}
		if (fgetc(f) == EOF)
		{
			break;
		}
		fseek(f, -1, SEEK_CUR);
		memset(config_linebuf, 0, sizeof(config_linebuf));
	}
	if (alter_sign == 0)
	{
		strcat(sum_buf, config_buff);
		strcat(sum_buf, NEWLINE);
	}
	//printf("---sum_buf---->%s<----------\n", sum_buf);
	remove(conf_path);
	fclose(f);
	FILE *fp;
	fp = fopen(conf_path, "w+");
	if (fp == NULL)
	{
		//printf("OPEN CONFIG FALID/n");
		return -2;
	}
	fseek(fp, 0, SEEK_SET);
	fputs(sum_buf, fp);
	fclose(fp);
	return 0;
}
/*
*ɾ�������ļ�����
*
*���������1�������ļ�·�� 2��ƥ����
*
*/
int CFG_DelProfileString(char *conf_name)
{
	char config_linebuf[256];
	char line_name[40];
	char *config_sign = "=";
	char *leave_line;
	char *conf_path = CONFIG_FILE_NAME;

	FILE *f;
	f = fopen(conf_path, "r+");
	if (f == NULL)
	{	return -1;}
	fseek(f, 0, SEEK_END);
	long congig_lenth = ftell(f);
	char sum_buf[MAX_CFG_FILELEN];
	memset(sum_buf, 0, sizeof(sum_buf));
	fseek(f, 0, SEEK_SET);
	while (fgets(config_linebuf, 256, f) != NULL)
	{
		if (strlen(config_linebuf) < 3)														//�ж��Ƿ��ǿ���
		{
			strcat(sum_buf, config_linebuf);
			continue;
		}
		leave_line = NULL;
		leave_line = strstr(config_linebuf, config_sign);
		if (leave_line == NULL)																//ȥ����"="�����
		{
			strcat(sum_buf, config_linebuf);
			continue;
		}
		int leave_num = leave_line - config_linebuf;
		memset(line_name, 0, sizeof(line_name));
		strncpy(line_name, config_linebuf, leave_num);
		if (strcmp(line_name, conf_name) == 0)
		{}
		else
		{
			strcat(sum_buf, config_linebuf);
		}

		if (fgetc(f) == EOF)
		{	break;}
		fseek(f, -1, SEEK_CUR);
		memset(config_linebuf, 0, sizeof(config_linebuf));
	}
	//printf("---sum_buf---->%s<----------/n", sum_buf);
	remove(conf_path);
	fclose(f);
	FILE *fp;
	fp = fopen(conf_path, "w+");
	if (fp == NULL)
	{
		//printf("OPEN CONFIG FALID/n");
		return -2;
	}
	fseek(fp, 0, SEEK_SET);
	fputs(sum_buf, fp);
	fclose(fp);
	return 0;
}