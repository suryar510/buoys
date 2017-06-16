#include "proc.hpp"

/*
 * Basic Color Filters
 */

cv::Mat yellowFilter(const cv::Mat& in){
	//Yellow
	cv::Mat lab,lab_split[3];
	cv::cvtColor(in,lab, cv::COLOR_BGR2Lab);
	cv::split(lab, lab_split);
	return lab_split[2];
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

/*
 * Mat Erosion
 */

cv::Mat erodeMat(const cv::Mat& src){

	cv::Mat dilation_dst;

	int dilation_elem = 1;
	int dilation_size = 0;
	int dilation_type;

  	if( dilation_elem == 0 ){ dilation_type = cv::MORPH_RECT; }
  	else if( dilation_elem == 1 ){ dilation_type = cv::MORPH_CROSS; }
  	else if( dilation_elem == 2) { dilation_type = cv::MORPH_ELLIPSE; }

	cv::Mat element = getStructuringElement( dilation_type,
                                       cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       cv::Point( dilation_size, dilation_size ) );
  	// Apply the dilation operation
  	erode( src, dilation_dst, element );
	return dilation_dst;
}

/*
 * Illumination Correction
 */

cv::Mat illumination_correction(cv::Mat const &correction_src, float blur_kernel_size)
{
	cv::Mat blurred_img, diff_img, corrected_img;
	cv::Scalar mean_val;
	cv::GaussianBlur(correction_src, blurred_img, cv::Size(blur_kernel_size, blur_kernel_size), 0, 0); 
	diff_img = correction_src - blurred_img;
	mean_val = mean(blurred_img);
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

/*
 *	White Balancing
 */

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
