#ifndef __MYTYPE_H__
#define __MYTYPE_H__
#include "wtypes.h"
/*multi platform*/
typedef unsigned char		T_U8;
typedef unsigned short		T_U16;
typedef unsigned int		T_U32;
typedef unsigned long		T_ULONG;
typedef unsigned long long	T_U64;

typedef char				T_S8;
typedef short				T_S16;
typedef int					T_S32;
typedef long long			T_S64;

typedef bool				T_BOOL;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define IN
#define OUT
#define IN_OUT

#endif