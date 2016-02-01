/* 
 * File:   functions.h
 * Author: cris
 *
 * Created on 01 February 2016, 19:37
 */

#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std; 
using namespace cv; 

void getRGBMatrix(Mat image,vector<Mat>& channels);
Mat solVector(Mat source,Mat dest,Mat mask);
#endif	/* FUNCTIONS_H */

