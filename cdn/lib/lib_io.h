#ifndef __LIB_IO_H__
#define __LIB_IO_H__

#define MAX_IN_NUM    (2000)
#define MAX_OUT_NUM    (2000)
#define MAX_LINE_LEN    (2000)
#define MAX_EDGE_NUM    (2000 * 20)
typedef struct opc_parameter{
	int lineNum;
	char** lines; 
} opc_parameter_t;

extern int read_file(char ** const buff, const unsigned int spec, const char * const filename);


extern void write_result(const char * const buff,const char * const filename);


extern void release_buff(char ** const buff, const int valid_item_num);
#ifdef _MSC_VER
__pragma (optimize("g", on))
__pragma (optimize("y", on))
__pragma (auto_inline(on))
__pragma( inline_depth(2))
#else
#pragma GCC optimize ("O3")
#endif


#ifdef _MSC_VER
#define __OPT __pragma (optimize( "g", on )) \
__pragma (optimize( "y", on )) \
__pragma (intrinsic(memcpy))\
__pragma (auto_inline(on))\
__pragma (inline_depth (2))
#endif

#endif



