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

    int ncol = 0;
    for (int i = 0; i < mask.rows; i++) {
        for (int j = 0; j < mask.cols; j++) {
            if (mask.at<uchar>(i, j) == 255)
                ncol++;
        }
    }

    cv::Mat solutionV = cv::Mat::zeros(3, ncol, CV_8UC3);
    vector<cv::Mat> rgbDest;
    vector<cv::Mat> rgbSource;

    cv::split(dest, rgbDest);
    cv::split(source, rgbSource);

    int internalPix = 0;
    int sumNred, sumNblue, sumNgreen;

    for (int i = 1; i < source.rows - 1; i++) {
        for (int j = 1; i < source.cols - 1; j++) {
          if (mask.at<uchar>(i, j) != 0){
                internalPix += 1;

            if (mask.at<uchar>(i - 1, j) == 0) {
                solutionV.at<uchar>(0, internalPix) += rgbDest.at(0).at<uchar>(i - 1, j);
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(1).at<uchar>(i - 1, j);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(2).at<uchar>(i - 1, j);
            }

            if (mask.at<uchar>(i, j - 1) == 0) {
                solutionV.at<uchar>(0, internalPix) += rgbDest.at(0).at<uchar>(i, j - 1);
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(1).at<uchar>(i, j - 1);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(2).at<uchar>(i, j - 1);
            }

            if (mask.at<uchar>(i + 1, j) == 0) {
                solutionV.at<uchar>(0, internalPix) += rgbDest.at(0).at<uchar>(i + 1, j);
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(1).at<uchar>(i + 1, j);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(2).at<uchar>(i + 1, j);
            }

            if (mask.at<uchar>(i, j + 1) == 0) {
                solutionV.at<uchar>(0, internalPix) += rgbDest.at(0).at<uchar>(i, j + 1);
                solutionV.at<uchar>(1, internalPix) += rgbDest.at(1).at<uchar>(i, j + 1);
                solutionV.at<uchar>(2, internalPix) += rgbDest.at(2).at<uchar>(i, j + 1);
            }

            sumNred = guidanceVect(rgbSource.at(0), i, j);
            sumNgreen = guidanceVect(rgbSource.at(1), i, j);
            sumNblue = guidanceVect(rgbSource.at(2), i, j);
            

        solutionV.at<uchar>(0, internalPix) += sumNred;
        solutionV.at<uchar>(1, internalPix) += sumNgreen;
        solutionV.at<uchar>(2, internalPix) +=sumNblue;
        }
      }
    }
    return solutionV;
}

int guidanceVect(cv::Mat &sourceChannel, float x, float y) {
    float total = 0;

    float n1 = sourceChannel.at<float>(x, y) - sourceChannel.at<float>(x - 1, y);
    float n2 = sourceChannel.at<float>(x, y) - sourceChannel.at<float>(x + 1, y);
    float n3 = sourceChannel.at<float>(x, y) - sourceChannel.at<float>(x, y - 1);
    float n4 = sourceChannel.at<float>(x, y) - sourceChannel.at<float>(x, y + 1);

    total = n1 + n2 + n3 + n4;
    return total;
}

cv::Mat coefficientMatrix(cv::Mat &source, cv::Mat &dest, cv::Mat &mask, cv::Mat &index) {

    int insidePix = 0;
    int n = 0;

    for (int i = 0; i < mask.rows; i++) {
        for (int j = 0; j < mask.cols; j++) {
            if (mask.at<uchar>(i, j) == 255) // 0 
                n++;
        }
    }

    cv::Mat coeffMatrix = cv::Mat::zeros(n, n, CV_8UC1);

    for (int i = 1; i < source.cols - 1; i++) {
        for (int j = 1; j < source.rows - 1; j++) {
            if (mask.at<uchar>(i, j) != 0) {
                insidePix += 1;
                if (mask.at<uchar>(i - 1, j) != 0) {
                    coeffMatrix.at<uchar>(insidePix, index.at<int>(i - 1, j)) = -1;

                } else if (mask.at<uchar>(i, j - 1) != 0) {
                    coeffMatrix.at<uchar>(insidePix, index.at<int>(i, j - 1)) = -1;

                } else if (mask.at<uchar>(i + 1, j) != 0) {
                    coeffMatrix.at<uchar>(insidePix, index.at<int>(i + 1, j)) = -1;

                } else if (mask.at<uchar>(i, j + 1) != 0) {
                    coeffMatrix.at<uchar>(insidePix, index.at<int>(i, j + 1)) = -1;

                } else {
                    coeffMatrix.at<uchar>(insidePix, insidePix) = 4;
                }
            }
        }
    }
    return coeffMatrix;
}

