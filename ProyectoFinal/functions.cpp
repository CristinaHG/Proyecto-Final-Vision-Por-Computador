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

void getRGBMatrix(Mat &image, vector<Mat> &channels) {

    cv::split(image, channels);

    cout << "NUM CANALES " << channels.size() << endl;
    //    cout<<"RED"<<channels.at<uchar>(0)<<endl;
    //    cout<<"GREEN"<<channels.at<uchar>(1)<<endl;
    //    cout<<"BLUE"<<channels.at<uchar>(2)<<endl;
    //    return channels;
}

Mat solVector(Mat &source, Mat &dest, Mat &mask) {

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
    int sumNred, sumNblue, sumNgreen;

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
            
            sumNred=guidanceVect(rgbSource.at(0),i,j);
            sumNblue=guidanceVect(rgbSource.at(1),i,j);
            sumNgreen=guidanceVect(rgbSource.at(2),i,j);
        }
    
        solutionV.at<uchar>(0,internalPix)+=sumNred;
        solutionV.at<uchar>(1,internalPix)+=sumNblue;
        solutionV.at<uchar>(2,internalPix)+=sumNgreen;
        
        return solutionV;
    }
}

int guidanceVect(Mat &sourceChannel,float x,float y){
    int total=0;
    float n1=sourceChannel(x,y)-sourceChannel(x-1,y);
    float n2=sourceChannel(x,y)-sourceChannel(x+1,y);
    float n3=sourceChannel(x,y)-sourceChannel(x,y-1);
    float n4=sourceChannel(x,y)-sourceChannel(x,y+1);

    total=n1+n2+n3+n4;
    return total;
}

Mat CoefficientMatrix(Mat souce,Mat dest, Mat mask,Mat index){

    int insidePix=0;
    int n=0;
    
    for(int i=0;i<mask.rows; i++){
        for(int j=0; j<mask.cols;j++){
            if(mask.at<uchar>(i,j)==1) // 0 
                n++;
        }
    }
    Mat coeffMatrix=Mat::zeros(n,n,CV_8UC3);
    
    }





//////////////////////////////////////////////////////////////////////

//function resultImg = reconstructImg(red, green, blue, mask, destination, indexes)
//
//[destinationR destinationG destinationB] = getRGB(destination);
//[destinationW destinationH channels] = size(destination);
//
//newR = destinationR;
//newG = destinationG;
//newB = destinationB;
//
//[maskW maskH c] = size(mask);
//
//for x = 1:maskW
//    for y = 1:maskH
//        if mask(x, y) ~= 0
//            index = indexes(x, y);
//            newR(x, y) = red(index);
//            newG(x, y) = green(index);
//            newB(x, y) = blue(index);
//        end
//    end
//end
//
//resultImg = buildImg(newR, newG, newB);
//    
//end

cv::Mat reconstructImage(cv::Mat&,)