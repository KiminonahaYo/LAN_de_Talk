#include "DxLib.h"
#include <math.h>
#include <string.h>

#include "UsefulHeaders/useful.h"

#define STR_COLOR   0xffffff
#define STR_COLOR_2	0x000000
#define BACK_JOIN	0xccccff
#define BACK_DIS	0xffcccc
#define BACK_NORMAL 0xcccccc
#define BACK_COLOR	0xffffff
#define SCREEN_X 640
#define SCREEN_Y 480

//プロトタイプ宣言

typedef enum
{
	DAT_JOINED,
	DAT_DIS,
	DAT_NAME,

	DAT_CHAT,

	DAT_ROOM_N,
	DAT_ROOM_NAME,
	DAT_ROOM_ISUSE,
	DAT_ROOM_ADD,

	DAT_CREATE_ROOM,
	DAT_DEST_ROOM,

	DAT_JOIN_ROOM,
	DAT_DIS_ROOM,

	DAT_TAIKAI_ROOM,

	DAT_NAME_ERROR,
	DAT_CHAT_ERROR,
	DAT_CONNECT_ERROR
}
DATATYPE;

typedef struct
{
	char *content;
	int len;
}
CHAT;

typedef struct
{
	char *name;
	bool isuse;
}
ROOMS;

CHAT Chat;
char name[32];

int NetHandle;

IPDATA Ip = { 127, 0, 0, 1 };

ROOMS *room;
int room_n = 0;

int inputname(void);
int input_ip(IPDATA*);
int inputchat(void);
void sendchat(void);
int input_roomname(void);

void communicate(void);
int checklost(void);
void disp(void);

/*
void send(DATATYPE, void*, size_t);
int recv_str(DATATYPE, char**);
*/
void connect_set(void);

void init(void);
void dest(void);

void init_val(void);
void dest_val(void);

void send_val(DATATYPE, void*, size_t);
int recv_str(DATATYPE, char**);
int recv_val(DATATYPE, void*);
int recv_roominfo(void);

void send(DATATYPE, void*, size_t);
int recv(DATATYPE, void*);

void select_room(void);
void del_select_room(void);

bool brkflag = false;

int pos_y_min = 0;
int before_pos_y_min;
int pos_y = pos_y_min;
bool pos_y_update_flag = false;

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hP, LPSTR lpC, int nC)
{
	init();

	connect_set();

	send(DAT_NAME, name, strlen(name) + 1);

	while (1)
	{
		if (brkflag)
			break;

		init_val();

		select_room();

		while (1)
		{
			if (Stick(KEY_INPUT_SPACE))
				inputchat();
			if (checklost() == -1 || ProcessMessage() == -1)
				brkflag = true;
			if (Stick(KEY_INPUT_ESCAPE))
				break;
			if (brkflag)
				break;

			communicate();

			ClearDrawScreen();

			disp();
			DrawFormatString(10, SCREEN_Y - 20, STR_COLOR_2, "SPACEでチャット入力、ホイールでスクロール、Escで部屋を退出");

			ScreenFlip();
		}
		send(DAT_DIS_ROOM, NULL, 0);
		dest_val();
	}

	dest();

	return 0;		//正常終了
}

