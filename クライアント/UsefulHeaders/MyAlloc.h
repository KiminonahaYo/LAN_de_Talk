#ifndef MYALLOC_DEFINED
#define MYALLOC_DEFINED

#include <malloc.h>

#define GetArray1Dim(type, dimarray, d1)					{dimarray = (type*)malloc(sizeof(type)*(d1));}
#define ChangeArraySize_1dim(type, dimarray, d1)			{dimarray = (type*)realloc(dimarray, sizeof(type)*(d1));}
#define DelArray1Dim(dimarray)								{free(dimarray);}

#define GetArray2Dim(type, dim2array, d1, d2)				{ int i; dim2array = (type **)malloc(sizeof(type*)*d1); for(i=0; i<d1; i++) dim2array[i] = (type *)malloc(sizeof(type)*d2); }
#define ChangeArraySize_2Dim(type, dim2array, bd1, d1, d2)	{ int i; for(i=0; i<d1; i++) dim2array[i] = (type *)realloc(dim2array[i], sizeof(type)*d2); if(bd1>d1) for(i=d1; i<bd1; i++) free(dim2array[i]); dim2array = (type **)realloc(dim2array, sizeof(type*)*d1);   }
#define DelArray2Dim(dim2array, d1)							{ int i; for(i=0; i<d1; i++) free(dim2array[i]); free(dim2array); }

#endif