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

/**
 * 
 * @param source
 * @param dest
 * @param mask
 * @return 
 */
cv::Mat solVector(const cv::Mat &source, const cv::Mat &dest, const cv::Mat &mask, bool, int nonz);

/**
 * 
 * @param sourceChannel
 * @param x
 * @param y
 * @return 
 */
double guidanceVect(cv::Mat &sourceChannel, int x, int y);
double guidanceVectMixin(cv::Mat &sourceChannel, cv::Mat &destChannel, int x, int y);

/**
 * 
 * @param source
 * @param dest
 * @param mask
 * @param index
 * @return 
 */
cv::Mat coefficientMatrix(const cv::Size &sz, const cv::Mat &mask, const cv::Mat &index, int nonz);

/**
 * 
 * @param source
 * @param dest
 * @param mask
 * @return 
 */
cv::Mat seamlessClonningNormal(cv::Mat &source, cv::Mat &dest, cv::Mat &mask);
cv::Mat seamlessClonningMixin(cv::Mat &source, cv::Mat &dest, cv::Mat &mask, cv::Point p);

//---------------------------
/**
 * 
 * @param r
 * @param g
 * @param b
 * @param mask
 * @param dest
 * @param indexes
 * @return 
 */
cv::Mat reconstructImage(cv::Mat &r, cv::Mat &g, cv::Mat &b, const cv::Mat &mask, cv::Mat &dest, const cv::Mat &indexes, const cv::Point &p);

/**
 * 
 * @param mask
 * @param cols
 * @param rows
 * @return 
 */
cv::Mat getIndexes(cv::Mat &mask, int cols, int rows);

#endif	/* FUNCTIONS_H */

