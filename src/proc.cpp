#include "proc.hpp"

cv::Mat yellowFilter(const cv::Mat& in){
	//Yellow
	cv::Mat lab,lab_split[3];
	cv::cvtColor(in,lab, cv::COLOR_BGR2Lab);
	cv::split(lab, lab_split);
	return lab_split[2];
}
cv::Mat illumination_correction(cv::Mat const &correction_src, float blur_kernel_size)
{
	cv::Mat blurred_img, diff_img, corrected_img;
	cv::Scalar mean_val;
	cv::GaussianBlur(correction_src, blurred_img, cv::Size(blur_kernel_size, blur_kernel_size), 0, 0); 
	diff_img = correction_src - blurred_img;
	mean_val = mean(blurred_img);
	std::cout <<"mean_val: " << mean_val << "\n";
	corrected_img = diff_img + cv::sum(mean_val)[0];
	return corrected_img;
}
cv::Mat color_illumination_correction(cv::Mat const &correction_src, float blur_kernel_size)
{
        //define 4 variables: blurred image, finished image, and grey illumination image
        cv::Mat blurred_img, corrected_img, grey_img;
        //vector to split color image into later
        std::vector<cv::Mat> split_img(3);
         
        //this works better by using a combination of all three channels to provide a better map of illumination 
        cv::cvtColor(correction_src, grey_img, CV_BGR2GRAY);
         
        //blur to remove clear features and leave only general brightness and darkness behind 
        cv::GaussianBlur(grey_img, blurred_img, cv::Size(blur_kernel_size, blur_kernel_size), 0, 0); 
            
        split(correction_src, split_img);
            
        for(int i = 0; i < 3; i++){
                cv::Mat diff_img = split_img[i] - blurred_img;
                cv::Scalar mean_val = mean(blurred_img);
                split_img[i] = diff_img + mean_val;
        }   
            
        cv::merge(split_img, corrected_img); 
            
        return corrected_img;
}           
cv::Point brightestRed(const cv::Mat& in, const cv::Mat& color, int thresh){
	int x, y, brightest;
	for(int i=0; i<in.rows; i++){
    	for(int j=0; j<in.cols; j++){
			int f = in.at<uchar>(i,j);
			if(f>brightest){
				x = j;
				y = i;
				brightest = f;
			}
		}
	}
	int r = color.at<cv::Vec3b>(y,x)[2];
	if(r<thresh){
		std::vector<cv::Point> bad = {cv::Point(x,y)};
		return brightestRedSmall(in, color, thresh, bad); 
	}
	return cv::Point(x,y);
}
cv::Point brightestRedSmall(const cv::Mat& in, const cv::Mat& color, int thresh, std::vector<cv::Point>& exc){
	int x, y, brightest;
	for(int i=0; i<in.rows; i++){
    	for(int j=0; j<in.cols; j++){
			int f = in.at<uchar>(i,j);
			if(f>brightest && possible(cv::Point(j,i), exc)){
				x = j;
				y = i;
				brightest = f;
			}
		}
	}
	int r = color.at<cv::Vec3b>(y,x)[2];
	if(r<thresh){
		exc.push_back(cv::Point(y,x));
		return brightestRedSmall(in, color, thresh, exc); 
	}
	return cv::Point(x,y);
}
bool possible(cv::Point p, const std::vector<cv::Point> & exc){
	for(auto a : exc){
		if(p.x == a.x && p.y == a.y)
			return true;
	}
	return false;
}
cv::Mat greenFilter(const cv::Mat& in){
	//Yellow
	cv::Mat lab,lab_split[3];
	cv::cvtColor(in,lab, cv::COLOR_BGR2HSV);
	cv::split(lab, lab_split);
	return lab_split[1];
}

cv::Mat redFilter(const cv::Mat& in){
	//Red
	cv::Mat ycrcb,ycrcb_split[3];
	cv::cvtColor(in,ycrcb, cv::COLOR_BGR2YCrCb);
	cv::split(ycrcb, ycrcb_split);
	return ycrcb_split[1];
}
template <typename T>
void print(std::vector<T> const& in){
	for(auto val : in)
		std::cout<< val << " ";
	std::cout<<'\n';
}
template <typename T>
inline T trunc(T val, T min, T max)
{
	if (val > min)
	{
		if (val < max)
		{
			return val;
		}
		return max;
	}
	return min;
}

cv::Mat removeOverlap(const cv::Mat& first, const cv::Mat& second, int thresh){
	cv::Mat out(first.rows, first.cols, CV_64F, 0.0);
    std::cout << first.cols << " " << first.rows << std::endl;
	for(int i=0; i<first.rows; i++){
    	for(int j=0; j<first.cols; j++){
			int f = first.at<uchar>(i,j);
			int s = second.at<uchar>(i,j);
        	std::cout << j << " " << i << std::endl;
			if((f > thresh) != (s > thresh)){
				out.at<uchar>(i,j) = std::max(s,f);
			}
		}
	}
	return out;
}

void whitebalance(const cv::Mat& src, cv::Mat& dst,
		float s1, float s2, int outmin, int outmax)
{
	if ((s1 > 1) || (s2 > 1))
	{
		dst = src;
		return;
	}

	int hist[256] = {0};
	const int numpix = src.rows*src.cols;
	// make histogram of pixel vals
	const unsigned char *srcdata = src.ptr();
	for (int i = 0; i < numpix; ++i)
	{
		++hist[(int)(srcdata[i])];
	}

	// find lowest val in range
	int	minv = 0; // lowest val in range
	{
		const int n1 = s1*numpix; // num pixels out of range on low end
		for (int num = 0; num < n1;)
		{
			num += hist[minv];
			++minv;
		}
	}

	// find higest val in range
	int maxv = 255; // higest val in range
	{
		const int n2 = s2*numpix; // num pixels out of range on high end
		for (int num = 0; num < n2;)
		{
			num += hist[maxv];
			--maxv;
		}
	}

	// scale vals
	const float scale = ((float)(outmax - outmin))/((float)(maxv - minv));
	cv::Mat tmp(src.rows, src.cols, src.type());
	unsigned char *dstdata = tmp.ptr();
	for (int i = 0; i < numpix; ++i)
	{
		dstdata[i] = trunc((int)(scale*(srcdata[i] - minv) + outmin), outmin, outmax);
	}
	dst = tmp;
}
