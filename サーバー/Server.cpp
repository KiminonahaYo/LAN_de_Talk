#include "DxLib.h"
#include <math.h>
#include <string.h>

#include "UsefulHeaders/useful.h"

#define SCREEN_X 640
#define SCREEN_Y 480
#define STR_COLOR 0xFFFFFF
#define STR_JOIN 0x9999ff
#define STR_DIS  0xff9999

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
	int NHandle;
	IPDATA Ip;
	bool flag;
	char *name;
	int room_no;
}
CLIENT;

typedef struct
{
	char *content;
	int len;
}
CHAT;

typedef struct
{
	char *content;
	int len;
	char name[256];
	int no;
}
MYLOG;

class ROOMS
{
private:
	CHAT Chat;
	MYLOG mylog;
	MYLOG clog;

	int no;
	bool flag;

public:
	char Name[32];

public:
	void init(int, char*);
	void update(void);
	void disp_chat(void);
	void disp_log(void);
	void dest(void);

	void join(int);
	void dis(int);
	void taikai(int, char*);

	char* GetName(void)
	{
		return Name;
	}

	bool isuse(void)
	{
		return flag;
	}

	ROOMS(void)
	{
		no = -1;
		flag = true;
	}
private:

	void Communicate(void);
	void addlog(const char *format, ...);

	/*
	void send_val(int, DATATYPE, void*, size_t);
	int recv_str(int, DATATYPE, char**);
	int recv_val(int, DATATYPE, void*);
	*/
};

class USERS
{
private:
	int no;

public:
	void init(void);
	void update(void);
	void dest(void);
	void disp(void);
	USERS(void)
	{
		no = -1;
	}

private:
	void CheckNewConnection(void);
	void CheckLostConnection(void);

	void Communicate(void);

	//void send_val(int, DATATYPE, void*, size_t);
	//int recv_str(int, DATATYPE, char**);
	//int recv_val(int, DATATYPE, void*);
};

//プロトタイプ宣言

USERS Users;

ROOMS *ChatRoom = new ROOMS[0];
int ChatRoom_n = 0;

CLIENT *Client;
int client_n;

int count_client(void);
void init(void);
void update(void);
void dest(void);
void disp(void);

/*
CHAT Chat;
MYLOG mylog;
MYLOG clog;
*/

/*
void CheckNewConnection(void);
void CheckLostConnection(void);

void Communicate_Client(void);

void my_send(int, DATATYPE, void*, size_t);
int my_recv(int, DATATYPE, void*);

void addlog(const char *format, ...);
*/

void send_val(int, DATATYPE, void*, size_t);
int recv_str(int, DATATYPE, char**);
int recv_val(int, DATATYPE, void*);

void send(int, DATATYPE, void*, size_t);
int recv(int, DATATYPE, void*);

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hP, LPSTR lpC, int nC)
{
	init();

	PreparationListenNetWork(9850);

	//ここから
	while(!ProcessMessage() && CheckHitKey(KEY_INPUT_ESCAPE)==0)
	{
		ClearDrawScreen();

		update();

		disp();

		/*
		CheckNewConnection();
		CheckLostConnection();

		Communicate_Client();
		*/

		ScreenFlip();
	}
	//ここまで

	StopListenNetWork();

	dest();
	return 0;		//正常終了
}

void init(void)
{
	//画面サイズとウィンドウモード設定
	SetGraphMode(640, 480, 32);
	ChangeWindowMode(true);										//ウィンドウモードで起動、コメントアウトでフルスクリーン
	SetDoubleStartValidFlag(true);
	SetWindowIconID(101);
	SetAlwaysRunFlag(true);
	SetOutApplicationLogValidFlag(false);

	//-------------------------------------------------------------------------------------------

	if (DxLib_Init() == -1) exit(-1);					//DXライブラリ初期化

	SetBackgroundColor(0, 0, 0);						//背景色
	SetDrawScreen(DX_SCREEN_BACK);						//裏画面に描画

	//DX ライブラリを初期化し、初期化が失敗したらプログラム終了
	if (DxLib_Init() == -1) exit(-1);

	//GetArray1Dim(int, Client.NHandle, 0);
	//GetArray1Dim(bool, Client.flag, 0);
	//GetArray1Dim(IPDATA, Client.Ip, 0);
	//GetArray1Dim(char*, Client.name, 0);

	Users.init();
}

