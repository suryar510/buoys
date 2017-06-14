#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>
#include <cmath>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <string>
using namespace std;
using namespace cv;
Mat equalize(Mat);
Mat filterRed(Mat);
Mat removeWhite(Mat);
Mat kmeans(Mat);
Mat filter2(Mat);
Mat filter3(Mat);
Mat addTogether(vector<Mat>);
Point getBrightest(Mat);

int main( int argc, char** argv )
{
	if(argc<2){
		std::cout<<"format: "<<argv[0]<<" [img file]"<<std::endl;
		return 0;
	} 
//	Mat s = equalize(src);
//	resize(s, s, Size(), 2, 2, INTER_CUBIC);
//	Mat output = equalize(src);
	string file = argv[1];
	Mat src = imread(file, 1);
	imshow("original", src);
	Mat temp = src;

//	Mat dst = (equalize(src));
//	cvtColor(dst, dst, COLOR_BGR2GRAY);
//	imshow("new", src);
	Mat alg3 = filter3(src);
//	equalizeHist(alg1,alg1);
	normalize(alg3,alg3, 0, 255, NORM_MINMAX, CV_8UC1);
//	std::cout<<getBrightestX(alg3).x<<" "<<getBrightest(alg3).y<<std::endl;
	double minVal; 
	double maxVal; 
	Point minLoc; 
	Point maxLoc;

	minMaxLoc( alg3, &minVal, &maxVal, &minLoc, &maxLoc );
	std::cout<<maxLoc<<std::endl;
	circle(alg3, maxLoc, 25, Scalar(255,255,255), 1, 0);
	imshow("alg3", alg3);
//	Mat alg1 = filter2(src);
//	equalizeHist(alg1,alg1);
//	normalize(alg1,alg1, 0, 255, NORM_MINMAX, CV_8UC1);
//	imshow("alg2", alg1);

//	Mat output = kmeans((temp));
//	imwrite("processed.jpg", output);
//	namedWindow("finish", CV_WINDOW_AUTOSIZE);
//	imshow("finish", output);

	waitKey(0);
	return(0);
}
Mat filter2(Mat in){
	Mat lab,ycrcb,hsv,lab_split[3];
	cvtColor(in,lab, COLOR_BGR2Lab);
//	cvtColor(in,ycrcb, COLOR_BGR2YCrCb);
	split(lab, lab_split);
	return lab_split[2];
}
Mat filter3(Mat in){
	Mat ycrcb,hsv,ycrcb_split[3];
	cvtColor(in,ycrcb, COLOR_BGR2YCrCb);
	split(ycrcb, ycrcb_split);
	return ycrcb_split[1];
}
Point getBrightest(Mat img){
	int brightest=0;
	int x=0;
	int y=0;
	unsigned char *in = (unsigned char*)(img.data);
	for(int i=0; i<img.rows; i++){
		for(int j=0; j<img.cols; j++){
 			int temp = in[img.cols * j + i ] ;
			if(temp>brightest){
				brightest = temp;
				x = j;
				y = i;
			}
		}
	}
	return Point(x,y);
}
Mat filter(Mat in){
	
	Mat gray,blurred,thresh;
	cvtColor(in,gray,  COLOR_BGR2GRAY);
	/*
	Mat blurred = GaussianBlur(gray, (11,11),0);
	Mat thresh = threshold(blurred, 200, 255, THRESH_BINARY)[1];

	erode(thresh, thresh, 
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
blurred = cv2.GaussianBlur(gray, (11, 11), 0)
thresh = cv2.threshold(blurred, 200, 255, cv2.THRESH_BINARY)[1]

thresh = cv2.erode(thresh, None, iterations=2)
thresh = cv2.dilate(thresh, None, iterations=4)
*/
}
Mat filterRed(Mat in){
	Mat bgr[3];
	split(in, bgr);
	return bgr[2];
}
Mat addTogether(vector<Mat> vec){
	//rows = y
	//cols = x
	Mat out(vec.at(0).rows*vec.size(), vec.at(0).cols, CV_8UC3,Scalar::all(0) );

	std::cout<<out.cols<< " "<<out.rows<<std::endl;
	std::cout<<vec.size()<<std::endl;
	for(int x=0;x<vec.size();x++){
		Mat in = vec.at(x);
		std::cout<<"beg: "<<in.rows<<" "<<in.cols<<std::endl;
		for(int i=0; i<in.rows; i++){
			for(int j=0; j<in.cols; j++){
				Vec3b color = in.at<Vec3b>(Point(j,i));
				out.at<Vec3b>(Point(j,i)) = color;
//				std::cout<<i+x*in.rows<<" "<<j<<std::endl;
			}
		}
		std::cout<<"end: "<<in.rows<<" "<<in.cols<<std::endl;
	}
	return out;
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

    clustered.convertTo(clustered, CV_8U);
	return clustered;
}
Mat removeWhite(Mat in){
	Mat out;
	in.copyTo(out);
	for(int i=0; i<in.rows; i++){
		for(int j=0; j<in.cols; j++){
			Vec3b color = in.at<Vec3b>(Point(i,j));
			int b = color[0];
			int g = in.at<cv::Vec3b>(i,j)[1];
			int r = in.at<cv::Vec3b>(i,j)[2];
			if(r>150 && b>100 && g>100){
				color[0]=0;
				color[1]=0;
				color[2]=0;
      			out.at<Vec3b>(Point(i,j)) = color;
			}
		}
	}
	return out;
}
Mat equalize(Mat in){
	Mat bgr[3],dst,hsv[3];
	split(in, bgr);
//	equalizeHist(bgr[0],bgr[0]);
//	equalizeHist(bgr[1],bgr[1]);
	normalize(bgr[2],bgr[2], 0, 255, NORM_MINMAX, CV_8UC1);

	merge(bgr, 3, dst);
	cvtColor(dst, dst, CV_BGR2HSV);
	split(dst, hsv);
	equalizeHist(hsv[1], hsv[1]);
	equalizeHist(hsv[2], hsv[2]);
	merge(hsv, 3, dst);
	cvtColor(dst, dst, CV_HSV2BGR);
	return dst;
}
Mat equalize2(Mat in){
	Mat bgr[3],dst,hsv[3];
//	equalizeHist(in,in);
	normalize(in,in, 0, 255, NORM_MINMAX, CV_8UC1);

//	merge(bgr, 3, dst);
	cvtColor(dst, dst, CV_BGR2HSV);
	split(dst, hsv);
	equalizeHist(hsv[1], hsv[1]);
	equalizeHist(hsv[2], hsv[2]);
	merge(hsv, 3, dst);
	cvtColor(dst, dst, CV_HSV2BGR);
	return dst;
}
