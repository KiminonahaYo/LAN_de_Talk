//DxLib専用

#ifndef KEYCONTROL_DEFINED

#include "MyMath.h"

bool Stick(int, bool, int);			//キーが押された瞬間に操作を受け付ける
bool StickMouse(int, bool, int);	//マウスがクリックされた瞬間に操作を受け付ける

bool GetKey(int);
int GetMouse(int);					//マウスクリック状態調査

bool GetKey_any_push(void);			//何かが押されている間を検知する
bool Stick_any_push(void);			//何かが押された瞬間を検知する。

int Key_PressedCount(int);			//押した、離した瞬間に離していた/押していた時間がどれくらいか返す関数
int Mouse_PressedCount(int);		//クリックした、離した瞬間に離していた/クリックしていた時間がどれくらいか返す関数

int m_inputnum(void);				//数字のみを入力する。
char m_inputalpha(void);			//英数字のみを入力する。

//キー入力関数

//何かが押されている間を検知する
bool GetKey_any_push(void)
{
	//キー入力情報を静的確保
	static char PushedKey[256];
	static int  PushCount[256];
	bool ret;

	int i;

	GetHitKeyStateAll(PushedKey);

	for (i = 0; i < 256; i++)
	{
		if (PushedKey[i] != 0) break;
	}
	if (i < 256)	ret = true;
	else			ret = false;

	return ret;
}

