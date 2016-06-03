/*
 * main_aux.cpp
 *
 *  Created on: 24 באפר׳ 2016
 *      Author: User
 */

	#include <iostream>
	#include <stdio.h>
	#include <stdlib.h>
    #include "main_aux.h"
 	#include "sp_image_proc_util.h"
	#include <string.h>
	using namespace std;
	#define Global_Descriptors "Nearest images using global descriptors:\n"
	#define Allocation_failure "An error occurred - allocation failure\n"
	#define Local_Descriptors "Nearest images using local descriptors:\n"



void freeByIndexINT (int** arr, int index){
	for (int i = 0; i < index; i++){
		free(arr[i]);
	}
	free(arr);
}


void freeByIndexDOUBLE (double** arr, int index){
	for (int i = 0; i < index; i++){
		free(arr[i]);
	}
	free(arr);
}


int cmpfunc (const void*  a, const void*  b)
{
 double* first =  *( (double**) a );
 double* second =  *( (double**) b );
 if(first[0] > second[0]){return 1;}
 if(first[0] < second[0]){return -1;}
 return 0;
}


int* closestRGBDistances(int** myPicRGB, int *** allRGBhist,int numberOfImages,int nBins){

	int*closestRGBImgs;
	double** RGBDistances;

	RGBDistances = (double**) malloc(numberOfImages * sizeof(double*));
	if(RGBDistances == NULL){
		printf(Allocation_failure);
		return NULL;
	}

	for(int i=0; i<numberOfImages; i++){
		RGBDistances[i] = (double*) malloc(2 * sizeof(double));
		if(RGBDistances[i] == NULL){
			freeByIndexDOUBLE(RGBDistances, i);
			printf(Allocation_failure);
			return NULL;
		}
	}

	closestRGBImgs=(int*) malloc (5 * sizeof(int));
	if(closestRGBImgs == NULL){
		freeByIndexDOUBLE(RGBDistances, numberOfImages);
		printf(Allocation_failure);
		return NULL;
	}



	for(int i=0; i<numberOfImages; i++){
		RGBDistances[i][0] = spRGBHistL2Distance(myPicRGB,allRGBhist[i], nBins);
		RGBDistances[i][1] = i;
		if(RGBDistances[i][0] == -1){
			freeByIndexDOUBLE(RGBDistances, numberOfImages);
			free(closestRGBImgs);
			return NULL;
		}
	}

	qsort(RGBDistances, numberOfImages, sizeof(double*), cmpfunc);

	for(int i=0; i<5; i++){
		closestRGBImgs[i] = (int)RGBDistances[i][1];
	}

	freeByIndexDOUBLE(RGBDistances, numberOfImages);
	return closestRGBImgs;

}


int cmpfunc2 (const void*  a, const void*  b)
{
 int* first =  *( (int**) a );
 int* second =  *( (int**) b );

 if(first[0] > second[0]){return -1;}
 if(first[0] < second[0]){return 1;}

 if(first[1] > second[1]){return 1;}
 if(first[1] < second[1]){return -1;}

 return 0;

}


int* LocalDescriptors(int* Sift_Images_Counter, int numberOfImages){

	//2D array that contains in the first column the hits and in the second the images indexs
	int** SiftsHitCounter;

	int* BestFiveLocal;

	SiftsHitCounter = (int**) malloc (numberOfImages * sizeof(int*));
	if(SiftsHitCounter == NULL){
		printf(Allocation_failure);
		return NULL;
	}
	for(int i = 0; i < numberOfImages; i++){
		SiftsHitCounter[i] = (int*) malloc (2 * sizeof(int));
		if(SiftsHitCounter[i] == NULL){
			freeByIndexINT(SiftsHitCounter, i);
			printf(Allocation_failure);
			return NULL;
		}
	}


	BestFiveLocal = (int*) malloc (5 * sizeof(int));
	if(BestFiveLocal == NULL){
		freeByIndexINT(SiftsHitCounter, numberOfImages);
		printf(Allocation_failure);
		return NULL;
	}

	for(int i = 0; i < numberOfImages; i++){
		SiftsHitCounter[i][0] = Sift_Images_Counter[i];
		SiftsHitCounter[i][1] = i;
	}

	qsort(SiftsHitCounter, numberOfImages, sizeof(int*), cmpfunc2);

	for(int i=0 ; i<5 ; i++){
		BestFiveLocal[i] = SiftsHitCounter[i][1];
	}


	freeByIndexINT(SiftsHitCounter, numberOfImages);
	return BestFiveLocal;

}


