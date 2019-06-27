#ifndef _GCVT_H_
#define _GCVT_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

char * gcvt(double f, uint32_t ndigit, char * buf);

#ifdef __cplusplus
}
#endif

#endif