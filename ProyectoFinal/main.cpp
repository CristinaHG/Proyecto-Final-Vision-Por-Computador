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

    cv::Mat im1 = cv::imread("imagenes/playa.jpg");
    cv::Mat im2 = cv::imread("imagenes/estrella-de-mar-equinodermos.png");
    cv::Mat mask = cv::imread("imagenes/bn4.png", cv::IMREAD_GRAYSCALE);
    mask = mask > 128;

    im1.convertTo(im1, CV_64FC3);
    im2.convertTo(im2, CV_64FC3);

    vector<cv::Mat> channelsVect;

    cv::Mat result = seamlessClonningNormal(im2, im1, mask);
    cv::imshow("mixed seamless cloning", result);
    cv::waitKey(0);

    return 0;
}

