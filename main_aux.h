/*
 * main_aux.h
 *
 *  Created on: 24 באפר׳ 2016
 *      Author: User
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_


/**
 * the function recieves a 3D array of all the RGB's, the number of images in the database
 * and how many bins are there. on top of it, it gets the queryImg RGB
 * and returns an array of the closest 5 images to the query image by their RGB's
 */

int* closestRGBDistances(int** myPicRGB, int *** allRGBhist,int numberOfImages,int nBins);

/**
 * the function recieves an array which contains in each index the number of hits this specific
 * images has on the query Image by same Sifts.
 * the function sorts it and returns a 5 integer array of the closest images to the query image by the
 * SIFTS
 */

int* LocalDescriptors(int* Sift_Images_Counter, int numberOfImages);

/**
 * the function recieves the queryImage path, the data base of all the image's RGB
 * the number of images and number of bins and print the 5 closest images to the query image
 * by the RGB's
 */

int Global(char*userImgpath, int *** allRGBhist,int numberOfImages,int nBins);

/**
 * the function returns the 5 closest images to the query images by SIFTS.
 */

int Local (int numberOfImages,int maxNFeautres,char* userImgpath,
		double *** allImgsSifts, int* nFeaturesPerImage  );

/**
 * the function frees two 3D arrays
 */
void terminate3D(int***allRGBhist, double***allImgsSifts, int numberOfImages,int* nFeaturesPerImage);

/**
 * the function frees 2D integer array to the given index
 */
void freeByIndexINT (int** arr, int index);

/**
 * the function frees 2D double array to the given index
 */
void freeByIndexDOUBLE (double** arr, int index);
#endif /* MAIN_AUX_H_ */

