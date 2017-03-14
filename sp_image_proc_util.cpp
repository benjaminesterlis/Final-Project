#include "sp_image_proc_util.h"
#include <opencv2/core.hpp>// Mat
#include <opencv2/imgcodecs.hpp>//imread
#include <opencv2/highgui.hpp> //imshow, drawKeypoints, waitKey
#include <opencv2/xfeatures2d.hpp>//SiftDescriptorExtractor
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>//calcHist
#include <vector>
#include <cstdio>
#include <iostream>

using namespace cv;
using namespace std;

#define report_error_msg(msg)							\
 	do { 												\
 		printf("Image cannot be loaded - %s:\n",msg);   \
		exit(-1);										\
		}                                               \
 	} while(0)

#define error_msg_ret(msg,ret)							\
 	do {												\
 		printf("An error occurred - %s\n",(msg));		\
 		return (ret);									\
 	} while (0)	

#define NBINS_FAIL "nBins must be positive integer"
#define ALLOC_FAIL "allocation failure"
#define IMAGE_FAIL "Image cannot be loaded - %s:\n"
#define HISTS_NULL "rgbHistA or rgbHistB is NULL"
#define PATH_FAIL "the path give to the function is NULL"
#define IMAGE_LOAD_FAIL "could't load image"
#define FEATURES_FAIL "Features is NULL pointer"
#define N_FEATURES_FAIL "nFeaturesToExtract isn't positive"


SPPoint** spGetRGBHist(const char* str,int imageIndex, int nBins){

	int i;
	Mat src;
	Mat b_hist, g_hist, r_hist;
	// Set the ranges ( for B,G,R )
	float range[] = { 0, 256 };
	const float* histRange = { range };
	double* data;
	SPPoint** Histogram;

	if ( str == NULL || nBins <= 0)
		error_msg_ret(NBINS_FAIL,NULL);

	if ((Histogram = (SPPoint**)malloc(sizeof(SPPoint*)*3)) == NULL )
		error_msg_ret(ALLOC_FAIL,NULL);

	//Load image
	if((src = imread(str,CV_LOAD_IMAGE_COLOR)).empty() ){
		printf(IMAGE_FAIL,str);
		exit(-1);	
	}
	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	// calc 3 Mat's of B G & R
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &nBins, &histRange);

	if (!(data = (double *)malloc(sizeof(double)*nBins)))
		error_msg_ret(ALLOC_FAIL,NULL);

	//for red
	for (i = 0; i < r_hist.rows; ++i)
		data[i] = cvRound(r_hist.at<float>(i));
	if ((Histogram[0] = spPointCreate(data, r_hist.rows, imageIndex)) == NULL)
		goto free1;

	//for green
	for (i = 0; i < g_hist.rows; ++i)
		data[i] = cvRound(g_hist.at<float>(i));	
	if ((Histogram[1] = spPointCreate(data, g_hist.rows, imageIndex)) == NULL)
		goto free2;

	//for blue
	for (i = 0; i < b_hist.rows; ++i)
		data[i] = cvRound(b_hist.at<float>(i));
	if ((Histogram[2] = spPointCreate(data, b_hist.rows, imageIndex)) == NULL)
		goto free3;
	
	free(data);
	return Histogram;
	
	free3:
		spPointDestroy(Histogram[1]);
	free2:
		spPointDestroy(Histogram[0]);
	free1:
		free(Histogram);
	free(data);
	return NULL;

}

double spRGBHistL2Distance(SPPoint** rgbHistA, SPPoint** rgbHistB)
{
	int i;
	double ret = 0;
	if (rgbHistB == NULL || rgbHistA == NULL)
		error_msg_ret(HISTS_NULL, -1);
	for( i = 0; i < 3; ++i)
		ret += (0.33)*spPointL2SquaredDistance(rgbHistA[i], rgbHistB[i]);

	return ret;
}

SPPoint** spGetSiftDescriptors(const char* str, int imageIndex, 
							int nFeaturesToExtract, int *nFeatures)
{
	Mat src, ds1;
	vector<KeyPoint> kp1;
	int i, j;
	double data[128];
	SPPoint** SIFT;

	if (!str)
		error_msg_ret(PATH_FAIL,NULL);
	if((src = imread(str,CV_LOAD_IMAGE_GRAYSCALE)).empty())
		error_msg_ret(IMAGE_LOAD_FAIL,NULL);
	if (!nFeatures)
		error_msg_ret(FEATURES_FAIL,NULL);
	if (nFeaturesToExtract <= 0)
		error_msg_ret(N_FEATURES_FAIL,NULL);

	Ptr<xfeatures2d::SiftDescriptorExtractor> detect =
            xfeatures2d::SIFT::create(nFeaturesToExtract);

	detect->detect(src, kp1);
	detect->compute(src, kp1, ds1);

	*nFeatures = ds1.rows;


	if ((SIFT = (SPPoint**)malloc(sizeof(SPPoint*) * (*nFeatures))) == NULL )
		error_msg_ret(ALLOC_FAIL,NULL);

	for (i = 0; i < ds1.rows; ++i)
	{
		for (j = 0; j < 128 ; ++j)
			data[j] = (double)cvRound(ds1.at<float>(i,j));
		SIFT[i] = spPointCreate(data,128,imageIndex);
	}
	return SIFT;
}

int* spBestSIFTL2SquaredDistance(int kClosest, SPPoint* queryFeature,
							SPPoint*** databaseFeatures, int numberOfImages,
							int* nFeaturesPerImage)
{
	int i, j;
	SPBPQueue *length_l2 = spBPQueueCreate(kClosest);
	for (i = 0; i < numberOfImages; i++)
	{
		for (j = 0; j < *nFeaturesPerImage; ++j)
		{
			spBPQueueEnqueue(length_l2,i,
				spPointL2SquaredDistance(databaseFeatures[i][j],queryFeature));
		}
	}
	int *indexes;

	if ((indexes = (int *)malloc(sizeof(int) *kClosest)) == NULL)
		error_msg_ret(ALLOC_FAIL,NULL);
	for (i = 0; i < kClosest; i++)
	{
		indexes[i] = spBPQueueMinValueIndex(length_l2);
		spBPQueueDequeue(length_l2);
	}
	
	spBPQueueDestroy(length_l2);
	return indexes;
} // need to free memory in main