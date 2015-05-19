#ifndef PATCHMATCH_H
#define PATCHMATCH_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <list>
#include <iostream>
#include <fstream>

#include "SimilarityMeasure.h"

#define ITERATION_PATCHMATCH 5
#define SEARCH_RATIO 0.5
#define PROPAGATION_EVEN 1
#define PROPAGATION_ODD -1

void calculateOpticalFlow(cv::Mat* firstImage, cv::Mat* secondImage, int windowSize);

std::pair<cv::Point, double> propagationAlg(cv::Mat* firstImage, cv::Mat* secondImage, int windowSize, int propegationDirection, cv::Point actualPoint, cv::Point actualOffset);
cv::Point randomSearchAlg(cv::Mat* firstImage, cv::Mat* secondImage, int windowSize, cv::Point actualPoint, std::pair<cv::Point, double> actualOffset);

cv::Mat warpImage(cv::Mat* firstImage, cv::Mat* opticalFlow, cv::String filename);
void saveImage(cv::Mat* outputImage, cv::String fileName);

void toFlowFile(cv::Mat* opticalFlow, cv::String filename);

#endif // !PATCHMATCH_H
