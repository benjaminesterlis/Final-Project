#include  "SPConfig.h"
#include <stdlib.h>


#ifndef EOL
#define EOL '\n'
#endif

#define SKIP_WHITESPACES  do { \
		while(read == 32/*' '*/) \
			read = getc(fp); \
	} while(0)
#define O_RONLY "r"
#define	SP_DIR "spImagesDirectory"
#define	SP_PRE "spImagesPrefix"
#define	SP_SUF "spImagesSuffix"
#define	SP_NOI "spNumOfImages"
#define	SP_PCAD "spPCADimension"
#define	SP_PCAF "spPCAFilename"
#define	SP_NOF "spNumOfFeatures"
#define	SP_EXM "spExtractionMode"
#define	SP_NOFSI "spNumOfSimilarImages"
#define	SP_KDTSM "spKDTreeSplitMethod"
#define	SP_KNN "spKNN"
#define	SP_MGUI "spMinimalGUI"
#define	SP_LL "spLoggerLevel"
#define	SP_LF "spLoggerFilename"

typedef enum DKTreeSplitMethods_t {
	MAX_SPRED,
	RANDOM,
	INCREAMENTAL
} SplitMethod;

struct sp_config_t
{
	char *spImagesDirectory;
	char *spImagesPrefix;
	char *spImagesSuffix;
	int spNumOfImages;
	int spPCADimension;
	char *spPCAFilename;
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SplitMethod spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI; 
	int spLoggerLevel;
	char *spLoggerFilename;
};

SPConfig conf;

