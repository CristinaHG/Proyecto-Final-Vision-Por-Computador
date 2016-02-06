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


    cv::Mat im1 = cv::imread("imagenes/lisa.png");
    cv::Mat im2 = cv::imread("imagenes/face.png");
    cv::Mat mask = cv::imread("imagenes/face_mask.png", cv::IMREAD_GRAYSCALE);

    mask = mask > 128;

    im1.convertTo(im1, CV_64FC3);
    im2.convertTo(im2, CV_64FC3);


//    cv::Mat result2 = seamlessClonningMixin(im2, im1, mask, cv::Point(47, 57));
    cv::Mat result2 = seamlessClonningNormal(im2, im1, mask, cv::Point(47, 57));

    cv::imshow("mixed seamless cloning", result2);
    cv::imwrite("results/mix_.jpg", result2);
    cv::waitKey(0);

    return 0;
}

