#ifndef PATCHMATCH_H
#define PATCHMATCH_H

#include <opencv2/core/core.hpp>
#include <set>
#include <map>

#include "SimilarityMeasure.h"

#define ITERAATION_TIMES 5
#define SEARCH_RATIO 1/2

struct comparePoints
{
	bool operator()(const cv::Point &left, const cv::Point &right);
};

cv::Vec<cv::Mat, 2> calculateOpticalFlow(const cv::Mat* firstImage, const cv::Mat* secondImage, const int windowSize);

cv::Mat createInitialization(const cv::Mat* firstImage);

#endif // !PATCHMATCH_H
