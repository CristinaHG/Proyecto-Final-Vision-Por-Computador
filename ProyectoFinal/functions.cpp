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

#define _DEBUG 1
#if _DEBUG
#define LOG_MESSAGE(x) std::cout << __FILE__ << " (" << __LINE__ << "): " << x << std::endl;
#else
#define LOG_MESSAGE(x)
#endif

cv::Mat solVector(cv::Mat &source, cv::Mat &dest, cv::Mat &mask) {

    int ncol = cv::countNonZero(mask);

    cv::Mat solutionV = cv::Mat::zeros(3, ncol, CV_64F);
    vector<cv::Mat> rgbDest;
    vector<cv::Mat> rgbSource;

    cv::split(dest, rgbDest);
    cv::split(source, rgbSource);

    int internalPix = 0;
    int sumNred, sumNblue, sumNgreen;

    for (int i = 1; i < source.cols - 1; i++) {
        for (int j = 1; j < source.rows - 1; j++) {
            if (mask.at<uchar>(i, j) != 0) {
                internalPix += 1;

                if (mask.at<uchar>(i - 1, j) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(0).at<uchar>(i - 1, j);
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<uchar>(i - 1, j);
                    solutionV.at<double>(2, internalPix) += rgbDest.at(2).at<uchar>(i - 1, j);
                }

                if (mask.at<uchar>(i, j - 1) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(0).at<uchar>(i, j - 1);
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<uchar>(i, j - 1);
                    solutionV.at<double>(2, internalPix) += rgbDest.at(2).at<uchar>(i, j - 1);
                }

                if (mask.at<uchar>(i + 1, j) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(0).at<uchar>(i + 1, j);
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<uchar>(i + 1, j);
                    solutionV.at<double>(2, internalPix) += rgbDest.at(2).at<uchar>(i + 1, j);
                }

                if (mask.at<uchar>(i, j + 1) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(0).at<uchar>(i, j + 1);
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<uchar>(i, j + 1);
                    solutionV.at<double>(2, internalPix) += rgbDest.at(2).at<uchar>(i, j + 1);
                }

                sumNred = guidanceVect(rgbSource.at(0), i, j);
                sumNgreen = guidanceVect(rgbSource.at(1), i, j);
                sumNblue = guidanceVect(rgbSource.at(2), i, j);

                solutionV.at<double>(0, internalPix) += sumNred;
                solutionV.at<double>(1, internalPix) += sumNgreen;
                solutionV.at<double>(2, internalPix) += sumNblue;
            }
        }
    }
    return solutionV;
}

uchar guidanceVect(cv::Mat &sourceChannel, int x, int y) {

    uchar n1 = sourceChannel.at<uchar>(x, y) - sourceChannel.at<uchar>(x - 1, y);
    uchar n2 = sourceChannel.at<uchar>(x, y) - sourceChannel.at<uchar>(x + 1, y);
    uchar n3 = sourceChannel.at<uchar>(x, y) - sourceChannel.at<uchar>(x, y - 1);
    uchar n4 = sourceChannel.at<uchar>(x, y) - sourceChannel.at<uchar>(x, y + 1);

    return n1 + n2 + n3 + n4;
}

cv::Mat coefficientMatrix(cv::Mat &source, cv::Mat &dest, cv::Mat &mask, cv::Mat &index) {

    int insidePix = 0;

    int n = cv::countNonZero(mask);

//    int size[] = {n, n};
//    cv::SparseMat coeffMatrix = cv::SparseMat(2, size, CV_64F);
        cv::Mat coeffMatrix = cv::Mat::zeros(n, n, CV_64F);

    for (int i = 1; i < source.cols - 1; i++) {
        for (int j = 1; j < source.rows - 1; j++) {
            if (mask.at<uchar>(i, j) != 0) {
                if (mask.at<uchar>(i - 1, j) != 0) {
                    coeffMatrix.at<double>(insidePix, index.at<double>(i - 1, j)) = -1;
                }
                if (mask.at<uchar>(i, j - 1) != 0) {
                    coeffMatrix.at<double>(insidePix, index.at<double>(i, j - 1)) = -1;
                }
                if (mask.at<uchar>(i + 1, j) != 0) {
                    coeffMatrix.at<double>(insidePix, index.at<double>(i + 1, j)) = -1;
                }
                if (mask.at<uchar>(i, j + 1) != 0) {
                    coeffMatrix.at<double>(insidePix, index.at<double>(i, j + 1)) = -1;
                }
                coeffMatrix.at<double>(insidePix, insidePix) = 4;
                insidePix += 1;
            }
        }
    }
    return coeffMatrix;
}

cv::Mat seamlessClonningNormal(cv::Mat &source, cv::Mat &dest, cv::Mat &mask) {


    vector<cv::Mat> DestChannels;
    vector<cv::Mat> SourceChannels;

    //     int insidePix = cv::countNonZero(mask);

    cv::split(source, SourceChannels);
    cv::split(dest, DestChannels);

    cv::Mat indexes = getIndexes(mask, dest.cols, dest.rows);

    cv::Mat coeffMat = coefficientMatrix(source, dest, mask, indexes);
    cv::Mat solutionVector = solVector(source, dest, mask);

    cv::Mat solR;
    cv::Mat solG;
    cv::Mat solB;

    cv::solve(coeffMat, solutionVector.row(0).t(), solR,  cv::DECOMP_SVD);
    cv::solve(coeffMat, solutionVector.row(1).t(), solG,  cv::DECOMP_SVD);
    cv::solve(coeffMat, solutionVector.row(2).t(), solB,  cv::DECOMP_SVD);

    cv::Mat result = reconstructImage(solR, solG, solB, mask, dest, indexes);

    return result;
}




//////////////////////////////////////////////////////////////////////

cv::Mat reconstructImage(cv::Mat &r, cv::Mat &g, cv::Mat &b, cv::Mat &mask, cv::Mat &dest, cv::Mat &indexes) {

    r.convertTo(r, CV_8UC1);
    g.convertTo(g, CV_8UC1);
    b.convertTo(b, CV_8UC1);
    
    vector<cv::Mat> destChannels;

    cv::split(dest, destChannels);

    cv::Mat newR, newG, newB;

    destChannels.at(0).copyTo(newR);
    destChannels.at(1).copyTo(newG);
    destChannels.at(2).copyTo(newB);
    
    for (int i = 0; i < mask.cols; i++) {
        for (int k = 0; k < mask.rows; k++) {
            if (mask.at<uchar>(i, k) != 0) {
                int index = indexes.at<double>(i, k);
                newR.at<uchar>(i, k) = r.at<uchar>(index);
                newG.at<uchar>(i, k) = g.at<uchar>(index);
                newB.at<uchar>(i, k) = b.at<uchar>(index);
            }
        }
    }
    cv::Mat assembledImage = cv::Mat(newR.size(), newR.type());
    vector<cv::Mat_<uchar> > mv;

    mv.push_back(newR);
    mv.push_back(newG);
    mv.push_back(newB);

    cv::merge(mv, assembledImage);
    
    return assembledImage;
}

cv::Mat getIndexes(cv::Mat &mask, int cols, int rows) {

    cv::Mat indexes = cv::Mat::zeros(cols, rows, CV_64F);

    double insiders = 0;

    for (int i = 0; i < mask.cols; i++) {
        for (int j = 0; j < mask.rows; j++) {
            if (mask.at<uchar>(i, j) != 0) {
                indexes.at<double>(i, j) = ++insiders;
            }
        }
    }

    return indexes;
}
