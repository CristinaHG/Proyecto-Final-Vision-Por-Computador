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


void getRGBMatrix(cv::Mat &image, vector<cv::Mat> &channels);

cv::Mat solVector(cv::Mat &source, cv::Mat &dest, cv::Mat &mask);

int guidanceVect(cv::Mat &sourceChannel, float x, float y);

cv::Mat CoefficientMatrix(cv::Mat &source, cv::Mat &dest, cv::Mat &mask, cv::Mat &index);

//---------------------------
cv::Mat reconstructImage(cv::Mat &r, cv::Mat &g, cv::Mat &b, cv::Mat &mask, cv::Mat &dest, cv::Mat &indexes);

cv::Mat getIndexes(cv::Mat &mask, int cols, int rows);

#endif	/* FUNCTIONS_H */

