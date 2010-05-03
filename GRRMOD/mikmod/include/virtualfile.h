#ifndef _VF_INCLUDED
#define _VF_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
	char *buffer;
	uint32_t size;
	uint32_t pos;
} VIRTUAL_FILE;

#ifdef __cplusplus
}
#endif

#endif

