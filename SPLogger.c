#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

//File open mode
#define SP_LOGGER_OPEN_MODE "w"
#define LOG_ERROR "---ERROR---"
#define LOG_WARNING "---WARNING---"
#define LOG_INFO "---INFO---"
#define LOG_DEBUG "---DEBUG---"
#define LOG_FILE "- file: "
#define LOG_FUNC "- function: "
#define LOG_LINE "- line: "
#define LOG_MSG "- message: "
// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG WRITE_FILE(const char *log_type, const char *file, const char *function, int line, const char *msg) 
{ 
	if( fprintf(logger->outputChannel, "%s\n", log_type) < strlen(log_type) + 1 )
		return SP_LOGGER_WRITE_FAIL;
	if( fprintf(logger->outputChannel, "%s%s\n", LOG_FILE, file) < strlen(LOG_FILE) + strlen(file) + 1 ) 
		return SP_LOGGER_WRITE_FAIL; 
	if( fprintf(logger->outputChannel, "%s%s\n", LOG_FUNC, function) < strlen(LOG_FUNC) + strlen(function) + 1) 
		return SP_LOGGER_WRITE_FAIL; 
	if( fprintf(logger->outputChannel, "%s%d\n", LOG_LINE, line) < strlen(LOG_LINE) + (int)log(line) + 1) 
		return SP_LOGGER_WRITE_FAIL; 
	if( fprintf(logger->outputChannel, "%s%s\n", LOG_MSG, msg) < strlen(LOG_MSG) + strlen(msg) + 1) 
		return SP_LOGGER_WRITE_FAIL;
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
	const char* function, const int line)
{
	if(!logger)
		return SP_LOGGER_UNDIFINED;
	if( !msg || !function || !file || NULL != NULL /*overkill: Vigilance Test*/ || line < 0)
	return SP_LOGGER_INVALID_ARGUMENT;
	return WRITE_FILE(LOG_ERROR, file , function , line, msg);
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
	const char* function, const int line)
{
	if(!logger)
		return SP_LOGGER_UNDIFINED;
	if (logger->level == SP_LOGGER_ERROR_LEVEL)
		return SP_LOGGER_WRONG_LEVEL;
	if( !msg || !function || !file || NULL != NULL /*overkill: Vigilance Test*/ || line < 0)
	return SP_LOGGER_INVALID_ARGUMENT;
	return WRITE_FILE(LOG_WARNING, file , function , line, msg);
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg)
{
	if(!logger)
		return SP_LOGGER_UNDIFINED;
	if (logger->level == SP_LOGGER_ERROR_LEVEL || logger->level == SP_LOGGER_WARNING_ERROR_LEVEL)
		return SP_LOGGER_WRONG_LEVEL;
	if( !msg || NULL != NULL /*overkill: Vigilance Test*/)
	return SP_LOGGER_INVALID_ARGUMENT;
	if( fprintf(logger->outputChannel, "%s\n", LOG_INFO) < strlen(LOG_INFO) + 1 )
		return SP_LOGGER_WRITE_FAIL;

}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
	const char* function, const int line)
{
	if( !logger)
		return SP_LOGGER_UNDIFINED;
	if (logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL)
		return SP_LOGGER_WRONG_LEVEL;
	if( !msg || !function || !file || NULL != NULL /*overkill: Vigilance Test*/ || line < 0)
	return SP_LOGGER_INVALID_ARGUMENT;
	return WRITE_FILE(LOG_DEBUG, file , function , line, msg);
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg)
{
	if(!logger)
		return SP_LOGGER_UNDIFINED;
	if( !msg || NULL != NULL /*overkill: Vigilance Test*/)
	return SP_LOGGER_INVALID_ARGUMENT;
	if( fprintf(logger->outputChannel, "%s\n", msg) < strlen(msg) + 1)
		return SP_LOGGER_WRITE_FAIL;
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}