#ifndef PATCHMATCH_H
#define PATCHMATCH_H

#include <opencv2/core/core.hpp>
#include <set>

#include "SimilarityMeasure.h"

#define ITERAATIONTIMES 5

cv::Vec<cv::Mat, 2> calculateOpticalFlow(const cv::Mat* firstImage, const cv::Mat* secondImage);

cv::Mat createInitialization(const cv::Mat* firstImage);

#endif // !PATCHMATCH_H
