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
using namespace cv;


Point point1, point2; /* vertical points of the bounding box */
int drag = 0;
Rect rect; /* bounding box */
Mat img, roiImg; /* roiImg - the part of the image in the bounding box */
int select_flag = 0;

void mouseHandler(int event, int x, int y, int flags, void* param) {
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        /* left button clicked. ROI selection begins */
        point1 = Point(x, y);
        drag = 1;
    }

    if (event == CV_EVENT_MOUSEMOVE && drag) {
        /* mouse dragged. ROI being selected */
        Mat img1 = img.clone();
        point2 = Point(x, y);
        rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 3, 8, 0);
        imshow("image", img1);
    }

    if (event == CV_EVENT_LBUTTONUP && drag) {
        point2 = Point(x, y);
        rect = Rect(point1.x, point1.y, x - point1.x, y - point1.y);
        drag = 0;
        roiImg = img(rect);
    }

    if (event == CV_EVENT_LBUTTONUP) {
        /* ROI selected */
        select_flag = 1;
        drag = 0;
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


    int k;
    imshow("image", im1);
    while (1) {
        cap >> img;
        cvSetMouseCallback("image", mouseHandler, NULL);
        if (select_flag == 1) {
            imshow("ROI", roiImg); /* show the image bounded by the box */
        }
        rectangle(img, rect, CV_RGB(255, 0, 0), 3, 8, 0);
        imshow("image", img);
        k = waitKey(10);
        if (k == 27) {
            break;
        }
    }


    im1.convertTo(im1, CV_64FC3);
    im2.convertTo(im2, CV_64FC3);

    vector<cv::Mat> channelsVect;

    cv::Mat result = seamlessClonningNormal(im2, im1, mask);
    cv::imshow("mixed seamless cloning", result);
    cv::waitKey(0);

    return 0;
}

