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
cv::Point3d center(const cv::Mat& in, const cv::Point& p){
	//Uses floodFill algorithm to find box around brightest pixels
	cv::Rect r; 
	float size = std::sqrt(cv::floodFill(in, cv::Point(p.x, p.y), cv::Scalar(255), &r, cv::Scalar(10), cv::Scalar(10), 4) * 4/M_PI);
	cv::Point3d centerP = cv::Point3d(r.x + r.width/2, r.y + r.height/2, (int) size);
	// the z component is radius
	return centerP;
}
cv::Point3d findGreen(const cv::Mat& in, cv::Point3d& redP, cv::Point3d& yellowP){
	int mid =(int) ( yellowP.y+redP.y)/2;
	int midSize = (int) (yellowP.z+redP.z)/2;
	
	int greenRectY = mid - 3*midSize;
	int greenRectHeight = midSize*6;

	//this doesn't work? cuz it sometimes gives errors	
	if(greenRectY<0)
		greenRectY = 0;
	if(greenRectY>in.rows)
		greenRectY = in.rows-1;
	if(greenRectHeight > in.rows)
		greenRectHeight = in.rows-1;

	cv::Rect greenRange = cv::Rect(yellowP.x, mid-3*midSize, redP.x-yellowP.x, midSize*6);
	cv::Point greenBright = brightest(in(greenRange));
	cv::Point greenPrelim = cv::Point(greenBright.x+greenRange.x, greenBright.y+greenRange.y);
	cv::Point3d greenP = center(in, greenPrelim);
	return greenP;
}
const float cropx = 1.0;
const float cropy = 0.6;
const float offset = 0.0 * (1 - cropy);
const float scalex = 512;
const float scaley = 240;
int main( int argc, char** argv )
{
	if(argc<2){
		std::cout<<"format: "<<argv[0]<<" [img file]"<<std::endl;
		return 0;
	} 
	std::string file = argv[1];
	cv::Mat image = cv::imread(file, 1);
	
	//Crop and resize image
/*	cv::resize(image(cv::Rect(image.cols*(1-cropx)/2, image.rows*(1-cropy-offset)/2, 
		image.cols*cropx, image.rows*cropy)), image, 
		cv::Size(cropx*scalex, cropy*scaley));
*/
	cv::Mat src;
	image.copyTo(src);

	// Apply Filters to the Image
	cv::Mat red = redBuoy(src);
	cv::Mat yellow = yellowBuoy(src);
	cv::Mat green = greenBuoy(src);

	//Find the brightest point from each Mat
	cv::Point redBright = brightest(red);
	cv::Point yellowBright = brightest(yellow);
	
	//find the center of those points
	cv::Point3d redP = center(red, redBright);
	cv::Point3d yellowP = center(yellow, yellowBright);

	cv::Point3d greenP = findGreen(green, redP, yellowP);

	/*
	 * check validity of each point and draw it on Mat if true
	 * the checking function is meh so I commented it out
	 */
//	if(checkRed(redP, src, 175))
		cv::circle(src, cv::Point(redP.x, redP.y), redP.z*2, cv::Scalar(0,0,255), 1, 0);
//	if(checkYellow(yellowP, src, 100))
		cv::circle(src, cv::Point(yellowP.x, yellowP.y), yellowP.z*2, cv::Scalar(0,255,255), 1, 0);
//	if(checkGreen(greenP, src, 75))
		cv::circle(src, cv::Point(greenP.x, greenP.y), greenP.z*2, cv::Scalar(0,255,0), 1, 0);

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
