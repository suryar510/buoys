#include <stdio.h>
#include <string>
#include <fstream>
#include <string>
#include "iostream"
#include "proc.hpp"

cv::Mat redBuoy(const cv::Mat&);
cv::Mat greenBuoy(const cv::Mat&);
cv::Mat yellowBuoy(const cv::Mat&);

cv::Point brightest(const cv::Mat&);

bool checkYellow(cv::Point, const cv::Mat&, int);
bool checkGreen(cv::Point, const cv::Mat&, int);
bool checkRed(cv::Point, const cv::Mat&, int);
