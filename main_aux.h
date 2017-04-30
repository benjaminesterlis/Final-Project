#include "SPImageProc.h"

#define C "-c"
#define DEFUALT "spcbir.config"
#define CONFIG ".config"
#define CONF_SIZE 7
#define INVALID_COMMAND "invalid command line: use -c <config_filename>\n"
#define INVALID_CONFIG "invlaid configuration file: %s couldn't be open\n"
#define DEFUALT_CONFIG "The default configuration file %s couldn’t be open\n"
#define FEAT ".feat"
#define O_RDONLY "r"
#define O_WRONLY "w"
#define O_RDWR "r+"
#define EXTRACT_ERROR "Extraction Error!\n"
#define BUILD_ERROR "Build Data instrcution Error!\n"
#define READ_ERROR "Reading Error!\n"
#define OPEN_ERROR "Opening Error!\n"
#define MALLOC_ERROR "Allocation Error!\n"
#define KDTREE_INIT_ERROR "KDTreeInit Error!\n"
#define EXIT "Exting...\n"
#define SRTCMP_ERROR "Strcmp Error!\n"
#define WRONG_FORMAT_ERROR "file is not .config Error!\n"
#define NULL_POINTER_ERROR "NULL pointer Error!\n"


extern "C" {
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <errno.h>
	#include "SPConfig.h"
	#include "KDTree.h"
	#include "SPPoint.h"
	#include "SPBPriorityQueue.h"
}


#define SEND_ERROR( fmt, ...) \
do { \
	printf(fmt, __VA__ARGS__); \
	ret = -1; /* the return value of the main fucntion :)*/ \
	goto CLEANUP; \
} while(0)

#define CHECK_RET(cond, ...) \
do { \
	if (!(cond)) \
		SEND_ERROR("%s" , __VA__ARGS__); \
} while(0)

#define CHECK_NOT(cond, ...) CHECK_RET(!(cond), __VA__ARGS__)

#define MSG_NOT_SUCCESS(msg, error) \
do { \
	if (msg) \
		SEND_ERROR("%s", error); \
} while(0)

#define CHECK_MSG_RET(cond, msg , val_msg) \
do { \
	if(!(cond)) \
	{ \
		msg = val_msg; \
		SEND_ERROR("",""); \
	} \
} while(0)

#define OPEN(fstream, path, prem, msg, msg_val) \
do { \
	CHECK_MSG_RET(fstream = fopen(path, prem), msg, val_msg); \
} while(0)

#define READ(fstream, buffer, size, nitem, msg, val_msg) \
do { \
	CHECK_MSG_RET(fread(buffer, size, nitem, fstream), msg , val_msg); \
} while(0);

#define WRITE(fstream, buffer, size, nitem, msg, val_msg) \
do { \
	CHECK_MSG_RET(fwrite(buffer, size, nitem, fstream) < (size_t)nitem, msg, val_msg); \
} while(0)

int get_least_ms_dword(long qword);

long extend(int most_seg_dword, int least_seg_dword);

int check_file_name(char* file_name);

int extraction_mode(const SPConfig conf, ImageProc proc);

int magic_func(SPPoint* p, char* buf);

SPPoint** read_features(const SPConfig conf, int index, int* num);

int* best_indexes(SPPoint* feature, KDTreeNode* curr, SPBPQueue* bpq, int size);

int _mine_cmp(const void* a, const void *b);

void ShowMinimalResult(int* pic_indexes, char* prefix, char* suffix, char* dir, int size, ImageProc proc);

void ShowNonMinimalResult(char* q_image_path, int* pic_indexes, char* prefix, char* suffix, char* dir, int size);