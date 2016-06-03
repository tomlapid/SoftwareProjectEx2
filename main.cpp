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
	#include "sp_image_proc_util.h"
	#include <opencv2/xfeatures2d.hpp>
    #include "main_aux.h"
	#include <string>

	using namespace std;
	using namespace cv;

	#define EXIT_MSG "Exiting...\n"
	#define Enter_Images_Path "Enter images directory path:\n"
	#define Enter_Prefix "Enter images prefix:\n"
	#define Enter_Images_Number "Enter number of images:\n"
	#define Images_Number_Error "An error occurred - invalid number of images\n"
	#define Enter_Suffix "Enter images suffix:\n"
	#define Enter_Bins_Number "Enter number of bins:\n"
	#define Bins_Number_Error "An error occurred - invalid number of bins\n"
	#define Enter_Features_Number "Enter number of features:\n"
	#define Features_Number_Error "An error occurred - invalid number of features\n"
	#define Enter_Query "Enter a query image or # to terminate:\n"
	#define Allocation_failure "An error occurred - allocation failure\n"

int main() {

	setvbuf(stdout, NULL, _IONBF, 0);

	char path[1025];
	char prefix[1025];
	char suffix[1025];
	char queryImage[1025];

	int *** allRGBhist;
	double *** allImgsSifts;
	int* nFeaturesPerImage;

	int numberOfImages;
	int nBins;
	int maxNFeautres;
	int terminate;

	printf(Enter_Images_Path);
	scanf("%s", path);


	printf(Enter_Prefix);
	scanf("%s", prefix);


	printf(Enter_Images_Number);
	scanf("%d", &numberOfImages);


	if (numberOfImages < 1){
		printf(Images_Number_Error);
		return -1;
	}


	printf(Enter_Suffix);
	scanf("%s", suffix);


	printf(Enter_Bins_Number);
	scanf("%d", &nBins);
	if (nBins < 1){
		printf(Bins_Number_Error);
		return -1;
	}


	printf(Enter_Features_Number);
	scanf("%d", &maxNFeautres);
	if (maxNFeautres < 1){
		printf(Features_Number_Error);
		return -1;
	}

	allRGBhist = (int***) malloc(numberOfImages * sizeof(int**));
	if(allRGBhist==NULL){
		printf(Allocation_failure);
		return -1;
	}

	allImgsSifts = (double***) malloc(numberOfImages * sizeof(double**));
	if(allImgsSifts==NULL){
		printf(Allocation_failure);
		free(allRGBhist);
		return -1;
	}

	nFeaturesPerImage = (int*) malloc(numberOfImages * sizeof(int));
	if(nFeaturesPerImage==NULL){
		printf(Allocation_failure);
		free(allRGBhist);
		free(allImgsSifts);
		return -1;
	}



	//building paths for the imgs and insert their RGB's and SIFTs to our arrays.
	for(int i=0 ; i<numberOfImages ; i++){
		char* Imgpath;
		Imgpath = (char*)malloc((strlen(path) + strlen(prefix)+ strlen(suffix)+4)*sizeof(char));
		if(Imgpath==NULL){
			printf(Allocation_failure);
			free(allRGBhist);
			free(allImgsSifts);
			return -1;
		}

		sprintf (Imgpath, "%s%s%d%s",path,prefix,i,suffix);

		int nFeatures;

		//build RGBDataBase
		allRGBhist[i] = spGetRGBHist(Imgpath,nBins);
		if(allRGBhist[i] == NULL){
			for(int j=0 ; j<i ; j++){
				freeByIndexINT(allRGBhist[j],3);
			}
			free(allRGBhist);
			free(allImgsSifts);
			free(nFeaturesPerImage);
			return -1;
		}

		//build SIFTSDataBase
	    allImgsSifts[i] = spGetSiftDescriptors(Imgpath, maxNFeautres, &nFeatures);
		if(allImgsSifts[i] == NULL){
			for(int j=0 ; j<i ; j++){
				freeByIndexDOUBLE(allImgsSifts[j],nFeaturesPerImage[j]);
			}
			free(allImgsSifts);
			terminate3D(allRGBhist,NULL,numberOfImages,NULL);
			free(nFeaturesPerImage);
			return -1;
		}

	    nFeaturesPerImage[i] = nFeatures;

	    free(Imgpath);
	}


	while(true){

		printf(Enter_Query);
		scanf("%s", queryImage);

		if(strcmp(queryImage,"#")==0){
			printf(EXIT_MSG);
			terminate3D(allRGBhist,allImgsSifts,numberOfImages,nFeaturesPerImage);
			free(nFeaturesPerImage);
			return 0;
		}

		//find and return the 5 closest imgs by RGB's
		terminate = Global(queryImage,allRGBhist,numberOfImages,nBins);
		if (terminate == -1){
			terminate3D(allRGBhist,allImgsSifts,numberOfImages,nFeaturesPerImage);
			free(nFeaturesPerImage);
			return terminate;
		}
		//find and return the 5 closest imgs by SIFT Descriptors.
		terminate = Local (numberOfImages,maxNFeautres,queryImage,
				allImgsSifts,nFeaturesPerImage );
		if (terminate == -1){
			terminate3D(allRGBhist,allImgsSifts,numberOfImages,nFeaturesPerImage);
			free(nFeaturesPerImage);
			return terminate;
		}

	}

	//free memory
	terminate3D(allRGBhist,allImgsSifts,numberOfImages,nFeaturesPerImage);
	free(nFeaturesPerImage);

	return 0;

}

