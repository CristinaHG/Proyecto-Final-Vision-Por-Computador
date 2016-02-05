/* 
 * File:   main.cpp
 * Author: cris
 *
 * Created on 01 February 2016, 16:01
 */

#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "functions.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    cv::Mat im1 = cv::imread("imagenes/11260892.jpg");
    cv::Mat im2 = cv::imread("imagenes/estreallae.png");
    cv::Mat mask = cv::imread("imagenes/bn.png", cv::IMREAD_GRAYSCALE);
    mask = mask > 128;

//    im1.convertTo(im1, CV_64FC3);
//    im2.convertTo(im2, CV_64FC3);
//
//    vector<cv::Mat> channelsVect;
////
//    cv::Mat result = seamlessClonningNormal(im2, im1, mask);
//    cv::imshow("seamless cloning", result);
////    cv::imwrite("results/nubebn1.jpg",result);
//    cv::waitKey(0);

    im1.convertTo(im1, CV_64FC3);
    im2.convertTo(im2, CV_64FC3);
    
    cv::Mat result2 = seamlessClonningMixin(im2, im1, mask);
    cv::imshow("mixed seamless cloning", result2);
    cv::imwrite("results/desertnube.jpg",result2);
    cv::waitKey(0);

    return 0;
}