void update(void)
{
	int i;
	Users.update();

	for (i = 0; i < ChatRoom_n; i++)
	{
		ChatRoom[i].update();
	}
}

void dest(void)
{
	int i;

	for (i = 0; i < ChatRoom_n; i++)
	{
		if (ChatRoom[i].isuse() == true)
			ChatRoom[i].dest();
	}

	delete ChatRoom;

	Users.dest();

	DxLib_End();	//DX ライブラリを終了
}

void disp(void)
{
	static int disproom = -1;
	static int dispkind = 0;

	//入力部
	if (StickMouse(MOUSE_INPUT_LEFT))
	{
		if (disproom == -1)
		{
			if (ChatRoom_n > 0)
				disproom++;
		}
		else
		{
			if (dispkind == 1)
			{
				do
				{
					if (disproom < ChatRoom_n - 1)
						disproom++;
					else
					{
						disproom = -1;
					}
				} while (ChatRoom[disproom].isuse() == false && disproom != -1);
			}
			dispkind = !dispkind;
		}
	}

	//表示部（関数呼び出し）
	if (disproom == -1)
	{
		Users.disp();
	}
	else
	{
		if (dispkind == 0)
			ChatRoom[disproom].disp_log();
		if (dispkind == 1)
			ChatRoom[disproom].disp_chat();
	}
}

void USERS::init(void)
{
	GetArray1Dim(CLIENT, Client, 0);
	client_n = 0;
}

void USERS::update(void)
{
	CheckNewConnection();
	CheckLostConnection();

	Communicate();
}

void USERS::dest(void)
{
	int i;

	for (i = 0; i < client_n; i++)
		DelArray1Dim(Client[i].name);
	DelArray1Dim(Client);
}

void USERS::disp(void)
{
	char UserList[4096];

	SPLITDATA spldata;
	int spl_n;
	int i;
	int num;

	static int pos_y_min = 0;
	static int before_pos_y_min;
	static int pos_y = pos_y_min;

	int rot;

	static bool newinfoflag = false;

	before_pos_y_min = pos_y_min;

	sprintf_s(UserList, 4096, "総合接続者一覧\n");
	num = 0;
	for (i = 0; i < client_n; i++)
	{
		if (Client[i].flag == true)
		{
			sprintf_s(UserList, 4096, "%s%02d, %s, IP : %3d.%3d.%3d.%3d, %dByte Buffer\n",
				UserList,
				num + 1,
				Client[i].name,
				Client[i].Ip.d1,
				Client[i].Ip.d2,
				Client[i].Ip.d3,
				Client[i].Ip.d4,
				GetNetWorkDataLength(Client[i].NHandle)
				);
			num++;
		}
	}

	spl_n = split(UserList, "\n", &spldata);

	{
		rot = GetMouseWheelRotVol();
		pos_y += rot * 10;
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
			newinfoflag = false;
		}
	}

	//画面に収まる行以上のチャットがあったら左上座標を下げる
	if (spl_n * 20 + 10 >= 460)
	{
		pos_y_min = -(spl_n * 20 + 10 - 460);
	}
	else pos_y_min = 0;

	/*
	if (islog != islog_before)
	{
		before_pos_y_min = pos_y_min;
		pos_y = pos_y_min;
	}
	*/

	//新しいメッセージが来たら
	if (before_pos_y_min != pos_y_min)
	{
		if (pos_y == before_pos_y_min)
		{
			pos_y = pos_y_min;
		}
		else
		{
			newinfoflag = true;
		}
	}

	for (i = 0; i < spl_n; i++)
	{
		DrawFormatString(10, pos_y + 10 + 20 * i, STR_COLOR, "%s", spldata[i]);
	}

	DrawBox(0, SCREEN_Y - 20, SCREEN_X, SCREEN_Y, 0x000000, TRUE);

	DrawFormatString(10, SCREEN_Y - 20, STR_COLOR, "[ユーザー情報] Client接続:%02d件、%d部屋、ホイールOK、左クリックで切替", count_client(), ChatRoom_n);

	if (newinfoflag == true)
	{
		DrawBox(SCREEN_X - 100, 0, SCREEN_X, 20, 0x000000, TRUE);
		DrawFormatString(SCREEN_X - 100, 0, STR_COLOR, "新しい情報");
	}

	delsplit(spl_n, spldata);
}

