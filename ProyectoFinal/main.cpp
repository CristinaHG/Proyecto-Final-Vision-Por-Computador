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


int var1 = 1;
cv::Point pos;
int radius = 7;

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        pos.x = x;
        pos.y = y;
        var1 = 0;
    }
}

/*
 * 
 */
int main(int argc, char** argv) {

    cv::Mat im1 = cv::imread("imagenes/playa.jpg");
    cv::Mat im2 = cv::imread("imagenes/estrella-de-mar-equinodermos.png");
    cv::Mat mask = cv::imread("imagenes/bn4.png", cv::IMREAD_GRAYSCALE);
    mask = mask > 128;

    //select the region on the image
    while (var1 == 1) {
        //set the callback function for any mouse event
        cv::setMouseCallback("Video", CallBackFunc, NULL);

        //show the image
        cv::imshow("Video", im1);

        // Wait until user press some key
        cv::waitKey(10);
    }


    im1.convertTo(im1, CV_64FC3);
    im2.convertTo(im2, CV_64FC3);

    vector<cv::Mat> channelsVect;

    cv::Mat result = seamlessClonningNormal(im2, im1, mask);
    cv::imshow("mixed seamless cloning", result);
    cv::waitKey(0);

    return 0;
}

