//DxLib専用

#ifndef MYCOLOR_DEFINED

#include <math.h>
#include "MyMath.h"

int GetHSVColor(int, int, int);

//HSVの色を、RGB指定で使えるようにした
int GetHSVColor(int h, int s, int v)
{
	double f;
	int i, p, q, t;
	int r, g, b;

	i = int(floor(h / 60.0)) % 6;
	f = double((h / 60.0)) - double(floor(h / 60.0));
	p = int(round(v * (1.0 - (s / 255.0))));
	q = int(round(v * (1.0 - (s / 255.0)*f)));
	t = int(round(v * (1.0 - (s / 255.0)*(1.0 - f))));

	switch (i){
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	}

	return GetColor(r, g, b);
}

#define MYCOLOR_DEFINED
#endif