void ROOMS::init(int room_no, char *room_name)
{
	DATEDATA Date;
	FILE *fp;

	no = room_no;
	strcpy_s(Name, 32, room_name);

	GetArray1Dim(char, Chat.content, 1);
	Chat.content[0] = '\0';
	Chat.len = 0;

	GetArray1Dim(char, mylog.content, 1);
	mylog.content[0] = '\0';
	mylog.len = 0;
	mylog.no = 0;

	MakeDir("./Logs");
	MakeDir("./Logs/SLog");
	do
	{
		mylog.no++;
		sprintf_s(mylog.name, 256, "./Logs/SLog/MyLog%2d.txt", mylog.no);
	} while (exist("%s", mylog.name) == true);

	clsDx();

	fopen_s(&fp, mylog.name, "w");
	fclose(fp);

	GetDateTime(&Date);
	addlog("This file made on %4d/%02d/%02d %02d:%02d:%02d\n",
	Date.Year, Date.Mon, Date.Day,
	Date.Hour, Date.Min, Date.Sec);
	addlog("RoomName is : %s\n", Name);

	addlog("Start------------------------------------------------------------\n");
}

void ROOMS::update(void)
{
	if (flag == true)
	{
		Communicate();
	}
}

void ROOMS::dest(void)
{
	FILE *fp;

	MakeDir("./Logs");
	MakeDir("./Logs/CLog");

	clog = mylog;
	clog.content = Chat.content;
	clog.len = Chat.len;

	sprintf_s(clog.name, 256, "./Logs/CLog/ChatLog%2d.txt", clog.no);

	clsDx();

	fopen_s(&fp, clog.name, "w");
	if (fp != NULL)
	{
		fprintf(fp, "%s", clog.content);
		fclose(fp);
	}

	DelArray1Dim(Chat.content);
	DelArray1Dim(mylog.content);

	flag = false;
}

