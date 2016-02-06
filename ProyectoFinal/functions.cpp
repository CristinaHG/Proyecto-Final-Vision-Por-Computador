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

cv::Mat solVector(const cv::Mat &source, const cv::Mat &dest, const cv::Mat &mask, bool mixin, int nonz) {

    cv::Mat solutionV = cv::Mat::zeros(3, nonz, CV_64FC1);

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

                if (mixin) {
                    sumNred = guidanceVectMixin(rgbSource.at(2), rgbDest.at(2), i, j);
                    sumNgreen = guidanceVectMixin(rgbSource.at(1), rgbDest.at(1), i, j);
                    sumNblue = guidanceVectMixin(rgbSource.at(0), rgbDest.at(0), i, j);
                } else {
                    sumNred = guidanceVect(rgbSource.at(2), i, j);
                    sumNgreen = guidanceVect(rgbSource.at(1), i, j);
                    sumNblue = guidanceVect(rgbSource.at(0), i, j);
                }

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

double guidanceVectMixin(cv::Mat &sourceChannel, cv::Mat &destChannel, int x, int y) {

    double n1, n2, n3, n4;

    if (abs(sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x - 1, y)) > abs(destChannel.at<double>(x, y) - destChannel.at<double>(x - 1, y)))
        n1 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x - 1, y);
    else
        n1 = destChannel.at<double>(x, y) - destChannel.at<double>(x - 1, y);
    if (abs(sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x + 1, y)) > abs(destChannel.at<double>(x, y) - destChannel.at<double>(x + 1, y)))
        n2 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x + 1, y);
    else
        n2 = destChannel.at<double>(x, y) - destChannel.at<double>(x + 1, y);
    if (abs(sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x, y - 1)) > abs(destChannel.at<double>(x, y) - destChannel.at<double>(x, y - 1)))
        n3 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x, y - 1);
    else
        n3 = destChannel.at<double>(x, y) - destChannel.at<double>(x, y - 1);
    if (abs(sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x, y + 1)) > abs(destChannel.at<double>(x, y) - destChannel.at<double>(x, y + 1)))
        n4 = sourceChannel.at<double>(x, y) - sourceChannel.at<double>(x, y + 1);
    else
        n4 = destChannel.at<double>(x, y) - destChannel.at<double>(x, y + 1);

    return n1 + n2 + n3 + n4;
}

cv::Mat coefficientMatrix(const cv::Size &sz, const cv::Mat &mask, const cv::Mat &index, int nonz) {

    int insidePix = 0;

    //        int size[] = {n, n};
    //        cv::SparseMat coeffMatrix2 = cv::SparseMat(2, size, CV_64FC1);
    cv::Mat coeffMatrix = cv::Mat::zeros(nonz, nonz, CV_64FC1);

    for (int i = 1; i < sz.width - 1; i++) {
        for (int j = 1; j < sz.height - 1; j++) {
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

cv::Mat seamlessClonningNormal(cv::Mat &source, cv::Mat &dest, cv::Mat &mask, cv::Point p) {


    vector<cv::Mat> DestChannels;
    vector<cv::Mat> SourceChannels;

    cv::split(source, SourceChannels);
    cv::split(dest, DestChannels);

    cv::Mat indexes = getIndexes(mask, dest.cols, dest.rows);

    int nonzero = cv::countNonZero(mask);

    cv::Mat coeffMat = coefficientMatrix(source.size(), mask, indexes, nonzero);
    cv::Mat solutionVector = solVector(source, dest, mask, false, nonzero);

    cv::Mat solR;
    cv::Mat solG;
    cv::Mat solB;

    cv::solve(coeffMat, solutionVector.row(2).t(), solR, cv::DECOMP_CHOLESKY);
    cv::solve(coeffMat, solutionVector.row(1).t(), solG, cv::DECOMP_CHOLESKY);
    cv::solve(coeffMat, solutionVector.row(0).t(), solB, cv::DECOMP_CHOLESKY);

    cv::Mat result = reconstructImage(solR, solG, solB, mask, dest, indexes, p);

    return result;
}

cv::Mat seamlessClonningMixin(cv::Mat &source, cv::Mat &dest, cv::Mat &mask, cv::Point p) {

    cv::Mat indexes = getIndexes(mask, dest.cols, dest.rows);
    
    int nonzero = cv::countNonZero(mask);

    cv::Mat coeffMat = coefficientMatrix(source.size(), mask, indexes, nonzero);
    cv::Mat solutionVector = solVector(source, dest, mask, true, nonzero);

    cv::Mat solR;
    cv::Mat solG;
    cv::Mat solB;

    cv::solve(coeffMat, solutionVector.row(2).t(), solR, cv::DECOMP_LU | cv::DECOMP_CHOLESKY);
    cv::solve(coeffMat, solutionVector.row(1).t(), solG, cv::DECOMP_LU | cv::DECOMP_CHOLESKY);
    cv::solve(coeffMat, solutionVector.row(0).t(), solB, cv::DECOMP_LU | cv::DECOMP_CHOLESKY);

    cv::Mat result = reconstructImage(solR, solG, solB, mask, dest, indexes, p);

    return result;
}

//////////////////////////////////////////////////////////////////////

cv::Mat reconstructImage(cv::Mat &r, cv::Mat &g, cv::Mat &b, const cv::Mat &mask, cv::Mat &dest, const cv::Mat &indexes, const cv::Point &p) {


    r.convertTo(r, CV_8UC1);
    g.convertTo(g, CV_8UC1);
    b.convertTo(b, CV_8UC1);
    dest.convertTo(dest, CV_8UC3);

    vector<cv::Mat> destChannels;


    // TODO Add assert to verify not overflow image
    int minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
    for (int i = 0; i < mask.size().height; i++) {
        for (int j = 0; j < mask.size().width; j++) {
            if (mask.at<uchar>(i, j) == 255) {
                minx = std::min(minx, i);
                maxx = std::max(maxx, i);
                miny = std::min(miny, j);
                maxy = std::max(maxy, j);
            }
        }
    }

    int lenx = maxx - minx;
    int leny = maxy - miny;

    int minxd = p.y - lenx / 2;
    int maxxd = p.y + lenx / 2;
    int minyd = p.x - leny / 2;
    int maxyd = p.x + leny / 2;

    CV_Assert(minxd >= 0 && minyd >= 0 && maxxd <= dest.rows && maxyd <= dest.cols);

    cv::Rect roi = cv::Rect(minyd, minxd, leny, lenx);

    cv::split(dest, destChannels);

    cv::Mat newR = destChannels.at(2);
    cv::Mat newG = destChannels.at(1);
    cv::Mat newB = destChannels.at(0);

    cv::Mat newRroi = newR(roi);
    cv::Mat newGroi = newG(roi);
    cv::Mat newBroi = newB(roi);

    for (int i = 0; i < mask.cols; i++) {
        for (int k = 0; k < mask.rows; k++) {
            if (mask.at<uchar>(i, k) != 0) {
                int index = indexes.at<double>(i, k);
                newRroi.at<uchar>(i, k) = r.at<uchar>(index);
                newGroi.at<uchar>(i, k) = g.at<uchar>(index);
                newBroi.at<uchar>(i, k) = b.at<uchar>(index);
            }
        }
    }

    cv::Mat assembledImage = cv::Mat(dest.size(), dest.type());

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
                indexes.at<double>(i, j) = insiders;
                ++insiders;
            }
        }
    }

    return indexes;
}
