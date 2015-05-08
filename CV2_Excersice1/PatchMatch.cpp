#include "PatchMatch.h"

using namespace cv;

Vec<Mat, 2> calculateOpticalFlow(const Mat* firstImage, const Mat* secondImage, const int windowSize)
{
	Mat tempFirstImage, tempSecondImage;
	firstImage->convertTo(tempFirstImage, CV_64FC3);
	secondImage->convertTo(tempSecondImage, CV_64FC3);

	Mat opticalFlow = createInitialization(firstImage);

	// Loop for all iterations
	for (int iterationIndex = 0; iterationIndex < ITERATION_TIMES; iterationIndex++)
	{
		// Iteration of all pixels/patches
		// EVEN Iteration
		for (int cY = 0; cY < firstImage->rows; cY++)
			for (int cX = 0; cX < firstImage->cols; cX++)
			{
				// Propagation step
				std::pair<Point, double> actualOffset = propagationAlg(&tempFirstImage, &tempSecondImage, windowSize, 
																	   PROPAGATION_EVEN, Point(cX, cY), opticalFlow.at<Point>(cY, cX));
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlow.at<Point>(cY, cX) = randomSearchAlg(&tempFirstImage, &tempSecondImage, windowSize, Point(cY, cX), actualOffset);
			}

		// Cancel the next part if the number of Iterations is reached
		if (++iterationIndex >= ITERATION_TIMES)
			break;

		// Iteration of all pixels/patches
		// ODD Iteration
		for (int cY = firstImage->rows; cY > 0; cY--)
			for (int cX = firstImage->cols; cX > 0; cX--)
			{
				// Propagation step
				std::pair<Point, double> actualOffset = propagationAlg(&tempFirstImage, &tempSecondImage, windowSize,
																	   PROPAGATION_ODD, Point(cX, cY), opticalFlow.at<Point>(cY, cX));
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlow.at<Point>(cY, cX) = randomSearchAlg(&tempFirstImage, &tempSecondImage, windowSize, Point(cY, cX), actualOffset);
			}
	}
}

Mat createInitialization(const Mat* firstImage)
{
	using namespace std;

	Mat initializationOpticalFlow = Mat_<Point>(firstImage->rows, firstImage->cols);
	set<Point, comparePoints> avaiblePointSet;

	// Fill the set with all avaible points
	for (int cY = 0; cY < firstImage->rows; cY++)
		for (int cX = 0; cX < firstImage->cols; cX++)
			avaiblePointSet.insert(Point(cX, cY));

	int avaiblePointSetSize = avaiblePointSet.size();

	// Pair all points with random point in the set
	for (int cY = 0; cY < firstImage->rows; cY++)
		for (int cX = 0; cX < firstImage->cols; cX++)
		{
			// Search for a random point in the set and delete it from there
			set<Point>::iterator iterator = avaiblePointSet.begin();
			next(iterator, rand() % avaiblePointSetSize--);
			Point randomPoint = *iterator;
			avaiblePointSet.erase(randomPoint);
			// Add the random point as offset to the matrix
			initializationOpticalFlow.at<Point>(cY, cX) = Point(randomPoint.x - cX, randomPoint.y - cY);
		}

	return initializationOpticalFlow;
}

std::pair<Point, double> propagationAlg(Mat* firstImage, Mat* secondImage, int windowSize, int propegationDirection, Point actualPoint, Point actualOffset)
{
	Point verticalNeighbourOffset = Point(actualOffset.x, actualOffset.y + propegationDirection);
	Point horizontalNeighbourOffset = Point(actualOffset.x + propegationDirection, actualOffset.y);

	// Propagation for both neighbours of one patch/pixel
	double actualSimilarity = colorSSD(firstImage, secondImage, actualPoint, windowSize, actualOffset);
	double verticalNeighbourSim = colorSSD(firstImage, secondImage, actualPoint, windowSize, verticalNeighbourOffset);
	double horizontalNeighbourSim = colorSSD(firstImage, secondImage, actualPoint, windowSize, horizontalNeighbourOffset);

	// Decide which has the best energy and save it as new offset/flow
	if (verticalNeighbourSim <= horizontalNeighbourSim && verticalNeighbourSim < actualSimilarity)
	{
		actualOffset = verticalNeighbourOffset;
		actualSimilarity = verticalNeighbourSim;
	}
	else if (horizontalNeighbourSim < verticalNeighbourSim && horizontalNeighbourSim < actualSimilarity)
	{
		actualOffset = horizontalNeighbourOffset;
		actualSimilarity = horizontalNeighbourSim;
	}

	return std::pair<Point, double>(actualOffset, actualSimilarity);
}

Point randomSearchAlg(Mat* firstImage, Mat* secondImage, int windowSize, Point actualPoint, std::pair<Point, double> actualOffset)
{
	using namespace std;

	int iteration = 0;
	int maxSearchRadius = max(firstImage->rows, firstImage->cols) / 2;
	map<Point, double, comparePoints> randomPointsMap;
	// Iterate while maxSearchRadius * SEARCH_RATIO^iteration < 1
	do
	{
		Vec2f randomPoint = Vec2f((static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2)) - 1,
								  (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2)) - 1);
		randomPoint *= maxSearchRadius*pow(SEARCH_RATIO, iteration++);
		Point randomOffset = Point((int)randomPoint[0] + actualOffset.first.x, (int)randomPoint[1] + actualOffset.first.y);
		randomPointsMap[randomOffset] = colorSSD(firstImage, secondImage, actualPoint, windowSize, randomOffset);
	}
	while (maxSearchRadius*pow(SEARCH_RATIO, iteration) >= 1);

	// Choose the best offset and test if it is better than the actual one
	pair<Point, double> minOffsetPair;
	for (map<Point, double>::iterator it = randomPointsMap.begin(); it != randomPointsMap.end(); next(it, 1))
		minOffsetPair = (it->second < minOffsetPair.second) ? *it : minOffsetPair;
	if (minOffsetPair.second < actualOffset.second)
		actualOffset = minOffsetPair;

	return actualOffset.first;
}

bool comparePoints::operator()(const Point &left, const Point &right)
{
	return (left.x < right.x && left.y < right.y);
}