void ROOMS::disp_chat(void)
{
	SPLITDATA spldata;
	int spl_n;
	int i;
	static int pos_y_min = 0;
	static int before_pos_y_min;
	static int pos_y = pos_y_min;
	int rot;
	static bool newchatflag = false;

	before_pos_y_min = pos_y_min;

	spl_n = split(Chat.content, "\n", &spldata);

	{
		rot = GetMouseWheelRotVol();
		pos_y += rot * 10;
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
	else pos_y_min = 0;

	//新しいメッセージが来たら
	if (before_pos_y_min != pos_y_min)
	{
		if (pos_y == before_pos_y_min)
		{
			pos_y = pos_y_min;
		}
		else
		{
			newchatflag = true;
		}
	}

	for (i = 0; i < spl_n; i++)
	{
		if (strstr(spldata[i], " : ") == NULL && strstr(spldata[i], "参加") != NULL)
			DrawFormatString(10, pos_y + 10 + 20 * i, STR_JOIN, "%s", spldata[i]);
		else if (strstr(spldata[i], " : ") == NULL && strstr(spldata[i], "退出") != NULL)
			DrawFormatString(10, pos_y + 10 + 20 * i, STR_DIS, "%s", spldata[i]);
		else
			DrawFormatString(10, pos_y + 10 + 20 * i, STR_COLOR, "%s", spldata[i]);
	}

	DrawBox(0, SCREEN_Y - 20, SCREEN_X, SCREEN_Y, 0x000000, TRUE);
	DrawFormatString(10, SCREEN_Y - 20, STR_COLOR, "[チャット内容] Client接続:%02d件、ホイールOK、左クリック切替", count_client());

	if (newchatflag == true)
	{
		DrawBox(SCREEN_X - 100, 0, SCREEN_X, 20, 0x000000, TRUE);
		DrawFormatString(SCREEN_X - 100, 0, STR_COLOR, "新しいログ");
	}

	delsplit(spl_n, spldata);
}

void ROOMS::disp_log(void)
{
	SPLITDATA spldata;
	int spl_n;
	int i;
	static int pos_y_min = 0;
	static int before_pos_y_min;
	static int pos_y = pos_y_min;
	int rot;
	static bool newchatflag = false;

	before_pos_y_min = pos_y_min;

	spl_n = split(mylog.content, "\n", &spldata);

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
	else pos_y_min = 0;

	//新しいメッセージが来たら
	if (before_pos_y_min != pos_y_min)
	{
		if (pos_y == before_pos_y_min)
		{
			pos_y = pos_y_min;
		}
		else
		{
			newchatflag = true;
		}
	}

	for (i = 0; i < spl_n; i++)
	{
		DrawFormatString(10, pos_y + 10 + 20 * i, STR_COLOR, "%s", spldata[i]);
	}

	DrawBox(0, SCREEN_Y - 20, SCREEN_X, SCREEN_Y, 0x000000, TRUE);
	DrawFormatString(10, SCREEN_Y - 20, STR_COLOR, "[サーバーログ] Client接続:%02d件、ホイールOK、左クリック切替", count_client());

	if (newchatflag == true)
	{
		DrawBox(SCREEN_X - 100, 0, SCREEN_X, 20, 0x000000, TRUE);
		DrawFormatString(SCREEN_X - 100, 0, STR_COLOR, "新しいログ");
	}

	delsplit(spl_n, spldata);
}

void ROOMS::join(int userno)
{
	int i;

	char joinmessage[256];
	int len;

	send(Client[userno].NHandle, DAT_CHAT, Chat.content, Chat.len + 1);

	//参加メッセージを送る
	sprintf_s(joinmessage, 256, "%sさんが参加しました。\n", Client[userno].name);
	len = strlen(joinmessage);

	//全体のチャット記録に追加する
	Chat.len += len;
	ChangeArraySize_1dim(char, Chat.content, Chat.len + 1);
	sprintf_s(Chat.content, Chat.len + 1, "%s%s", Chat.content, joinmessage);

	//ログを残す
	addlog("IP=%3d.%3d.%3d.%3d %sさんが参加しました。\n",
		Client[userno].Ip.d1, Client[userno].Ip.d2, Client[userno].Ip.d3, Client[userno].Ip.d4,
		Client[userno].name);

	//同じ部屋のメンバー全員に送る
	for (i = 0; i < client_n; i++)
	{
		if (Client[i].flag == true && Client[i].room_no == Client[userno].room_no)
		{
			send(Client[i].NHandle, DAT_JOINED, joinmessage, sizeof(char)*(len + 1));
			addlog("Sent %3d.%3d.%3d.%3d Name=%s\n",
				Client[i].Ip.d1, Client[i].Ip.d2, Client[i].Ip.d3, Client[i].Ip.d4,
				Client[i].name);
		}
	}

	Client[userno].room_no = no;
}

void ROOMS::dis(int userno)
{
	int i;

	char dismessage[256];
	int len;

	sprintf_s(dismessage, 256, "%sさんが退出しました。\n", Client[userno].name);
	len = strlen(dismessage);

	Chat.len += len;
	ChangeArraySize_1dim(char, Chat.content, Chat.len + 1);
	sprintf_s(Chat.content, Chat.len + 1, "%s%s", Chat.content, dismessage);

	addlog("IP=%3d.%3d.%3d.%3d %sさんが退出しました。\n",
		Client[userno].Ip.d1, Client[userno].Ip.d2, Client[userno].Ip.d3, Client[userno].Ip.d4,
		Client[userno].name);

	//同じ部屋のメンバー全員に送る
	for (i = 0; i < client_n; i++)
	{
		if (Client[i].flag == true && Client[i].room_no == Client[userno].room_no && i != userno)
		{
			send(Client[i].NHandle, DAT_DIS, dismessage, sizeof(char)*(len + 1));
			addlog("Sent %3d.%3d.%3d.%3d Name=%s\n",
				Client[i].Ip.d1, Client[i].Ip.d2, Client[i].Ip.d3, Client[i].Ip.d4,
				Client[i].name);
		}
	}

	Client[userno].room_no = -1;
}

void ROOMS::taikai(int userno, char *reason)
{
	send(Client[userno].NHandle, DAT_TAIKAI_ROOM, reason, sizeof(reason) + 1);

	//全体のチャット記録に追加する
	Chat.len += sizeof(reason);
	ChangeArraySize_1dim(char, Chat.content, Chat.len + 1);
	sprintf_s(Chat.content, Chat.len + 1, "%s%s", Chat.content, reason);

	//ログを残す
	addlog("%s\n", reason);
}

void USERS::CheckNewConnection(void)
{
	int NetHandle_Temp;
	int cursor = 0;
	bool isuse;
	char name[32];

	int i;

	NetHandle_Temp = GetNewAcceptNetWork();

	if (NetHandle_Temp != -1)
	{
		ChangeArraySize_1dim(CLIENT, Client, client_n + 1);

		GetArray1Dim(char, Client[client_n].name, 0);
		Client[client_n].flag = true;
		Client[client_n].NHandle = NetHandle_Temp;
		GetNetWorkIP(Client[client_n].NHandle, &Client[client_n].Ip);
		ChangeArraySize_1dim(char, Client[client_n].name, 1);
		Client[client_n].name[0] = '\0';
		Client[client_n].room_no = -1;		//ダミーの値

		//send(Client[client_n].NHandle, DAT_CHAT, Chat.content, sizeof(char)*(Chat.len + 1));

		send(Client[client_n].NHandle, DAT_ROOM_N, &ChatRoom_n, sizeof(int));
		for (i = 0; i < ChatRoom_n; i++)
		{
			isuse = ChatRoom[i].isuse();
			strcpy_s(name, 32, ChatRoom[i].GetName());
			send(Client[client_n].NHandle, DAT_ROOM_ADD, NULL, 0);
			send(Client[client_n].NHandle, DAT_ROOM_NAME, name, strlen(name) + 1);
			send(Client[client_n].NHandle, DAT_ROOM_ISUSE, &isuse, sizeof(bool));
		}

		client_n++;
	}
}

void USERS::CheckLostConnection(void)
{
	int NetHandle_Temp;

	int i;

	NetHandle_Temp = GetLostNetWork();

	for (i = 0; i < client_n; i++)
	{
		if (Client[i].NHandle == NetHandle_Temp)
		{
			if (Client[i].flag == true)
			{
				Client[i].flag = false;

				if (Client[i].room_no != -1)
				{
					ChatRoom[Client[i].room_no].dis(i);
					break;
				}
			}
		}
	}
}

int count_client(void)
{
	int i, cnt = 0;
	for (i = 0; i < client_n; i++)
	{
		if (Client[i].flag == true)
			cnt++;
	}
	return cnt;
}

void USERS::Communicate(void)
{
	int i, j;
	int leng;

	DATATYPE d_type;
	char *d_s;
	int d_s_len;

	bool isuse;
	char name[32];

	static int callcnt = 0;

	GetArray1Dim(char, d_s, 0);

	for (i = 0; i < client_n; i++)
	{
		if (Client[i].flag == true && Client[i].room_no == no)
		{
			//送信（定期）
			if (callcnt % 5 == 0)
			{
				send(Client[i].NHandle, DAT_ROOM_N, &ChatRoom_n, sizeof(int));
				for (j = 0; j < ChatRoom_n; j++)
				{
					isuse = ChatRoom[j].isuse();
					strcpy_s(name, 32, ChatRoom[j].GetName());
					send(Client[i].NHandle, DAT_ROOM_ADD, NULL, 0);
					send(Client[i].NHandle, DAT_ROOM_NAME, name, strlen(name) + 1);
					send(Client[i].NHandle, DAT_ROOM_ISUSE, &isuse, sizeof(bool));
				}
			}
			//受信対応
			//受信サイズを調べる
			leng = GetNetWorkDataLength(Client[i].NHandle);

			//受信があったら
			while ((leng = GetNetWorkDataLength(Client[i].NHandle)) >= sizeof(DATATYPE) && Client[i].room_no == no && Client[i].flag == true)
			{
				//データタイプを取得する
				NetWorkRecv(Client[i].NHandle, &d_type, sizeof(DATATYPE));
				leng = GetNetWorkDataLength(Client[i].NHandle);

				//データタイプに応じた処理
				switch (d_type)
				{
				case DAT_NAME:
					//受信
					d_s_len = recv(Client[i].NHandle, d_type, &d_s);

					//名前を登録する
					ChangeArraySize_1dim(char, Client[i].name, d_s_len + 1);
					sprintf_s(Client[i].name, d_s_len + 1, "%s", d_s);

					break;

				case DAT_JOIN_ROOM:
					recv(Client[i].NHandle, d_type, &Client[i].room_no);
					ChatRoom[Client[i].room_no].join(i);
					break;

				case DAT_CREATE_ROOM:
					d_s_len = recv(Client[i].NHandle, d_type, &d_s);
					{
						ROOMS *temp;
						temp = new ROOMS[ChatRoom_n + 1];
						for (j = 0; j < ChatRoom_n; j++)
						{
							temp[j] = ChatRoom[j];
						}
						delete ChatRoom;
						ChatRoom = temp;
					}
					ChatRoom[ChatRoom_n].init(ChatRoom_n, d_s);
					ChatRoom_n++;
					break;

				case DAT_DEST_ROOM:
					ChatRoom[Client[i].room_no].dest();
					for (j = 0; j < client_n; j++)
					{
						if (Client[i].room_no == Client[j].room_no && Client[j].flag == true)
						{
							ChatRoom[Client[i].room_no].taikai(j, "部屋が管理者によって破棄されました。");
						}
						ChatRoom[Client[i].room_no].dis(i);
					}
					//Client[i].room_no = -1;
					break;

				}	//end switch

				ChangeArraySize_1dim(char, d_s, 0);
				leng = GetNetWorkDataLength(Client[i].NHandle);
			}
		}
	}
	DelArray1Dim(d_s);

	callcnt++;
}

void ROOMS::Communicate(void)
{
	int i, j;
	int leng;

	DATATYPE d_type;
	char *d_s;
	int d_s_len;

	char *a_content;
	int content_len;

	GetArray1Dim(char, d_s, 0);
	GetArray1Dim(char, a_content, 0);

	for (i = 0; i < client_n; i++)
	{
		if (Client[i].flag == true && Client[i].room_no == no)
		{
			//受信サイズを調べる
			leng = GetNetWorkDataLength(Client[i].NHandle);

			//受信があったら
			while ((leng = GetNetWorkDataLength(Client[i].NHandle)) >= sizeof(DATATYPE) && Client[i].room_no == no && Client[i].flag == true)
			{
				//データタイプを取得する
				NetWorkRecv(Client[i].NHandle, &d_type, sizeof(DATATYPE));
				leng = GetNetWorkDataLength(Client[i].NHandle);

				//データタイプに応じた処理
				switch (d_type)
				{
				case DAT_CHAT:
					//名前が登録されていたら
					if (strcmp(Client[i].name, "") != 0)
					{
						//受信
						d_s_len = recv(Client[i].NHandle, d_type, &d_s);

						//チャット内容に送信者名を追加する
						content_len = d_s_len + strlen(Client[i].name) + 3 + 1;
						ChangeArraySize_1dim(char, a_content, content_len + 1);
						sprintf_s(a_content, content_len + 1, "%s : %s\n", Client[i].name, d_s);

						//全体のチャット記録に追加する
						Chat.len += content_len;
						ChangeArraySize_1dim(char, Chat.content, Chat.len + 1);
						sprintf_s(Chat.content, Chat.len + 1, "%s%s", Chat.content, a_content);

						//ログを残す
						addlog("IP=%3d.%3d.%3d.%3d %s : %s\n",
							Client[i].Ip.d1, Client[i].Ip.d2, Client[i].Ip.d3, Client[i].Ip.d4,
							Client[i].name, d_s);

						//同じ部屋のメンバー全員に送る
						for (j = 0; j < client_n; j++)
						{
							if (Client[j].flag == true && Client[j].room_no == no)
							{
								send(Client[j].NHandle, DAT_CHAT, a_content, sizeof(char)*(content_len + 1));
								addlog("Sent %3d.%3d.%3d.%3d Name=%s\n",
									Client[j].Ip.d1, Client[j].Ip.d2, Client[j].Ip.d3, Client[j].Ip.d4,
									Client[j].name);
							}
						}
					}
					break;

				case DAT_DIS_ROOM:
					ChatRoom[Client[i].room_no].dis(i);
					break;
				}	//end switch
				leng = GetNetWorkDataLength(Client[i].NHandle);
				ChangeArraySize_1dim(char, d_s, 0);
			}
		}
	}
	DelArray1Dim(d_s);
	DelArray1Dim(a_content);
}

void ROOMS::addlog(const char *format, ...)
{
	va_list list;
	char buf[1024];
	FILE *fp;

	MakeDir("./Logs");
	MakeDir("./Logs/SLog");

	va_start(list, format);
	vsprintf_s(buf, 1024, format, list);
	va_end(list);

	fopen_s(&fp, mylog.name, "a");
	if (fp != NULL)
	{
		fprintf(fp, "%s", buf);
		fclose(fp);
	}

	//printfDx("%s", buf);
	mylog.len += strlen(buf);
	ChangeArraySize_1dim(char, mylog.content, mylog.len + 1);
	sprintf_s(mylog.content, mylog.len + 1, "%s%s", mylog.content, buf);
}

void send(int NetHandle, DATATYPE d_type, void *data, size_t size)
{
	NetWorkSend(NetHandle, &d_type, sizeof(DATATYPE));
	if (data != NULL)
		send_val(NetHandle, d_type, data, size);
}

int recv(int NetHandle, DATATYPE d_type, void *data)
{
	switch (d_type)
	{
	case DAT_CHAT:
	case DAT_NAME:
	case DAT_CREATE_ROOM:
		return recv_str(NetHandle, d_type, (char**)data);
		break;
	case DAT_JOIN_ROOM:
		return recv_val(NetHandle, d_type, data);
		break;
	default:
		return -1;
	}
}

void send_val(int NetHandle, DATATYPE d_type, void* data, size_t size)
{
	NetWorkSend(NetHandle, data, size);
}

int recv_str(int NetHandle, DATATYPE d_type, char** data)
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

int recv_val(int NetHandle, DATATYPE d_type, void* data)
{
	switch (d_type)
	{
	case DAT_JOIN_ROOM:
		while(GetNetWorkDataLength(NetHandle) < sizeof(int));
		NetWorkRecv(NetHandle, (int*)data, sizeof(int));
		return 0;
		break;
	default:
		return -1;
		break;
	}
}
