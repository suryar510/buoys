#include "iostream"
#include <stdio.h>
#include <string>
#include <fstream>
#include <string>

#include "proc.hpp"

using namespace std;
using namespace cv;
Mat kmeans(Mat);
Mat kmeansGray(Mat);
cv::Point brightest(const cv::Mat&);
void print(std::string s, int x, int y){
	std::cout<<s<<" "<<x<<" "<<y<<std::endl;
}
cv::Mat redBuoy(const cv::Mat& in){
	/*
	 * At Competition Pool Uncomment these lines
	 */
		//Mat src = color_illumination_correction(in, 171);
		//Mat alg3 = redFilter(src);
	/*
	 * At AVHS Pool Uncomment these lines
	 */
		Mat alg3 = redFilter(in);

	normalize(alg3,alg3, 0, 255, NORM_MINMAX, CV_8UC1);
	return alg3;
}
cv::Mat yellowBuoy(const cv::Mat& in){
	Mat alg1 = yellowFilter(in);
	normalize(alg1,alg1, 0, 255, NORM_MINMAX, CV_8UC1);
	alg1 = illumination_correction(alg1, 101);
	return alg1;
}
cv::Point brightest(const cv::Mat& in){
	double minVal; 
	double maxVal; 
	Point minLoc; 
	Point maxLoc;
	minMaxLoc( in, &minVal, &maxVal, &minLoc, &maxLoc );
	return maxLoc;
}
int main( int argc, char** argv )
{
	if(argc<2){
		std::cout<<"format: "<<argv[0]<<" [img file]"<<std::endl;
		return 0;
	} 
	string file = argv[1];
	Mat src = imread(file, 1);


	Mat red = redBuoy(src);
	Mat yellow = yellowBuoy(src);
	cv::Point redP = brightest(red);
	cv::Point yellowP = brightest(yellow);

	circle(src, redP, 25, Scalar(0,0,255), 1, 0);
	circle(src, yellowP, 25, Scalar(0,255,255), 1, 0);

	resize(src, src, Size(), 2, 2, INTER_CUBIC);
	imshow("red", red);
	imshow("yellow", yellow);
	imshow("src", src);

	waitKey(0);
	return(0);
}
Mat kmeansGray(Mat in){
	Mat src;
	in.copyTo(src);
    blur(src, src, Size(10,10));

    Mat p = Mat::zeros(src.cols*src.rows, 3, CV_32F);
    Mat bestLabels, centers, clustered;
    Mat gray = src;
    // i think there is a better way to split pixel bgr color
    for(int i=0; i<src.cols*src.rows; i++) {
        p.at<float>(i,0) = (i/src.cols) / src.rows;
        p.at<float>(i,1) = (i%src.cols) / src.cols;
        p.at<float>(i,2) = gray.data[i] / 255.0;
    }

    int K = 8;
    cv::kmeans(p, K, bestLabels,
            TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
            3, KMEANS_PP_CENTERS, centers);

    int colors[K];
    for(int i=0; i<K; i++) {
        colors[i] = 255/(i+1);
    }
    // i think there is a better way to do this mayebe some Mat::reshape?
    clustered = Mat(src.rows, src.cols, CV_32F);
    for(int i=0; i<src.cols*src.rows; i++) {
        clustered.at<float>(i/src.cols, i%src.cols) = (float)(colors[bestLabels.at<int>(0,i)]);
//      cout << bestLabels.at<int>(0,i) << " " <<
//              colors[bestLabels.at<int>(0,i)] << " " <<
//              clustered.at<float>(i/src.cols, i%src.cols) << " " <<
//              endl;
    }

//    clustered.convertTo(clustered, CV_8U);
	return clustered;
}
Mat kmeans(Mat in){
	Mat src = in;
    blur(src, src, Size(10,10));

    Mat p = Mat::zeros(src.cols*src.rows, 5, CV_32F);
    Mat bestLabels, centers, clustered;
    vector<Mat> bgr;
    cv::split(src, bgr);
    // i think there is a better way to split pixel bgr color
    for(int i=0; i<src.cols*src.rows; i++) {
        p.at<float>(i,0) = (i/src.cols) / src.rows;
        p.at<float>(i,1) = (i%src.cols) / src.cols;
        p.at<float>(i,2) = bgr[0].data[i] / 255.0;
        p.at<float>(i,3) = bgr[1].data[i] / 255.0;
        p.at<float>(i,4) = bgr[2].data[i] / 255.0;
    }

    int K = 8;
    cv::kmeans(p, K, bestLabels,
            TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
            3, KMEANS_PP_CENTERS, centers);

    int colors[K];
    for(int i=0; i<K; i++) {
        colors[i] = 255/(i+1);
    }
    // i think there is a better way to do this mayebe some Mat::reshape?
    clustered = Mat(src.rows, src.cols, CV_32F);
    for(int i=0; i<src.cols*src.rows; i++) {
        clustered.at<float>(i/src.cols, i%src.cols) = (float)(colors[bestLabels.at<int>(0,i)]);
//      cout << bestLabels.at<int>(0,i) << " " <<
//              colors[bestLabels.at<int>(0,i)] << " " <<
//              clustered.at<float>(i/src.cols, i%src.cols) << " " <<
//              endl;
    }

//    clustered.convertTo(clustered, CV_8U);
	return clustered;
}
