#include "buoy.hpp"

cv::Mat redBuoy(const cv::Mat& in){
	cv::Mat src;
	in.copyTo(src);
	//true when at Competition Pool
	//false when at AVHS Pool
	if(true)
		src = color_illumination_correction(in, 171);
	cv::Mat red = redFilter(src);
	cv::normalize(red,red, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	red = erodeMat(red);
	cv::fastNlMeansDenoising(red, red, 3, 7, 21 );
	return red;
}
cv::Mat greenBuoy(const cv::Mat& in){
	//Doesn't work very well
	cv::Mat base = redBuoy(in);
	cv::Mat green;
	cv::bitwise_not(base,green);
	cv::normalize(green,green, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	return green;
}
cv::Mat yellowBuoy(const cv::Mat& in){
	cv::Mat yellow = yellowFilter(in);
	cv::normalize(yellow, yellow, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	yellow = illumination_correction(yellow, 101);
	return yellow;
}
cv::Point brightest(const cv::Mat& in){
	/*
	 * TODO needs to be improved
	 * Currently finds only brightest pixel instead of brightest blob
	 */
	double minVal; 
	double maxVal; 
	cv::Point minLoc; 
	cv::Point maxLoc;
	cv::minMaxLoc( in, &minVal, &maxVal, &minLoc, &maxLoc );
	return maxLoc;
}
int main( int argc, char** argv )
{
	if(argc<2){
		std::cout<<"format: "<<argv[0]<<" [img file]"<<std::endl;
		return 0;
	} 
	std::string file = argv[1];
	cv::Mat src = cv::imread(file, 1);

	// Apply Filters to the Image
	cv::Mat red = redBuoy(src);
	cv::Mat yellow = yellowBuoy(src);
	cv::Mat green = greenBuoy(src);
//	imshow("green", green);
//	imshow("red", red);
//	imshow("yellow", yellow);

	//Find the brightest point from each Mat
	cv::Point redP = brightest(red);
	cv::Point yellowP = brightest(yellow);
	cv::Point greenP = brightest(green);

	//check validity of each point and draw it on Mat if true
	if(checkRed(redP, src, 175))
		cv::circle(src, redP, 25, cv::Scalar(0,0,255), 1, 0);
	if(checkYellow(yellowP, src, 100))
		cv::circle(src, yellowP, 25, cv::Scalar(0,255,255), 1, 0);
	if(checkGreen(greenP, src, 75))
		cv::circle(src, greenP, 25, cv::Scalar(0,255,0), 1, 0);

	cv::resize(src, src, cv::Size(), 2, 2, cv::INTER_CUBIC);
	cv::imshow("src", src);

	cv::waitKey(0);
	return(0);
}
/*
 * These test the validity of the buoy
 * If false, currently we have no backup
 */
bool checkYellow(cv::Point p, const cv::Mat& in, int thresh){
	int red = in.at<cv::Vec3b>(p.y, p.x)[2];
	int green = in.at<cv::Vec3b>(p.y, p.x)[1];
	int blue = in.at<cv::Vec3b>(p.y, p.x)[0];
	std::cout<<blue<<" "<<green<<" "<<red<<'\n';
	if((abs(red-green))<thresh)
		return true;
	return false;
}
bool checkGreen(cv::Point p, const cv::Mat& in, int thresh){
	int red = in.at<cv::Vec3b>(p.y, p.x)[2];
	int green = in.at<cv::Vec3b>(p.y, p.x)[1];
	int blue = in.at<cv::Vec3b>(p.y, p.x)[0];
	if((abs(green-blue)>thresh) )
		return true;
	return true;
}
bool checkRed(cv::Point p, const cv::Mat& in, int thresh){
	int red = in.at<cv::Vec3b>(p.y, p.x)[2];
	int green = in.at<cv::Vec3b>(p.y, p.x)[1];
	int blue = in.at<cv::Vec3b>(p.y, p.x)[0];
	if(red>thresh && (abs(red-green)>50) && (abs(red-blue)>50) )
		return true;
	return true;
}
