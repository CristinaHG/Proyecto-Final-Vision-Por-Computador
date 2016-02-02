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

Mat CoefficientMatrix(Mat source,Mat dest, Mat mask,Mat index){

    int insidePix=0;
    int n=0;
    
    for(int i=0;i<mask.rows; i++){
        for(int j=0; j<mask.cols;j++){
            if(mask.at<uchar>(i,j)==1) // 0 
                n++;
        }
    }
    Mat coeffMatrix=Mat::zeros(n,n,CV_8UC3);
    
    for(int i=1; i<source.rows-1;i++){
        for(int j=1; j<source.cols-1;j++){
            if(mask.at<uchar>(i,j)!=0){
                insidePix+=1;
            
            }else if(mask.at<uchar>(i-1,j)!=0){
                coeffMatrix.at<uchar>(i-1,j)=-1;
            
            }else if(mask.at<uchar>(i,j-1)!=0){
                coeffMatrix.at<uchar>(i,j-1)=-1;
                
            }else if(mask.at<uchar>(i+1,j)!=0){
                coeffMatrix.at<uchar>(i+1,j)=-1;
            
            }else if (mask.at<uchar>(i,j+1)!=0){
                coeffMatrix.at<uchar>(i,j+1)=-1;
            
            }else{
            coeffMatrix.at<uchar>(insidePix,insidePix)=4;
            }       
        }
    }
    return coeffMatrix;
}





//////////////////////////////////////////////////////////////////////

cv::Mat reconstructImage(cv::Mat &r, cv::Mat &g, cv::Mat &b, cv::Mat &mask, cv::Mat &dest, cv::Mat &indexes){
    vector<cv::Mat> destChannels;
    
    cv::split(dest, destChannels);
    
    int destRows = dest.rows; 
    int destCols = dest.cols;
    int destCh = dest.channels();
    int maskRows = mask.rows; 
    int maskCols = mask.cols;
    int maskCh = mask.channels();
    
    cv::Mat newR, newG, newB;
    
    destChannels.at(0).copyTo(newR);
    destChannels.at(1).copyTo(newG);
    destChannels.at(2).copyTo(newB);
    
    for (int i = 0; i < mask.cols; i++) {
        for (int k = 0; k < mask.rows; k++) {
            if (mask.at<uchar>(i,k) != 0){
                int index = indexes.at<int>(i, k);
                newR.at(i, k) = r.at<uchar>(index);
                newG.at(i, k) = g.at<uchar>(index);
                newB.at(i, k) = b.at<uchar>(index);
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