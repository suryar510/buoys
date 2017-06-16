#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <opencv2/photo.hpp>
#include <vector>

cv::Mat yellowFilter(const cv::Mat&);
cv::Mat redFilter(const cv::Mat&);
cv::Mat greenFilter(const cv::Mat&);
cv::Mat erodeMat(const cv::Mat&);

cv::Point brightestRed(const cv::Mat&, const cv::Mat&, int);
cv::Point brightestRedSmall(const cv::Mat&, const cv::Mat&, int, std::vector<cv::Point>&);
bool possible(cv::Point, const std::vector<cv::Point>&);
template <typename T>
inline T trunc(T val, T min, T max);

cv::Mat color_illumination_correction(cv::Mat const &correction_src, float blur_kernel_size);
cv::Mat illumination_correction(cv::Mat const &correction_src, float blur_kernel_size);

/*
 * s1 -> lowest percent range of colors
 * s2 -> highest percent range of colors
 */
void whitebalance(
	const cv::Mat& src, cv::Mat& dst, float s1 = .04f,
	float s2 = .04f, int outmin=0, int outmax=255);
