#ifndef __STD_LONG_H__
#define __STD_LONG_H__
#include <inttypes.h>


struct SStdULong
{
	uint32_t	uiLow;
	uint32_t	uiHigh;
};

struct SStdLong
{
	uint32_t	uiLow;
	int32_t		uiHigh;
};

typedef struct SStdULong uint64;
typedef struct SStdLong  int64;


#endif // __STD_LONG_H__

