#ifndef PATCHMATCH_H
#define PATCHMATCH_H

#include <opencv2/core/core.hpp>
#include <list>
#include <map>

#include "SimilarityMeasure.h"

#define ITERATION_TIMES 5
#define SEARCH_RATIO 1/2
#define PROPAGATION_EVEN 1
#define PROPAGATION_ODD -1

struct comparePoints
{
	bool operator()(const cv::Point &left, const cv::Point &right);
};

cv::Mat calculateOpticalFlow(cv::Mat* firstImage, cv::Mat* secondImage, int windowSize);

cv::Mat createInitialization(const cv::Mat* firstImage);
std::pair<cv::Point, double> propagationAlg(cv::Mat* firstImage, cv::Mat* secondImage, int windowSize, int propegationDirection, cv::Point actualPoint, cv::Point actualOffset);
cv::Point randomSearchAlg(cv::Mat* firstImage, cv::Mat* secondImage, int windowSize, cv::Point actualPoint, std::pair<cv::Point, double> actualOffset);

cv::Mat warpImage(cv::Mat* firstImage, cv::Mat* opticalFlow);

#endif // !PATCHMATCH_H