void select_room(void)
{
	int i;
	int sel_no = 0;
	int ret;

	clsDx();
	while (!ProcessMessage())
	{
		if (checklost() == -1 || ProcessMessage() == -1)
			brkflag = true;
		if (brkflag)
			break;

		communicate();

		ClearDrawScreen();

		//操作部
		if (Stick(KEY_INPUT_UP))
		{
			if (sel_no > 0)
				sel_no--;
			else
				sel_no = room_n - 1;
		}

		if (Stick(KEY_INPUT_DOWN))
		{
			if (sel_no < room_n - 1)
				sel_no++;
			else
				sel_no = 0;
		}

		if (Stick(KEY_INPUT_RETURN))
		{
			if (room[sel_no].isuse == true && room_n >= 1)
			{
				send(DAT_JOIN_ROOM, &sel_no, sizeof(int));
				break;
			}
		}

		if (Stick(KEY_INPUT_F1))
		{
			ret = input_roomname();
			if (ret == -1)
			{
				DelArray1Dim(Chat.content);
				DxLib_End();
				exit(2);
			}
		}

		//表示部
		DrawFormatString(10, 10, STR_COLOR, "チャットをするルームを選んでください");
		for (i = 0; i < room_n; i++)
		{
			if (sel_no == i)
			{
				if (room[i].isuse == true)
					DrawBox(10, 40 + i * 30, SCREEN_X - 10, 40 + (i + 1) * 30, GetColor(100, 200, 100), true);
				else
					DrawBox(10, 40 + i * 30, SCREEN_X - 10, 40 + (i + 1) * 30, GetColor(200, 100, 100), true);
			}
			else
			{
				if (room[i].isuse == true)
					DrawBox(10, 40 + i * 30, SCREEN_X - 10, 40 + (i + 1) * 30, GetColor(100, 200, 100), false);
				else
					DrawBox(10, 40 + i * 30, SCREEN_X - 10, 40 + (i + 1) * 30, GetColor(200, 100, 100), false);
			}

			if (room[i].isuse == true)
			{
				DrawFormatString(10, 40 + i * 30, STR_COLOR, "ルーム%d : %s", i + 1, room[i].name);
			}
		}
		if (room_n == 0)
		{
			DrawBox(10, 40, SCREEN_X - 10, SCREEN_Y - 50, GetHSVColor(60, 50, 200), TRUE);
			DrawFormatString(SCREEN_X / 2 - 200, SCREEN_Y / 2 - 20, STR_COLOR_2, "部屋がまだありません。F1で作ってみましょう。");
		}
		DrawFormatString(10, SCREEN_Y - 40, STR_COLOR, "↑↓で選択、Enter : ルームに参加する、F1 : ルームを作成する");

		ScreenFlip();
	}
}

void del_select_room(void)
{
}

void init(void)
{
	//画面サイズとウィンドウモード設定
	SetGraphMode(SCREEN_X, SCREEN_Y, 32);
	ChangeWindowMode(true);										//ウィンドウモードで起動、コメントアウトでフルスクリーン
	SetDoubleStartValidFlag(true);
	SetWindowIconID(101);
	SetAlwaysRunFlag(true);
	SetOutApplicationLogValidFlag(false);

	//-------------------------------------------------------------------------------------------

	if (DxLib_Init() == -1) exit(-1);					//DXライブラリ初期化

	SetBackgroundColor(0, 0, 0);								//背景色
	SetDrawScreen(DX_SCREEN_BACK);								//裏画面に描画

	//DX ライブラリを初期化し、初期化が失敗したらプログラム終了
	if (DxLib_Init() == -1) exit(-1);

	SetFontSize(16);

	name[0] = '\0';
}

void dest(void)
{
	CloseNetWork(NetHandle);

	DxLib_End();	//DX ライブラリを終了
}

void init_val(void)
{
	GetArray1Dim(char, Chat.content, 1);
	Chat.content[0] = '\0';
	Chat.len = 0;

	GetArray1Dim(ROOMS, room, 0);
	room_n = 0;
}

void dest_val(void)
{
	int i;

	DelArray1Dim(Chat.content);
	for (i = 0; i < room_n; i++)
		DelArray1Dim(room[i].name);
	DelArray1Dim(room);
}

void connect_set(void)
{
	int ret;

	ret = inputname();
	if (ret == -1)
	{
		DelArray1Dim(Chat.content);
		DxLib_End();
		exit(2);
	}

	do
	{
		ret = input_ip(&Ip);
		if (ret == -1)
		{
			DelArray1Dim(Chat.content);
			DxLib_End();
			exit(2);
		}

		NetHandle = ConnectNetWork(Ip, 9850);
	} while (NetHandle == -1);
}