cv::Mat seamlessClonningNormal(cv::Mat &source, cv::Mat &dest, cv::Mat &mask) {

    int insidePix = 0;
    vector<cv::Mat> DestChannels;
    vector<cv::Mat> SourceChannels;
    for (int i = 0; i < mask.rows; i++) {
        for (int j = 0; j < mask.cols; j++) {
            if (mask.at<uchar>(i, j) == 255)
                insidePix++;
        }
    }


    cv::split(source, SourceChannels);
    cv::split(dest, DestChannels);

    cv::Mat indexes = getIndexes(mask, dest.cols, dest.rows);

    cv::Mat coeffMat = coefficientMatrix(source, dest, mask, indexes);
    cv::Mat solutionVector = solVector(source, dest, mask);

    cv::Mat_<uchar> solR;
    cv::Mat_<uchar> solG;
    cv::Mat_<uchar> solB;

    solR = coeffMat / solutionVector.row(0);
    solG = coeffMat / solutionVector.row(1);
    solB = coeffMat / solutionVector.row(2);

    cv::Mat result;
    result = reconstructImage(solR, solG, solB, mask, dest, indexes);

    return result;
}




//////////////////////////////////////////////////////////////////////

cv::Mat reconstructImage(cv::Mat &r, cv::Mat &g, cv::Mat &b, cv::Mat &mask, cv::Mat &dest, cv::Mat &indexes) {
    vector<cv::Mat> destChannels;

    cv::split(dest, destChannels);

    cv::Mat newR, newG, newB;

    destChannels.at(0).copyTo(newR);
    destChannels.at(1).copyTo(newG);
    destChannels.at(2).copyTo(newB);

    for (int i = 0; i < mask.cols; i++) {
        for (int k = 0; k < mask.rows; k++) {
            if (mask.at<uchar>(i, k) != 0) {
                int index = indexes.at<int>(i, k);
                newR.at<uchar>(i, k) = r.at<uchar>(index);
                newG.at<uchar>(i, k) = g.at<uchar>(index);
                newB.at<uchar>(i, k) = b.at<uchar>(index);
            }
        }
    }
    cv::Mat_<uchar> assembledImage;
    vector<cv::Mat> mv;

    mv.push_back(newR);
    mv.push_back(newG);
    mv.push_back(newB);

    cv::merge(mv, assembledImage);

    return assembledImage;
}

cv::Mat getIndexes(cv::Mat &mask, int cols, int rows) {

    cv::Mat indexes = cv::Mat::zeros(cols, rows, CV_32SC1);

    int insiders = 0;

    for (int i = 0; i < mask.cols; i++) {
        for (int j = 0; j < mask.rows; j++) {
            if (mask.at<uchar>(i, j) != 0) {
                //                LOG_MESSAGE("SIZE OF INDEXES");
                //                LOG_MESSAGE(rows);
                //                LOG_MESSAGE(cols);
                //                LOG_MESSAGE("i,j");
                //                LOG_MESSAGE(i);
                //                LOG_MESSAGE(j);
                //                LOG_MESSAGE(insiders)
                indexes.at<int>(i, j) = ++insiders;
            }
        }
    }
    return indexes;
}
