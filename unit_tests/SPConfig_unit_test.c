#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h"
#include "../SPConfig.h"

#define MAX_SIZE 1024


#define LINE printf("%d\n", __LINE__);

#define PRINT(cond, fmt, ERROR) do {\
	printf(fmt "\n", ERROR); \
	ASSERT_TRUE(cond); \
} while(0)


//net{$i} -> pos{$i}
SP_CONFIG_MSG ret_msg[8] = { /*neg:*/ SP_CONFIG_INVALID_STRING, SP_CONFIG_MISSING_NUM_IMAGES, SP_CONFIG_WRONG_FIELD_NAME, SP_CONFIG_MISSING_PREFIX, SP_CONFIG_DOUBLE_USED_VAR, /*pos:*/SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS};

static
bool CHECK_ALL()
{
	int i;
	SP_CONFIG_MSG msg;
	SPConfig conf;
	int num_of_pos = 3;
	int num_of_neg = 5;
	int total = num_of_neg + num_of_pos;
	char file_name[MAX_SIZE];

	for (i = 0; i < total; ++i)
	{
		if (i+1 <= num_of_neg)
			sprintf(file_name, "./SPConfig_test_files/neg_test_%d.config", i + 1);
		else
			sprintf(file_name, "./SPConfig_test_files/pos_test_%d.config", i - num_of_neg + 1);
		conf = spConfigCreate(file_name, &msg);
		PRINT(msg == ret_msg[i], "Error no. %d", msg);
		spConfigDestroy(conf);
	}
	return true;
}

static
bool Check_output()
{
	SP_CONFIG_MSG msg;
	SPConfig conf;
	char* file_name;
	file_name = (char*)malloc(39); // sizeof(char) == 1
	sprintf(file_name, "%s", "./SPConfig_test_files/pos_test_3.config");
	conf = spConfigCreate(file_name, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(0 == strcmp(conf->spImagesDirectory, "../photos/"));
	ASSERT_TRUE(0 == strcmp(conf->spImagesPrefix, "img"));
	ASSERT_TRUE(0 == strcmp(conf->spImagesSuffix, ".jpg"));
	ASSERT_TRUE(10 == conf->spNumOfImages);
	ASSERT_TRUE(true == conf->spMinimalGUI);
	ASSERT_TRUE(0 == strcmp(conf->spPCAFilename, "pca.yml"));
	ASSERT_TRUE(100 == conf->spNumOfFeatures);
	ASSERT_TRUE(true == conf->spExtractionMode);
	ASSERT_TRUE(5 == conf->spNumOfSimilarImages);
	ASSERT_TRUE(0 == strcmp(conf->spLoggerFilename, "stdout"));
	ASSERT_TRUE(20 == conf->spPCADimension);
	return true;
}

int main(int argc, char const *argv[])
{
	RUN_TEST(CHECK_ALL);
	RUN_TEST(Check_output);
	return 0;
}
