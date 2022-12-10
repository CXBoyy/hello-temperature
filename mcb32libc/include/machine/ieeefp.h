#ifndef _IEEEFP_H_
#define _IEEEFP_H_

#include <stdint.h>

#define	_EXFUN(name, proto)		name proto
#define	_DEFUN(name, arglist, args)	name(args)

#define __IEEE_LITTLE_ENDIAN
#define _LDBL_EQ_DBL

#ifndef __uint32_t
#define __uint32_t uint32_t
#endif

#ifndef __int32_t
#define __int32_t uint32_t
#endif

#endif
