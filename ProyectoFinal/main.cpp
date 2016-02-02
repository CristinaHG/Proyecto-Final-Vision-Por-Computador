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

    cv::Mat im1 = cv::imread("imagenes/188633347_64c691e138_b.jpg");
    cv::Mat im2 = cv::imread("imagenes/Articulo0002080.jpg");

    //    Mat mascara = 255*Mat::ones(im2.rows, im2.cols, im2.depth());
    cv::Mat mask = 255 * Mat::zeros(im2.rows, im2.cols, im2.depth());
    cv::Point poly[1][16];
    poly[0][0] = Point(98, 150);
    poly[0][1] = Point(150, 113);
    poly[0][2] = Point(228, 94);
    poly[0][3] = Point(321, 114);
    poly[0][4] = Point(383, 164);
    poly[0][5] = Point(420, 221);
    poly[0][6] = Point(439, 283);
    poly[0][7] = Point(444, 291);
    poly[0][8] = Point(434, 372);
    poly[0][9] = Point(398, 411);
    poly[0][10] = Point(350, 396);
    poly[0][11] = Point(315, 368);
    poly[0][12] = Point(276, 335);
    poly[0][13] = Point(239, 378);
    poly[0][14] = Point(125, 378);
    poly[0][15] = Point(54, 273);

    const Point * polygons[1] = {poly[0]};
    int num_points[] = {16};

    cv::fillPoly(mask, polygons, num_points, 1, Scalar(255, 255, 255));

    cv::Point center(750, 496);

    cv::Mat output;
    cv::Mat output2;
    cv::seamlessClone(im2, im1, mask, center, output, NORMAL_CLONE);
    cv::seamlessClone(im2, im1, mask, center, output2, MIXED_CLONE);
    cv::imshow("normal seamless cloning", output);
    cv::waitKey(0);
    cv::imshow("mixed seamless cloning", output2);
    cv::waitKey(0);

    //    getRGBMatrix(im1);

    return 0;
}