int checklost(void)
{
	int lost_handle;

	lost_handle = GetLostNetWork();

	if (lost_handle == NetHandle)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int inputname(void)
{
	int ret;

	ClearDrawScreen();

	SetFontSize(70);
	DrawFormatString(SCREEN_X / 6, SCREEN_Y / 5, GetColor(255, 255, 255), "LAN de Talk");
	SetFontSize(16);

	DrawFormatString(10, SCREEN_Y - 20, STR_COLOR, "名前を入力 > ");

	ScreenFlip();

	ret = KeyInputString(10 + 120, SCREEN_Y - 20, 31, name, TRUE);

	Stick(KEY_INPUT_ESCAPE);
	Stick(KEY_INPUT_RETURN);

	if (ret == 1)	return 0;
	else			return -1;
}

int input_ip(IPDATA *Ip)
{
	int cursor = 0;
	int ip_input_pos = 0;
	bool brkflag = false;
	char inputdata[256];
	char c;
	int i;

	int framecnt = 0;
	
	static bool confflag = true;

	while (1)
	{
		if (ProcessMessage() == -1 || CheckHitKey(KEY_INPUT_ESCAPE))
		{
			brkflag = true;
		}
		if (brkflag)
			break;

		ClearDrawScreen();

		SetFontSize(70);
		DrawFormatString(SCREEN_X / 6, SCREEN_Y / 5, GetColor(255, 255, 255), "LAN de Talk");
		SetFontSize(16);

		if (confflag == true)
		{
			ClearInputCharBuf();
			switch (ip_input_pos)
			{
				case 0: itoa(Ip->d1, inputdata, 10); break;
				case 1: itoa(Ip->d2, inputdata, 10); break;
				case 2: itoa(Ip->d3, inputdata, 10); break;
				case 3: itoa(Ip->d4, inputdata, 10); break;
			}
			cursor = 0;

			confflag = false;
		}

		c = m_inputnum();

		if (c >= 0 && c <= 9)
		{
			inputdata[cursor] = '0' + c;
			inputdata[cursor + 1] = '\0';

			if (atoi(inputdata) < 256)
			{
				cursor++;
			}
			else
			{
				if (atoi(inputdata) < 1000)
				{
					cursor++;
				}
				inputdata[0] = '2';
				inputdata[1] = '5';
				inputdata[2] = '5';
				inputdata[3] = '\0';
			}
		}
		else if (c == 10)
		{
			if (cursor > 0) cursor--;
			if (cursor >= 1)
				inputdata[cursor] = '\0';
			else
			{
				inputdata[cursor] = '0';
				inputdata[cursor + 1] = '\0';
			}
		}

		if (Stick(KEY_INPUT_LEFT, true))
		{
			if (ip_input_pos > 0)
			{
				confflag = true;
				switch (ip_input_pos)
				{
				case 0: Ip->d1 = atoi(inputdata); break;
				case 1: Ip->d2 = atoi(inputdata); break;
				case 2: Ip->d3 = atoi(inputdata); break;
				case 3: Ip->d4 = atoi(inputdata); break;
				}
				ip_input_pos--;

				continue;
			}
		}

		if (Stick(KEY_INPUT_RIGHT, true))
		{
			if (ip_input_pos < 3)
			{
				confflag = true;
				switch (ip_input_pos)
				{
				case 0: Ip->d1 = atoi(inputdata); break;
				case 1: Ip->d2 = atoi(inputdata); break;
				case 2: Ip->d3 = atoi(inputdata); break;
				case 3: Ip->d4 = atoi(inputdata); break;
				}
				ip_input_pos++;

				continue;
			}
		}

		if (Stick(KEY_INPUT_RETURN))
		{
			if (ip_input_pos < 3)
			{
				confflag = true;
				switch (ip_input_pos)
				{
				case 0: Ip->d1 = atoi(inputdata); break;
				case 1: Ip->d2 = atoi(inputdata); break;
				case 2: Ip->d3 = atoi(inputdata); break;
				case 3: Ip->d4 = atoi(inputdata); break;
				}

				ip_input_pos++;

				continue;
			}
			else
			{
				confflag = true;

				switch (ip_input_pos)
				{
				case 0: Ip->d1 = atoi(inputdata); break;
				case 1: Ip->d2 = atoi(inputdata); break;
				case 2: Ip->d3 = atoi(inputdata); break;
				case 3: Ip->d4 = atoi(inputdata); break;
				}

				break;
			}
		}

		DrawFormatString(10, SCREEN_Y - 20, STR_COLOR, "IPアドレス入力 > ");
		for (i = 0; i < 4; i++)
		{
			if (ip_input_pos == i)
			{
				DrawBox(10 + 170 + i * 70, SCREEN_Y - 20, 10 + 170 + (i + 1) * 70 - 30, SCREEN_Y, GetColor(50, 50, 50), TRUE);
				if (i < 3)
					DrawFormatString(10 + 170 + i * 70, SCREEN_Y - 20, STR_COLOR, "%s.", inputdata);
				else
					DrawFormatString(10 + 170 + i * 70, SCREEN_Y - 20, STR_COLOR, "%s", inputdata);

				if (framecnt % 60 < 30)
					DrawLine(
					10 + 170 + i * 70,
					SCREEN_Y - 20,
					10 + 170 + i * 70,
					SCREEN_Y, 
					STR_COLOR);
			}
			else
			{
				switch (i)
				{
				case 0:
					DrawFormatString(10 + 170 + i * 70, SCREEN_Y - 20, STR_COLOR, "%d.", Ip->d1);
					break;
				case 1:
					DrawFormatString(10 + 170 + i * 70, SCREEN_Y - 20, STR_COLOR, "%d.", Ip->d2);
					break;
				case 2:
					DrawFormatString(10 + 170 + i * 70, SCREEN_Y - 20, STR_COLOR, "%d.", Ip->d3);
					break;
				case 3:
					DrawFormatString(10 + 170 + i * 70, SCREEN_Y - 20, STR_COLOR, "%d", Ip->d4);
					break;
				}
			}
		}

		framecnt++;

		ScreenFlip();
	}

	if (brkflag)
		return -1;
	else
		return 0;
}

int inputchat(void)
{
	int ret;
	char value[256];
	int InputHandle;
	int InputStatus = 2;

	InputHandle = MakeKeyInput(255, true, false, false);

	SetKeyInputStringColor(STR_COLOR_2, STR_COLOR_2, STR_COLOR_2, STR_COLOR_2, STR_COLOR_2, STR_COLOR_2, STR_COLOR);

	do
	{
		SetActiveKeyInput(InputHandle);

		while (!ProcessMessage())
		{
			communicate();

			ClearDrawScreen();

			disp();
			DrawFormatString(10, SCREEN_Y - 20, STR_COLOR_2, "Enter送信、Esc解除 %s > ", name);

			if ((InputStatus = CheckKeyInput(InputHandle)), InputStatus == 1 || InputStatus == 2 || InputStatus == -1)
				break;

			DrawKeyInputString(10 + 22 * 9 + strlen(name) * 9, SCREEN_Y - 20, InputHandle);
			DrawKeyInputModeString(SCREEN_X - 9 * 12, 10 + 30);

			ScreenFlip();
		}

		GetKeyInputString(value, InputHandle);
	} while (strcmp(value, "") == 0 && InputStatus == 1);

	if (InputStatus == 1)
		send(DAT_CHAT, value, strlen(value) + 1);

	DeleteKeyInput(InputHandle);

	Stick(KEY_INPUT_ESCAPE);
	Stick(KEY_INPUT_RETURN);

	//ret = KeyInputString(10 + 22 * 9 + strlen(name)*9, SCREEN_Y - 20, 255, value, TRUE);

	if (InputStatus == 1)
		return 0;
	else
		return -1;
}

int input_roomname(void)
{
	int ret;
	char roomname[32] = "";

	ClearDrawScreen();

	SetKeyInputStringColor(STR_COLOR, STR_COLOR, STR_COLOR, STR_COLOR, STR_COLOR, STR_COLOR, STR_COLOR_2);
	DrawFormatString(10, SCREEN_Y - 20, STR_COLOR, "新しい部屋の名前 > ");

	ScreenFlip();

	ret = KeyInputString(10 + 9*19, SCREEN_Y - 20, 31, roomname, TRUE);

	Stick(KEY_INPUT_ESCAPE);
	Stick(KEY_INPUT_RETURN);

	if (ret == 1)
	{
		send(DAT_CREATE_ROOM, roomname, sizeof(char)*(strlen(roomname) + 1));

		return 0;
	}
	else			return -1;
}

void disp(void)
{
	SPLITDATA spldata;
	int spl_n;
	int i;
	int rot;
	static bool newchatflag = false;

	before_pos_y_min = pos_y_min;

	spl_n = split(Chat.content, "\n", &spldata);
	spl_n--;
	{
		rot = GetMouseWheelRotVol();
		pos_y += rot * 60;
		if (pos_y < pos_y_min)
		{
			pos_y = pos_y_min;
		}
		else if (pos_y > 0)
		{
			pos_y = 0;
		}
		if (pos_y == pos_y_min)
		{
			newchatflag = false;
		}
	}

	//画面に収まる行以上のチャットがあったら左上座標を下げる
	if (spl_n * 20 + 10 >= 460)
	{
		pos_y_min = -(spl_n * 20 + 10 - 460);
	}
	else pos_y = 0;

	//新しいメッセージが来たら
	if (before_pos_y_min != pos_y_min)
	{
		if (pos_y == before_pos_y_min || pos_y_update_flag)
		{
			pos_y_update_flag = false;
			pos_y = pos_y_min;
		}
		else
		{
			newchatflag = true;
		}
	}

	DrawBox(0, 0, SCREEN_X, SCREEN_Y, BACK_COLOR, true);

	for (i = 0; i < spl_n; i++)
	{
		if (strstr(spldata[i], " : ") == NULL && strstr(spldata[i], "参加") != NULL)
		{
			DrawBox(10, pos_y + 10 + 20 * i, SCREEN_X - 10, pos_y + 10 + 20 * (i + 1) - 2, BACK_JOIN, true);
			DrawFormatString(10, pos_y + 10 + 20 * i, STR_COLOR_2, "%s", spldata[i]);
		}
		else if (strstr(spldata[i], " : ") == NULL && strstr(spldata[i], "退出") != NULL)
		{
			DrawBox(10, pos_y + 10 + 20 * i, SCREEN_X - 10, pos_y + 10 + 20 * (i + 1) - 2, BACK_DIS, true);
			DrawFormatString(10, pos_y + 10 + 20 * i, STR_COLOR_2, "%s", spldata[i]);
		}
		else
		{
			DrawBox(10, pos_y + 10 + 20 * i, SCREEN_X - 10, pos_y + 10 + 20 * (i + 1) - 2, BACK_NORMAL, true);
			DrawFormatString(10, pos_y + 10 + 20 * i, STR_COLOR_2, "%s", spldata[i]);
		}
	}

	DrawBox(0, SCREEN_Y - 20, SCREEN_X, SCREEN_Y, BACK_COLOR, TRUE);
	if (newchatflag == true)
	{
		DrawBox(SCREEN_X - 160, 0, SCREEN_X, 20, BACK_COLOR, TRUE);
		DrawFormatString(SCREEN_X - 160, 0, STR_COLOR_2, "新しいメッセージ");
	}

	delsplit(spl_n, spldata);
}

void communicate(void)
{
	DATATYPE d_type;
	int len;
	char *d_s;

	GetArray1Dim(char, d_s, 0);

	while (GetNetWorkDataLength(NetHandle) >= sizeof(DATATYPE))
	{
		NetWorkRecv(NetHandle, &d_type, sizeof(DATATYPE));

		switch (d_type)
		{
		case DAT_JOINED:
		case DAT_DIS:
		case DAT_CHAT:
		case DAT_TAIKAI_ROOM:
			len = recv_str(d_type, &d_s);

			Chat.len += len;
			ChangeArraySize_1dim(char, Chat.content, Chat.len + 1);
			sprintf_s(Chat.content, Chat.len + 1, "%s%s", Chat.content, d_s);

			pos_y_update_flag = true;
			break;

		case DAT_ROOM_N:
			recv(d_type, NULL);
			break;
		}
	}

	DelArray1Dim(d_s);
}

/*
void send(DATATYPE d_type, void* data, size_t size)
{
	NetWorkSend(NetHandle, &d_type, sizeof(DATATYPE));
	NetWorkSend(NetHandle, data, size);
}

int recv_str(DATATYPE d_type, char** data)
{
	int cursor = 0;

	NetWorkRecv(NetHandle, &d_type, sizeof(DATATYPE));

	switch (d_type)
	{
	case DAT_CHAT:
	case DAT_JOINED:
	case DAT_DIS:
	case DAT_TAIKAI_ROOM:
		do
		{
			while (GetNetWorkDataLength(NetHandle) < sizeof(char));
			ChangeArraySize_1dim(char, *data, cursor + 1);
			NetWorkRecv(NetHandle, &(*data)[cursor], sizeof(char));
			if ((*data)[cursor] == '\0')
				break;
			cursor++;
		} while (1);

		break;
	}

	return cursor;
}
*/

void send(DATATYPE d_type, void *data, size_t size)
{
	NetWorkSend(NetHandle, &d_type, sizeof(DATATYPE));
	if (data != NULL)
		send_val(d_type, data, size);
}

int recv(DATATYPE d_type, void *data)
{
	switch (d_type)
	{
	case DAT_CHAT:
	case DAT_JOINED:
	case DAT_DIS:
	case DAT_TAIKAI_ROOM:
		return recv_str(d_type, (char**)data);
		break;
	case DAT_ROOM_N:
		return recv_roominfo();
		break;
	default:
		return -1;
		break;
	}
}

void send_val(DATATYPE d_type, void* data, size_t size)
{
	NetWorkSend(NetHandle, data, size);
}

int recv_str(DATATYPE d_type, char** data)
{
	int cursor = 0;

	do
	{
		while (GetNetWorkDataLength(NetHandle) < sizeof(char));
		ChangeArraySize_1dim(char, *data, cursor + 1);
		NetWorkRecv(NetHandle, &(*data)[cursor], sizeof(char));
		if ((*data)[cursor] == '\0')
			break;
		cursor++;
	} while (1);

	return cursor;
}

int recv_val(DATATYPE d_type, void* data)
{
	switch (d_type)
	{
	case DAT_ROOM_ISUSE:
		while (GetNetWorkDataLength(NetHandle) < sizeof(bool));
		NetWorkRecv(NetHandle, (bool*)data, sizeof(bool));
		return 0;
		break;
	default:
		while (GetNetWorkDataLength(NetHandle) < sizeof(int));
		NetWorkRecv(NetHandle, (int*)data, sizeof(int));
		return 0;
		break;
	}
}

int recv_roominfo(void)
{
	int cnt = 0;
	int i;
	DATATYPE d_type;
	
	for (i = 0; i < room_n; i++)
		DelArray1Dim(room[i].name);
	DelArray1Dim(room);

	recv_val(DAT_ROOM_N, &room_n);

	GetArray1Dim(ROOMS, room, 0);

	while (cnt < room_n)
	{
		if (GetNetWorkDataLength(NetHandle) >= sizeof(DATATYPE))
		{
			NetWorkRecv(NetHandle, &d_type, sizeof(DATATYPE));
			switch (d_type)
			{
				case DAT_ROOM_ADD:
					ChangeArraySize_1dim(ROOMS, room, cnt + 1);
					break;
				case DAT_ROOM_NAME:
					GetArray1Dim(char, room[cnt].name, 0);
					recv_str(d_type, &room[cnt].name);
					break;
				case DAT_ROOM_ISUSE:
					recv_val(d_type, &room[cnt].isuse);
					cnt++;
					break;
			}
		}
	}

	return 0;
}
