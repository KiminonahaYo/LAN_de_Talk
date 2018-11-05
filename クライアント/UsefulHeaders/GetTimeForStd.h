#ifndef MYTIMEFORSTD_DEFINED

#include <time.h>
#include <stdio.h>
#include <string.h>

//ŽžŠÔŽæ“¾‚Ì‚½‚ß‚Ì\‘¢‘Ì
typedef struct
{
	int Year, Mon, Day;
	int Hour, Min, Sec;
	int Week;
}
DateAndTime;

//ƒvƒƒgƒ^ƒCƒvéŒ¾
void GetTime(DateAndTime *);
char *GetWeek(int, char *);

int  Get12Hour(int);
char *GetAMorPM(int);

char *GetJapaneseYear(int, char *);

//ŽžŠÔŽæ“¾‚ð‚µ‚â‚·‚­‚·‚éŠÖ”
void GetTime(DateAndTime *Date)
{
	time_t timer;
	struct tm local;

	timer = time(NULL);
	localtime_s(&local, &timer);

	Date->Year = local.tm_year + 1900;
	Date->Mon = local.tm_mon + 1;
	Date->Day = local.tm_mday;
	Date->Hour = local.tm_hour;
	Date->Min = local.tm_min;
	Date->Sec = local.tm_sec;
	Date->Week = local.tm_wday;

	return;
}

//GetTime()ŠÖ”‚Å“¾‚½—j“ú‚Ìƒtƒ‰ƒO‚ð—^‚¦‚Ä•¶Žš—ñ‚Å•Ô‚·ŠÖ”
//JS_S:“ú–{ŒêƒVƒ‡[ƒgA	JS:“ú–{Œê
//US_S:‰pŒêƒVƒ‡[ƒgA	US:‰pŒê
char *GetWeek(int WeekDayFlag, char *Mode)
{
	if (strcmp(Mode, "JS_S") == 0)
	{
		switch (WeekDayFlag)
		{
		case 0: return "“ú"; break;
		case 1: return "ŒŽ"; break;
		case 2: return "‰Î"; break;
		case 3: return "…"; break;
		case 4: return "–Ø"; break;
		case 5: return "‹à"; break;
		case 6: return "“y"; break;
		default: return ""; break;
		}
	}
	else if (strcmp(Mode, "JS") == 0)
	{
		switch (WeekDayFlag)
		{
		case 0: return "“ú—j“ú"; break;
		case 1: return "ŒŽ—j“ú"; break;
		case 2: return "‰Î—j“ú"; break;
		case 3: return "…—j“ú"; break;
		case 4: return "–Ø—j“ú"; break;
		case 5: return "‹à—j“ú"; break;
		case 6: return "“y—j“ú"; break;
		default: return ""; break;
		}
	}
	else if (strcmp(Mode, "US_S") == 0)
	{
		switch (WeekDayFlag)
		{
		case 0: return "Sun"; break;
		case 1: return "Mon"; break;
		case 2: return "Tue"; break;
		case 3: return "Wed"; break;
		case 4: return "Thu"; break;
		case 5: return "Fri"; break;
		case 6: return "Sat"; break;
		default: return ""; break;
		}
	}
	else if (strcmp(Mode, "US") == 0)
	{
		switch (WeekDayFlag)
		{
		case 0: return "Sunday"; break;
		case 1: return "Monday"; break;
		case 2: return "Tuesday"; break;
		case 3: return "Wednesday"; break;
		case 4: return "Thursday"; break;
		case 5: return "Friday"; break;
		case 6: return "Saturday"; break;
		default: return ""; break;
		}
	}
	else
	{
		return "";
	}
}

//12ŽžŠÔ•\Ž¦‚É‚·‚é
int Get12Hour(int Hour)
{
	return Hour % 12;
}

//AM-PM‚ðŽæ“¾‚·‚é
char *GetAMorPM(int Hour)
{
	return (Hour / 12 == 0 ? "AM" : (Hour / 12 == 1 ? "PM" : ""));
}

//”N†‚ðŽæ“¾‚·‚é
char *GetJapaneseYear(int Year, char *Mode)
{
	static char Ret[64];
	char *Nengo;
	int JYear;

	if (Year >= 1900)
	{
		if (strcmp(Mode, "A") == 0)
		{
			if (Year < 1912)		Nengo = "M";
			else if (Year < 1926)	Nengo = "T";
			else if (Year < 1989)	Nengo = "S";
			else				Nengo = "H";
		}
		else if (strcmp(Mode, "K") == 0)
		{
			if (Year < 1912)		Nengo = "–¾Ž¡";
			else if (Year < 1926)	Nengo = "‘å³";
			else if (Year < 1989)	Nengo = "º˜a";
			else				Nengo = "•½¬";
		}
		else return "";

		if (Year < 1912)		JYear = Year - 1868 + 1;
		else if (Year < 1926)	JYear = Year - 1912 + 1;
		else if (Year < 1989)	JYear = Year - 1926 + 1;
		else				JYear = Year - 1989 + 1;

		sprintf_s(Ret, 64, "%s%d", Nengo, JYear);

		return Ret;
	}
	else
	{
		return "";
	}
}

#define MYTIMEFORSTD_DEFINED
#endif
