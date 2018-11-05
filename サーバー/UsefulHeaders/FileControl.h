#ifndef FILECONTROL_DEFINED

#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <malloc.h>
#include <shellapi.h>
#include <windows.h>

#define FILE_PATH_LENGTH 256

#define DATAFILE 0
#define APPFILE 1

//プロトタイプ宣言
int filecopy(char *, char *);
int filedel(char *, ...);
int MakeDir(char *, ...);
bool exist(char *, ...);
void ExecCmd(char *, int, char *);

void dirlist(char[], int, char *, char *);
char *GetCurName(void);

//ファイルコピー
int filecopy(char *CopyMotoFileName, char *CopySakiFileName)
{
	FILE *MotoFilePtr;
	FILE *SakiFilePtr;
	unsigned char c;

	fopen_s(&MotoFilePtr, CopyMotoFileName, "rb");

	if (MotoFilePtr != NULL && strcmp(CopyMotoFileName, CopySakiFileName) != 0)
	{
		fopen_s(&SakiFilePtr, CopySakiFileName, "wb");

		if (SakiFilePtr != NULL)
		{
			while (feof(MotoFilePtr)==0)
			{
				fread(&c, sizeof(unsigned char), 1, MotoFilePtr);
				fwrite(&c, sizeof(unsigned char), 1, SakiFilePtr);
			}

			//ファイルポインタ後始末
			fclose(SakiFilePtr);
		}

		//ファイルポインタ後始末
		fclose(MotoFilePtr);

		return 0;
	}
	else
	{
		return -1;
	}
}

int filedel(char *filenameFormat, ...)
{
	va_list list;
	char buf[256];

	va_start(list, filenameFormat);
	vsprintf_s(buf, 256, filenameFormat, list);
	va_end(list);

	return remove(buf);
}

int MakeDir(char *DirNameFormat, ...)
{
	va_list list;
	char buf[256];

	va_start(list, DirNameFormat);
	vsprintf_s(buf, 256, DirNameFormat, list);
	va_end(list);

	return _mkdir(buf);
}

bool exist(char *FileNameFormat, ...)
{
	char buf[256];
	FILE *fp;
	va_list list;

	va_start(list, FileNameFormat);
	vsprintf_s(buf, 256, FileNameFormat, list);
	va_end(list);

	fopen_s(&fp, buf, "r");
	if (fp == NULL){
		return false;
	}
	else{
		fclose(fp);
		return true;
	}
}

void ExecCmd(char *command, int Mode, char *param = NULL)
{
	if (Mode)	ShellExecute(NULL, NULL, command, NULL, NULL, SW_SHOWNORMAL);
	else		ShellExecute(NULL, "open", command, param, NULL, SW_SHOWNORMAL);
}

void dirlist(char Dest[], int destsize, char *FindDir, char *FindFile)
{
	WIN32_FIND_DATA find;
	HANDLE hFind;
	TCHAR szPath[MAX_PATH];
	LPTSTR p;

	//GetModuleFileName(NULL, szPath, MAX_PATH);	//実行ファイルのあるパスを取得
	lstrcpy(szPath, FindDir);

	p = _tcsrchr(szPath, _T('\\'));					//最後の\マークを検出
	*p = '\0';										//最後の\マークを消去
	lstrcat(szPath, TEXT("\\"));
	lstrcat(szPath, TEXT(FindFile));

	hFind = FindFirstFile(szPath, &find);			//実行ファイルのあるディレクトリを検索

	strcpy_s(Dest, destsize, "");

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			//現在のディレクトリ(.)や前のディレクトリ(..)がパスとして出てしまうので除外
			if (lstrcmpi(find.cFileName, TEXT(".")) != 0 && lstrcmpi(find.cFileName, TEXT("..")) != 0)
			{
				//printfDx("%s\n", find.cFileName);	//ここで出力
				sprintf_s(Dest, destsize, "%s%s\n", Dest, find.cFileName);
			}
		} while (FindNextFile(hFind, &find));

		FindClose(hFind);
	}
}

char *GetCurName(void)
{
	static TCHAR szPath[MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH);	//実行ファイルのあるパスを取得

	return szPath;
}

#define FILECONTROL_DEFINED
#endif
