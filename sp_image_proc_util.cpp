/*
 * sp_image_proc_util.cpp
 *
 *  Created on: 19 באפר׳ 2016
 *      Author: User
 */
#include <opencv2/imgproc.hpp>//calcHist
#include <opencv2/core.hpp>//Mat
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/xfeatures2d.hpp>
#include "main_aux.h"

using namespace std;
using namespace cv;

#define Allocation_failure "An error occurred - allocation failure\n"
#define One_Third 0.33

int** spGetRGBHist(char* str, int nBins){

	if(str==NULL || nBins<=0){
		return NULL;
	}

	Mat src;

	/// Load image
	src = imread(str, CV_LOAD_IMAGE_COLOR);


	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	/// Set the other parameters:
	int nImages = 1;


	//Output
	Mat b_hist, g_hist, r_hist;


	/// Compute the histograms:
	/// The results will be store in b_hist,g_hist,r_hist.
	/// The output type of the matrices is CV_32F (float)
	calcHist(&bgr_planes[0], nImages, 0, Mat(), b_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[1], nImages, 0, Mat(), g_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[2], nImages, 0, Mat(), r_hist, 1, &nBins, &histRange);


    int** result;
    result = (int **) malloc(3 * sizeof(int*) );

    if(result == NULL){
    	printf(Allocation_failure);
    	return NULL;
    }

    for (int i=0;i<3; i++){

         result[i] = (int*) malloc(nBins * sizeof(int));

         if(result[i]==NULL){
        	 freeByIndexINT (result, i);
        	 printf(Allocation_failure);
        	 return NULL;
         }
    }

    for(int i = 0; i < nBins; i++){
       	result[0][i] = (int)r_hist.at<float>(i,0);
       	result[1][i] = (int)g_hist.at<float>(i,0);
       	result[2][i] = (int)b_hist.at<float>(i,0);
    }

    return result;

}

double spRGBHistL2Distance(int** histA, int** histB, int nBins){


	if( nBins <= 0  || histA == NULL || histB == NULL){
		return -1;
	}

    double Red_Sum = 0;
    double Red_Dif;

    double Green_Sum = 0;
    double Green_Dif;

    double Blue_Sum = 0;
    double Blue_Dif;

    for (int i =0; i< nBins; i++){

    	Red_Dif = (double) ((histA[0][i]) - (histB[0][i]));
    	Red_Sum+=pow(Red_Dif,2);

    	Green_Dif = (double) ((histA[1][i]) - (histB[1][i]));
    	Green_Sum+=pow(Green_Dif,2);

    	Blue_Dif = (double) ((histA[2][i]) - (histB[2][i]));
    	Blue_Sum+=pow(Blue_Dif,2);

    }

    double result = (One_Third)*(Red_Sum + Green_Sum + Blue_Sum);
    return result;
}



double** spGetSiftDescriptors(char* str, int maxNFeautres, int *nFeatures){

	if(str == NULL || nFeatures == NULL || maxNFeautres <= 0){
		return NULL;
	}

	Mat src;
	/// Load image
	src = imread(str, CV_LOAD_IMAGE_GRAYSCALE);

	vector<KeyPoint> kp1;
	Mat ds1;
	Ptr<xfeatures2d::SiftDescriptorExtractor> detect = xfeatures2d::SIFT::create(maxNFeautres);
	detect -> detect (src , kp1, Mat());
	detect -> compute(src, kp1, ds1);

	*nFeatures = ds1.rows;

    double** result;
    result = (double **) malloc((*nFeatures) * sizeof(double*) );
    if(result == NULL){
    		printf(Allocation_failure);
    		return NULL;
    }

    for (int i=0;i<(*nFeatures); i++){
         result[i] = (double*) malloc(128 * sizeof(double));
         if(result[i]==NULL){
           	printf(Allocation_failure);
           	freeByIndexDOUBLE (result, i);
        	return NULL;
         }
    }

    for(int i=0;i<(*nFeatures); i++){
    	for(int j=0; j<128; j++){
    		result[i][j] = (double)ds1.at<float>(i,j);
    	}
    }

    return result;

}


double spL2SquaredDistance(double* featureA, double* featureB){

	if(featureA == NULL || featureB == NULL){
		return -1;
	}
	double sum = 0;
	for(int i=0 ; i<128; i++){
		sum+=((featureA[i]-featureB[i])*(featureA[i]-featureB[i]));
	}

	return sum;

}



void InsertFeatureNew (int bestNFeatures, int Imgindex, int FeatureDistance,
		int* BestImages,int* BestDistances, int* maxDistanceInArray){

	int insertPoint;

	for(int i=0 ; i<bestNFeatures; i++){
		if(BestImages[i] == -1){
			BestImages[i] = Imgindex;
			BestDistances[i] = FeatureDistance;
			return;
		}
		else if(FeatureDistance < BestDistances[i]){
			insertPoint = i;
			break;
		}
	}

	for(int i=(bestNFeatures-1); i>insertPoint; i--){
			BestImages[i] = BestImages[i-1];
			BestDistances[i] = BestDistances[i-1];
	}
	BestImages[insertPoint] = Imgindex;
	BestDistances[insertPoint] = FeatureDistance;
	*maxDistanceInArray = BestDistances[bestNFeatures-1];

}


int* spBestSIFTL2SquaredDistance(int bestNFeatures, double* featureA,
		double*** databaseFeatures, int numberOfImages,
		int* nFeaturesPerImage){

		if(featureA == NULL || databaseFeatures == NULL || numberOfImages<=1){
			return NULL;
		}

		int maxDistanceInArray = -1;
		// the next two arrays contains pairs of image index and its distance from featureA
		int* BestImages;
		int* BestDistances;

		BestImages = (int*) malloc( (bestNFeatures) * sizeof(int) );
		if(BestImages == NULL){
			printf(Allocation_failure);
			return NULL;
		}


		BestDistances = (int*) malloc( (bestNFeatures) * sizeof(int) );
		if(BestDistances == NULL){
			free(BestImages);
			printf(Allocation_failure);
			return NULL;
		}


		for (int i=0; i<bestNFeatures; i++){
			BestDistances[i]=-1;
			BestImages[i]=-1;
		}


		for(int i=0 ; i < numberOfImages ; i++){
			for(int j=0 ; j < nFeaturesPerImage[i] ; j++){

				int currDistance = spL2SquaredDistance(featureA,databaseFeatures[i][j]);
				if(currDistance == -1){
					free(BestImages);
					free(BestDistances);
					return NULL;
				}

				if(currDistance < maxDistanceInArray || maxDistanceInArray == -1){
					InsertFeatureNew (bestNFeatures, i, currDistance, BestImages,
							BestDistances, &maxDistanceInArray);
				}

			}
		}


		free(BestDistances);
		return BestImages;

}
