#ifndef _CIFM_H_
#define _CIFM_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE* ci_fopen(const char* name, const char* mode);
const char* CI_FixName(const char* name);

#ifdef __cplusplus
};
#endif

#endif