//何かが押された瞬間を検知する
bool Stick_any_push(void)
{
	bool pushedkey = GetKey_any_push();
	static bool pushedflag = true;
	bool ret;

	if (pushedkey == true)
	{
		if (pushedflag == false)
		{
			pushedflag = true;
			ret = true;
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		pushedflag = false;
		ret = false;
	}

	return ret;
}

//キーを押した瞬間に操作を受け付ける
bool Stick(int KeyCode, bool ForSelectStickFlag = 0, int Select_kirikae_kankaku = 3)
{
	//キー入力情報を静的確保
	static char PushedKey[256];
	static int  PushCount[256];
	bool ret;

	//キーが押されたら
	if (GetKey(KeyCode))
	{
		PushCount[KeyCode]++;

		//キーを押した瞬間または押しっぱなしで操作できるモードのとき
		if (PushedKey[KeyCode] == 0 || (ForSelectStickFlag && (PushCount[KeyCode] == 1 || PushCount[KeyCode] >= 30)))
		{
			//キーを押したかどうかのフラグを立てる
			PushedKey[KeyCode] = 1;

			//押しっぱなしで操作できるモードのとき
			if (ForSelectStickFlag && PushCount[KeyCode] >= 30)
			{
				if (PushCount[KeyCode] % Select_kirikae_kankaku == 0)
					ret = true;
				else
					ret = false;
			}
			//押された瞬間のとき
			else
			{
				ret = true;
			}
		}
		//押した瞬間以外のとき
		else
		{
			ret = false;
		}
	}
	//キーが押されていなかったら
	else
	{
		PushCount[KeyCode] = 0;
		PushedKey[KeyCode] = 0;
		ret = false;
		//return false;
	}
	return ret;
}

bool GetKey(int KeyCode)
{
	bool ret = CheckHitKey(KeyCode)!= 0 ? true : false;

	return ret;
}

//キーを押して離された/離されて押された瞬間に押した/離した時間を返す関数
int Key_PressedCount(int KeyCode)
{
	//キー入力情報を静的確保
	static bool PushFlag[256];
	static int  PushCount[256];
	int ret;

	//押されたら
	if (GetKey(KeyCode))
	{
		//押された瞬間、離された時間（マイナス）を返す
		if (PushFlag[KeyCode] == false)
		{
			ret = PushCount[KeyCode];
			PushFlag[KeyCode] = true;
			PushCount[KeyCode] = 0;
		}
		//それ以外はダミーを返してプラスカウント
		else
		{
			ret = 0;
			PushFlag[KeyCode] = true;
			PushCount[KeyCode]++;
		}
	}
	//離されたら
	else
	{
		//離された瞬間、押した時間（プラス）を返す
		if (PushFlag[KeyCode] == true)
		{
			ret = PushCount[KeyCode];
			PushFlag[KeyCode] = false;
			PushCount[KeyCode] = 0;
		}
		//それ以外はダミーを返してマイナスカウント
		else
		{
			ret = 0;
			PushFlag[KeyCode] = false;
			PushCount[KeyCode]--;
		}
	}
	return ret;
}

//マウスをクリックして離した/離してクリックした瞬間にクリックした/離した時間を返す関数
int Mouse_PressedCount(int KeyCode)
{
	//キー入力情報を静的確保
	static bool PushFlag[8];
	static int  PushCount[8];
	int ret;

	//マウスのキーコードが2の乗数になっているため、log関数を使う
	int Key = (int)(log((double)KeyCode) / log(2.0));

	//クリックされたら
	if (GetMouse(KeyCode))
	{
		//クリックされた瞬間、離された時間（マイナス）を返す
		if (PushFlag[Key] == false)
		{
			ret = PushCount[Key];
			PushFlag[Key] = true;
			PushCount[Key] = 0;
		}
		//それ以外はダミーを返してプラスカウント
		else
		{
			ret = 0;
			PushFlag[Key] = true;
			PushCount[Key]++;
		}
	}
	//離されたら
	else
	{
		//離された瞬間、クリックした時間（プラス）を返す
		if (PushFlag[Key] == true)
		{
			ret = PushCount[Key];
			PushFlag[Key] = false;
			PushCount[Key] = 0;
		}
		//それ以外はダミーを返してマイナスカウント
		else
		{
			ret = 0;
			PushFlag[Key] = false;
			PushCount[Key]--;
		}
	}
	return ret;
}

//マウスをクリックした瞬間に操作を受け付ける
bool StickMouse(int KeyCode, bool ForSelectStickFlag = 0, int Select_kirikae_kankaku = 3)
{
	static char PushedKey[8];
	static int  PushCount[8];
	bool ret;

	//マウスのキーコードが2の乗数になっているため、log関数を使う
	int Key = (int)(log((double)KeyCode) / log(2.0));

	//マウスがクリックされているとき
	if (GetMouse(KeyCode))
	{
		PushCount[Key]++;

		//マウスがクリックされた瞬間またはマウスをクリックしたままでも操作できるモードのとき
		if (PushedKey[Key] == 0 || (ForSelectStickFlag && (PushCount[Key] == 1 || PushCount[Key] >= 30)))
		{
			PushedKey[Key] = 1;

			//マウスをクリックしたままでも操作できるモードのとき
			if (ForSelectStickFlag && PushCount[Key] >= 30)
			{
				if (PushCount[Key] % Select_kirikae_kankaku == 0)
					ret = true;
				else
					ret = false;
			}
			//マウスをクリックされた瞬間のとき
			else
			{
				ret = true;
			}
		}
		//マウスをクリックされた瞬間以外のとき
		else
		{
			ret = false;
		}
	}
	//マウスがクリックされていないとき
	else
	{
		PushCount[Key] = 0;
		PushedKey[Key] = 0;
		ret = false;
	}

	return ret;
}

//マウスのクリック状態を調べる関数
int GetMouse(int KeyCode)
{
	int InputState = GetMouseInput();
	int KeyFlag = InputState & KeyCode;

	return KeyFlag;
}

//文字入力関数
//数字のみを入力する
int m_inputnum(void)
{
	char c = GetInputChar(true);

	if(c>='0' && c<='9')
	{
		return c - '0';
	}
	else if(c == CTRL_CODE_BS)
	{
		return 10;
	}
	else
	{
		while (GetInputChar(true) != 0);
		return -1;
	}
}

//英数字のみを入力する
char m_inputalpha(void)
{
	char c = GetInputChar(true);

	if ((c >= 'A' && c <= 'Z') ||(c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == CTRL_CODE_BS)
	{
		return c;
	}
	else
	{
		while (GetInputChar(true) != 0);
		return -1;
	}
}

#define KEYCONTROL_DEFINED
#endif
