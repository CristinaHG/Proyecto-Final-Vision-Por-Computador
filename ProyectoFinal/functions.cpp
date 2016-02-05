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

    cv::Mat solutionV = cv::Mat::zeros(3, ncol, CV_64FC1);
    vector<cv::Mat> rgbDest;
    vector<cv::Mat> rgbSource;

    cv::split(dest, rgbDest);
    cv::split(source, rgbSource);

    int internalPix = 0;
    int sumNred, sumNblue, sumNgreen;

    for (int i = 1; i < source.cols - 1; i++) {
        for (int j = 1; j < source.rows - 1; j++) {
            if (mask.at<uchar>(i, j) != 0) {
                if (mask.at<uchar>(i - 1, j) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(2).at<double>(i - 1, j); // B
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<double>(i - 1, j); // G
                    solutionV.at<double>(2, internalPix) += rgbDest.at(0).at<double>(i - 1, j); // R
                }

                if (mask.at<uchar>(i, j - 1) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(2).at<double>(i, j - 1);
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<double>(i, j - 1);
                    solutionV.at<double>(2, internalPix) += rgbDest.at(0).at<double>(i, j - 1);
                }

                if (mask.at<uchar>(i + 1, j) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(2).at<double>(i + 1, j);
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<double>(i + 1, j);
                    solutionV.at<double>(2, internalPix) += rgbDest.at(0).at<double>(i + 1, j);
                }

                if (mask.at<uchar>(i, j + 1) == 0) {
                    solutionV.at<double>(0, internalPix) += rgbDest.at(2).at<double>(i, j + 1);
                    solutionV.at<double>(1, internalPix) += rgbDest.at(1).at<double>(i, j + 1);
                    solutionV.at<double>(2, internalPix) += rgbDest.at(0).at<double>(i, j + 1);
                }

                sumNred = guidanceVect(rgbSource.at(2), i, j);
                sumNgreen = guidanceVect(rgbSource.at(1), i, j);
                sumNblue = guidanceVect(rgbSource.at(0), i, j);

                solutionV.at<double>(0, internalPix) += sumNblue;
                solutionV.at<double>(1, internalPix) += sumNgreen;
                solutionV.at<double>(2, internalPix) += sumNred;
                internalPix += 1;
            }
        }
    }
    return solutionV;
}

double guidanceVect(cv::Mat &sourceChannel, int x, int y) {

    double n1 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x - 1, y);
    double n2 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x + 1, y);
    double n3 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x, y - 1);
    double n4 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x, y + 1);

    return n1 + n2 + n3 + n4;
}

cv::Mat coefficientMatrix(cv::Mat &source, cv::Mat &dest, cv::Mat &mask, cv::Mat &index) {

    int insidePix = 0;

    int n = cv::countNonZero(mask);

    //    int size[] = {n, n};
    //    cv::SparseMat coeffMatrix = cv::SparseMat(2, size, CV_64FC1);
    cv::Mat coeffMatrix = cv::Mat::zeros(n, n, CV_64FC1);

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

    cv::invert(coeffMat, coeffMat);

    solR = coeffMat * solutionVector.row(2).t();
    solG = coeffMat * solutionVector.row(1).t();
    solB = coeffMat * solutionVector.row(0).t();

    //    cv::solve(coeffMat, solutionVector.row(0).t(), solR);
    //    cv::solve(coeffMat, solutionVector.row(1).t(), solG);
    //    cv::solve(coeffMat, solutionVector.row(2).t(), solB);

    cv::Mat result = reconstructImage(solR, solG, solB, mask, dest, indexes);

    return result;
}




//////////////////////////////////////////////////////////////////////

cv::Mat reconstructImage(cv::Mat &r, cv::Mat &g, cv::Mat &b, cv::Mat &mask, cv::Mat &dest, cv::Mat &indexes) {

    r.convertTo(r, CV_8UC1);
    g.convertTo(g, CV_8UC1);
    b.convertTo(b, CV_8UC1);
    dest.convertTo(dest, CV_8UC3);
    
    
    vector<cv::Mat> destChannels;

    cv::split(dest, destChannels);

    cv::Mat newR, newG, newB;

    destChannels.at(2).copyTo(newR);
    destChannels.at(1).copyTo(newG);
    destChannels.at(0).copyTo(newB);

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
    vector<cv::Mat_<double> > mv;

    mv.push_back(newB);
    mv.push_back(newG);
    mv.push_back(newR);

    cv::merge(mv, assembledImage);

    assembledImage.convertTo(assembledImage, CV_8UC3);

    return assembledImage;
}

cv::Mat getIndexes(cv::Mat &mask, int cols, int rows) {

    cv::Mat indexes = cv::Mat::zeros(cols, rows, CV_64FC1);

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
