#ifndef LS_DBG_H_
#define LS_DBG_H_
//#define NDEBUG

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#ifdef NDEBUG
#define LS_ASSERT(e) 
#define LS_RAM_ASSERT(e) 
#else
#define LS_ASSERT(e) ((e)? (void)0 : ls_assert(#e,__FILE__,__LINE__))
#define LS_RAM_ASSERT(e) ((e)? (void)0 : ls_ram_assert())
#endif

void ls_assert(const char *expr,const char *file,int line);

void ls_ram_assert(void);

#endif /* LS_DBG_H_ */
