/* 
 * File:   main.cpp
 * Author: cris
 *
 * Created on 01 February 2016, 16:01
 */

#include <cstdlib>
#include<opencv2/opencv.hpp>
#include<opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;
/*
 * 
 */
int main(int argc, char** argv) {

    Mat im1 = imread("imagenes/188633347_64c691e138_b.jpg");
    Mat im2 = imread("imagenes/Articulo0002080.jpg");
    
    Mat mascara = Mat::zeros(im2.rows, im2.cols, im2.depth());
    return 0;
}