/** 
 * - cell 0  for spImagesDirectory
 * - cell 1  for spImagesPrefix
 * - cell 2  for spImagesSuffix
 * - cell 3  for spNumOfImages
 * - cell value is 0 if not changed
 * - every time we changed each varilabe we increment the corresponded cell
*/
int corr_field_cell[4]={0};
/**
 * Fit SP_CONFIG_MSG error to his cell
*/
SP_CONFIG_MSG conf_error[4]={SP_CONFIG_MISSING_DIR, SP_CONFIG_MISSING_PREFIX,
						SP_CONFIG_MISSING_SUFFIX, SP_CONFIG_MISSING_NUM_IMAGES};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	FILE* fp;
	int i = 0;
	char var[200]; //beacuse variables names are no longer then 200 ;)
	char* val = NULL;
	char read;
	SP_CONFIG_MSG* new_msg;
	
	if ( filename == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	if ((fp = fopen(filename, O_RONLY))  < 0 )
	{
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	
	if((conf = (SPConfig)malloc(sizeof(SPConfig))) == NULL){
		close(fp);
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	conf->spPCADimension = 20;
	conf->spPCAFilename = "pca.yml";
	conf->spNumOfFeatures = 100;
	conf->spExtractionMode = true;
	conf->spNumOfSimilarImages = 1;
	conf->spKDTreeSplitMethod = MAX_SPRED;
	conf->spKNN = 1;
	conf->spMinimalGUI = false;
	conf->spLoggerLevel = 3;
	conf->spLoggerFilename = "stdout";

	while ((read = getc(fp)) != EOF){
		
		// for empty lines
		if(read == EOL)
			continue;

		// for comments
		if ( read == '#'){
			while ((read = getc(fp)) != EOL || read != EOF);
			if (read == EOF)
				break;
			continue;
		}

		SKIP_WHITESPACES;
		// read till end of variable name 
		while(read != 32/*' '*/ || read != 61/*'='*/ )
		{
			if (97/*'A'*/ < read < 122/*'Z'*/ || 65/*'a'*/ < read < 90/*'z'*/)//only alphabet's chars
			{
				var[i] = read;
				i++;
				read = getc(fp);
			}
			else
			{
				close(fp);
				*msg = SP_CONFIG_INVALID_STRING;
				return NULL;
			}
		}

		SKIP_WHITESPACES;
		//if there are whitespaces in varliabes name
		if ( read != 61/*'='*/) 
		{
			close(fp);
			*msg = SP_CONFIG_INVALID_STRING;
			return NULL;
		}
		
		SKIP_WHITESPACES;

		i = 0;
		//assumed cant be comments after a variable data
		// b3cu453 f0ld3r/f1l3n4m3 c4n c0nt41n # 1n 17   
		//#w3_4r3_1337
		whlie (read != 32/*' '*/ || read != EOL || read != EOF)
		{
			if((val = (char*)realloc(val, i + 1)) == NULL )
			{
				close(fp);
				*msg = SP_CONFIG_ALLOC_FAIL;
				return NULL;
			}
			val[i] = read;
			i++;
		}

		if((val = (char*)realloc(val, i + 1)) == NULL )
		{
			close(fp);
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}
		val[i] = 0;

		if(read == EOL || read == EOF)
		{
			*new_msg = add_field_to_struct(var, val);
			if (*new_msg != SP_CONFIG_SUCCESS){
				close(fp);
				free(val);
				msg = new_msg;
				return NULL;
			}
		}
		SKIP_WHITESPACES;
		if(read != EOL || read != EOF)
		{
			*msg = SP_CONFIG_INVALID_STRING;
			return NULL;
		}
	}	

	for (i = 0; i < 4; i++)
	{
		if (corr_field_cell[i] != 1)
		{
			*msg =  conf_error[i];
			return NULL;
		}
	}

}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index);

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

void spConfigDestroy(SPConfig config);

SP_CONFIG_MSG add_field_to_struct(char* var, char* val)
{	
	char *temp;
	if(val == NULL || var == NULL)
		return SP_CONFIG_INVALID_STRING;
	
	// spImagesDirectory
	if(strcmp(SP_DIR,var) == 0)
	{	corr_field_cell[0]++;
		conf->spImagesDirectory = val;	
		return SP_CONFIG_SUCCESS;
	}

	// spImagesPrefix
	if(strcmp(SP_PRE,var) == 0 )
	{
		corr_field_cell[1]++;
		conf->spImagesPrefix = val;
		return SP_CONFIG_SUCCESS;
	}

	// spImagesSuffix
	if(strcmp(SP_SUF,var) == 0 )
	{
		corr_field_cell[2]++;
		conf->spImagesSuffix = val;
		return SP_CONFIG_SUCCESS;
	}
	
	// spNumOfImages
	if(strcmp(SP_NOI,var) == 0 )
	{
		corr_field_cell[3]++;
		conf->spNumOfImages = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spPCADimension
	if(strcmp(SP_PCAD,var) == 0 )
	{
		conf-> spPCADimension = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spPCAFilename
	if(strcmp(SP_PCAF,var) == 0 )
	{
		conf->spPCAFilename = val;
		return SP_CONFIG_SUCCESS;
	}

	// spNumOfFeatures
	if(strcmp(SP_NOF,var) == 0 )
	{
		conf->spNumOfFeatures = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spExtractionMode
	if(strcmp(SP_EXM,var) == 0 )
	{
		if (strcmp(val,"true") == 0)
		{
			conf->spExtractionMode = true;
			return SP_CONFIG_SUCCESS;
		}
		if  (strcmp(val,"false") == 0)
		{
			conf->spExtractionMode = false;
			return SP_CONFIG_SUCCESS;
		}
		return SP_CONFIG_INVALID_BOOLEAN;
	}

	// spNumOfSimilarImages
	if(strcmp(SP_NOFSI,var) == 0 )
	{
		conf->spNumOfSimilarImages = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spKDTreeSplitMethod
	if(strcmp(SP_KDTSM,var) == 0 )
	{
		if(strcmp(val, "RANDOM") == 0)
		{
			conf->spKDTreeSplitMethod = RANDOM;	
			return SP_CONFIG_SUCCESS;
		}
		if(strcmp(val, "MAX_SPRED") == 0)
		{
			conf->spKDTreeSplitMethod = MAX_SPRED;
			return SP_CONFIG_SUCCESS;
		}
		if(strcmp(val,"INCREAMENTAL") == 0)
		{
			conf->spKDTreeSplitMethod = INCREAMENTAL;
			return SP_CONFIG_SUCCESS;
		}
		return SP_CONFIG_INVALID_ENUM;
	}

	// spKNN
	if(strcmp(SP_KNN,var) == 0 )
	{
		conf->spKNN = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}
	
	// spMinimalGUI
	if(strcmp(SP_MGUI,var) == 0 )
	{
		if (strcmp(val,"true") == 0)
		{
			conf->spMinimalGUI = true;
			return SP_CONFIG_SUCCESS;
		}
		if  (strcmp(val,"false") == 0)
		{
			conf->spMinimalGUI = false;
			return SP_CONFIG_SUCCESS;
		}
		return SP_CONFIG_INVALID_BOOLEAN;
	}

	// spLoggerLevel
	if(strcmp(SP_LL,var) == 0 )
	{
		conf->spLoggerLevel = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spLoggerFilename
	if(strcmp(SP_LF,var) == 0 )
	{
		conf->spLoggerFilename = val;
		return SP_CONFIG_SUCCESS;
	}

	return SP_CONFIG_WRONG_FIELD_NAME;
}
