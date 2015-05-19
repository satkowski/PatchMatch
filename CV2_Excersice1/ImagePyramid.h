#ifndef IMAGEPYRAMID_H
#define IMAGEPYRAMID_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>

#include "PatchMatch.h"

#define ITERATION_IMAGEPYRAMID 5

void calculateImagePiramide(cv::Mat* firstImage, cv::Mat* secondImage, int windowSize);

#endif // !IMAGEPYRAMID_H