int Global(char*userImgpath, int *** allRGBhist,int numberOfImages,int nBins){

	int** userPicRGB;
	userPicRGB = spGetRGBHist(userImgpath,nBins);

	if(userPicRGB == NULL){
		return -1;
	}

	int* closestRGBImgs;
	closestRGBImgs = closestRGBDistances(userPicRGB, allRGBhist, numberOfImages, nBins);

	if(closestRGBImgs == NULL){
		freeByIndexINT(userPicRGB, 3);
		return -1;
	}


	printf(Global_Descriptors);
	printf("%d, %d, %d, %d, %d\n",closestRGBImgs[0],closestRGBImgs[1],closestRGBImgs[2],closestRGBImgs[3],closestRGBImgs[4] );

	freeByIndexINT(userPicRGB, 3);
	free(closestRGBImgs);
	return 0;

}

int Local (int numberOfImages,int maxNFeautres,char* userImgpath,
		double *** allImgsSifts, int* nFeaturesPerImage  ){


	int* Sift_Images_Counter;
	double** userPicSifts;
	int nFeatures;
	int* closestSIFTImages;


	Sift_Images_Counter = (int*) malloc (numberOfImages * sizeof(int));
	if(Sift_Images_Counter==NULL){
		printf(Allocation_failure);
		return -1;
	}
	for(int i=0 ; i<numberOfImages; i++){
		Sift_Images_Counter[i] = 0;
	}



	userPicSifts = spGetSiftDescriptors(userImgpath,maxNFeautres, &nFeatures);
	if(userPicSifts == NULL){
		free(Sift_Images_Counter);
		return -1;
	}

	for(int i=0 ; i<nFeatures; i++){

		int* bestFiveImgs;

		bestFiveImgs = spBestSIFTL2SquaredDistance(5 ,userPicSifts[i],
				allImgsSifts,numberOfImages,
				nFeaturesPerImage);

		if(bestFiveImgs == NULL){
			free(Sift_Images_Counter);
			freeByIndexDOUBLE(userPicSifts,nFeatures);
			return -1;
		}

		for(int j=0; j<5; j++){
			int index = bestFiveImgs[j];
			Sift_Images_Counter[index]++;
		}

		free(bestFiveImgs);
	}


	closestSIFTImages = LocalDescriptors(Sift_Images_Counter,numberOfImages);
	if(closestSIFTImages == NULL){
		free(Sift_Images_Counter);
		freeByIndexDOUBLE(userPicSifts,nFeatures);
		return -1;
	}


	printf(Local_Descriptors);
	printf("%d, %d, %d, %d, %d\n",closestSIFTImages[0],closestSIFTImages[1],closestSIFTImages[2],closestSIFTImages[3],closestSIFTImages[4] );

	freeByIndexDOUBLE(userPicSifts,nFeatures);
	free(Sift_Images_Counter);
	free(closestSIFTImages);
	return 0;
}


void terminate3D(int***allRGBhist, double***allImgsSifts, int numberOfImages,int* nFeaturesPerImage){

	for(int i=0 ; i<numberOfImages ; i++){

		if(allRGBhist != NULL){
			freeByIndexINT(allRGBhist[i],3);
		}

		if(allImgsSifts != NULL){
			freeByIndexDOUBLE(allImgsSifts[i],nFeaturesPerImage[i]);
		}

	}

	free(allRGBhist);
	free(allImgsSifts);

}


