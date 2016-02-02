/* 
 * File:   functions.cpp
 * Author: cris
 *
 * Created on 01 February 2016, 16:01
 */

#include <vector>
#include "functions.h"
#include <math.h> 
#include <opencv2/imgproc.hpp>

void getRGBMatrix(Mat image, vector<Mat>& channels) {

    cv::split(image, channels);

    cout << "NUM CANALES " << channels.size() << endl;
    //    cout<<"RED"<<channels.at<uchar>(0)<<endl;
    //    cout<<"GREEN"<<channels.at<uchar>(1)<<endl;
    //    cout<<"BLUE"<<channels.at<uchar>(2)<<endl;
    //    return channels;
}

Mat solVector(Mat source, Mat dest, Mat mask) {

    int ncol = 0;
    for (int i = 0; i < mask.rows; i++) {
        for (int j = 0; j < mask.cols; j++) {
            if (mask.at<uchar>(i, j) == 1)
                ncol++;
        }
    }

    Mat solutionV = Mat::zeros(3, ncol, CV_8UC3);
    vector<Mat> rgbDest;
    vector<Mat> rgbSource;

    getRGBMatrix(dest, rgbDest);
    getRGBMatrix(source, rgbSource);

    int internalPix = 0;

    for (int i = 1; i < source.rows - 1; i++) {
        for (int j = 1; i < source.cols - 1; j++) {
            if (mask.at<uchar>(i, j) == 0)
                internalPix += 1;

            if (mask.at<uchar>(i - 1, j) == 0) {
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(0).at<uchar>(i - 1, j);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(1).at<uchar>(i - 1, j);
                solutionV.at<uchar>(3, internalPix) += rgbDest.at(2).at<uchar>(i - 1, j);
            }

            if (mask.at<uchar>(i, j - 1) == 0) {
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(0).at<uchar>(i, j - 1);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(1).at<uchar>(i, j - 1);
                solutionV.at<uchar>(3, internalPix) += rgbDest.at(2).at<uchar>(i, j - 1);
            }

            if (mask.at<uchar>(i + 1, j) == 0) {
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(0).at<uchar>(i + 1, j);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(1).at<uchar>(i + 1, j);
                solutionV.at<uchar>(3, internalPix) += rgbDest.at(2).at<uchar>(i + 1, j);
            }

            if (mask.at<uchar>(i, j + 1) == 0) {
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(0).at<uchar>(i, j + 1);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(1).at<uchar>(i, j + 1);
                solutionV.at<uchar>(3, internalPix) += rgbDest.at(2).at<uchar>(i, j + 1);
            }
        }
    }
}