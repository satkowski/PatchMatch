#ifndef SIMILARITYMEASURE_H
#define SIMILARITYMEASURE_H

#include <opencv2/core/core.hpp>

double colorSSD(const cv::Mat* firstImage, const cv::Mat* secondImage, cv::Point imagePoint, int windowSize, cv::Point offset);

#endif // !SIMILARITYMEASURE_H
