#ifndef MYSTRING_DEFINED

#include <malloc.h>

typedef char** SPLITDATA;

//プロトタイプ宣言
void delsplit(int, SPLITDATA);
int split(char*, char*, SPLITDATA*);

//splitで確保した配列を開放
void delsplit(int len, SPLITDATA SplitedData)
{
	int i;
	for (i = 0; i < len; i++)
		free(SplitedData[i]);
	free(SplitedData);
}

//区切り文字を指定して、splitedDataに区分けして格納
int split(char *str, char *kugiri, SPLITDATA *splitedData)
{
	int slen;
	int klen;
	char *temp;

	int i, j;
	int strc = 1, chrc = 1;

	slen = strlen(str);
	klen = strlen(kugiri);

	(*splitedData) = (char**)malloc(sizeof(char*));
	(*splitedData)[0] = (char*)malloc(sizeof(char));
	(*splitedData)[0][0] = '\0';

	temp = (char*)malloc(sizeof(char)*klen);

	for (i = 0; i < slen;/* i++ */)
	{

		for (j = 0; j < klen && i + klen - 1 < slen; j++)
		{
			temp[j] = str[i + j];
		}
		temp[j] = '\0';
		if (i + klen - 1 < slen && strcmp(kugiri, temp) == 0)
		{
			strc++;
			chrc = 1;
			(*splitedData) = (char**)realloc(*splitedData, sizeof(char*)*strc);
			(*splitedData)[strc - 1] = (char*)malloc(sizeof(char));
			(*splitedData)[strc - 1][0] = '\0';
			i += klen;
		}
		else
		{
			chrc++;
			(*splitedData)[strc - 1] = (char*)realloc((*splitedData)[strc - 1], sizeof(char)*chrc);
			(*splitedData)[strc - 1][chrc - 2] = str[i];
			(*splitedData)[strc - 1][chrc - 1] = '\0';
			i++;
		}

	}

	return strc;
}

#define MYSTRING_DEFINED
#endif
