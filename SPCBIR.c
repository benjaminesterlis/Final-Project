#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPConfig.h"

#define DEFAULT "spcbir.config"

int main(int argc, char const *argv[])
{
	SP_CONFIG_MSG* msg;
	SPConfig conf;
	if(argc == 1)
	{
		conf = spConfigCreate(DEFAULT, msg);
		if (conf == NULL)
		{
			printf("%s\n", "The default Configuration file "DEFAULT" couldn't be open");
			return -1;
		}
	}
	else if (argc == 3)
	{
		if (!strcmp(argv[1], "-c"))
		{
			conf = spConfigCreate(argv[2], msg);
			if (conf == NULL)
			{
				printf("%s%s%s\n", "The Configuration file ", argv[2], " couldn't be open");
				return -1;
			}
		}
		else
		{
			printf("%s\n", "invalid command line:use -c <config_filename>");
			return -1;
		}
	}
	else
	{
		printf("%s\n", "invalid command line:use -c <config_filename>");
		return -1;
	}
	return 0;
}