#include "PatchMatch.h"

using namespace cv;

void calculateOpticalFlow(Mat* firstImage, Mat* secondImage, int windowSize)
{
	Mat tempFirstImage, tempSecondImage, outputImage;
	firstImage->convertTo(tempFirstImage, CV_64FC3);
	secondImage->convertTo(tempSecondImage, CV_64FC3);

	Mat opticalFlow = createInitialization(firstImage);

	// Loop for all iterations
	for (int iterationIndex = 0; iterationIndex < ITERATION_TIMES; iterationIndex++)
	{
		printf("Iteration %d\n", iterationIndex);
		// Iteration of all pixels/patches
		// EVEN Iteration
		for (int cY = 0; cY < firstImage->rows; cY++)
			for (int cX = 0; cX < firstImage->cols; cX++)
			{
				// Propagation step
				std::pair<Point, double> actualOffset = propagationAlg(&tempFirstImage, &tempSecondImage, windowSize, 
																	   PROPAGATION_EVEN, Point(cX, cY), opticalFlow.at<Point>(cY, cX));
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlow.at<Point>(cY, cX) = randomSearchAlg(&tempFirstImage, &tempSecondImage, windowSize, Point(cX, cY), actualOffset);
			}

		outputImage = warpImage(firstImage, &opticalFlow);
		saveImage(&outputImage, "Iteration_" + std::to_string(iterationIndex) + ".jpeg");
		outputImage = warpImage(firstImage, &actualOpticalFlow, "OpticalFlow_" + std::to_string(iterationIndex) + ".txt");
		saveImage(&outputImage, "WarpedImage_" + std::to_string(iterationIndex) + ".jpeg");
		// Cancel the next part if the number of Iterations is reached
		if (++iterationIndex >= ITERATION_TIMES)
			break;

		printf("Iteration %d\n", iterationIndex);
		// Iteration of all pixels/patches
		// ODD Iteration
		for (int cY = firstImage->rows - 1; cY > 0; cY--)
			for (int cX = firstImage->cols - 1; cX > 0; cX--)
			{
				// Propagation step
				std::pair<Point, double> actualOffsetPair = propagationAlg(&tempFirstImage, &tempSecondImage, windowSize,
																		   PROPAGATION_ODD, Point(cX, cY), opticalFlow.at<Point>(cY, cX));
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlow.at<Point>(cY, cX) = randomSearchAlg(&tempFirstImage, &tempSecondImage, windowSize, Point(cX, cY), actualOffsetPair);
			}

		outputImage = warpImage(firstImage, &opticalFlow);
		saveImage(&outputImage, "Iteration_" + std::to_string(iterationIndex) + ".jpeg");
		outputImage = warpImage(firstImage, &actualOpticalFlow, "OpticalFlow_" + std::to_string(iterationIndex) + ".txt");
		saveImage(&outputImage, "WarpedImage_" + std::to_string(iterationIndex) + ".jpeg");
	}
}

Mat createInitialization(const Mat* firstImage)
{
	using namespace std;
	printf("Initialization: Start");

	Mat initializationOpticalFlow = Mat_<Point>(firstImage->rows, firstImage->cols);
	vector<Point> avaiblePointList;
	
	// Fill the set with all avaible points
	for (int cY = 0; cY < firstImage->rows; cY++)
		for (int cX = 0; cX < firstImage->cols; cX++)
			avaiblePointList.push_back(Point(cX, cY)); 

	random_shuffle(avaiblePointList.begin(), avaiblePointList.end());

	// Pair all points with random point in the set
	for (int cY = 0; cY < firstImage->rows; cY++)
		for (int cX = 0; cX < firstImage->cols; cX++)
		{
			// Search for a random point in the set and delete it from there
			Point randomPoint = avaiblePointList.back();
			avaiblePointList.pop_back();
			// Add the random point as offset to the matrix
			initializationOpticalFlow.at<Point>(cY, cX) = Point(randomPoint.x - cX, randomPoint.y - cY);
		}

	printf(" - End\n");
	return initializationOpticalFlow;
}

std::pair<Point, double> propagationAlg(Mat* firstImage, Mat* secondImage, int windowSize, int propegationDirection, Point actualPoint, Point actualOffset)
{
	//printf("Propagation: Start");

	Point verticalNeighbourOffset = Point(actualOffset.x, actualOffset.y + propegationDirection);
	Point horizontalNeighbourOffset = Point(actualOffset.x + propegationDirection, actualOffset.y);

	// Propagation for both neighbours of one patch/pixel
	double actualSimilarity = colorSSD(firstImage, secondImage, actualPoint, windowSize, actualOffset);
	double verticalNeighbourSim = colorSSD(firstImage, secondImage, actualPoint, windowSize, verticalNeighbourOffset);
	double horizontalNeighbourSim = colorSSD(firstImage, secondImage, actualPoint, windowSize, horizontalNeighbourOffset);

	std::pair<Point, double> output;
	// Decide which has the best energy and save it as new offset/flow
	if (verticalNeighbourSim <= horizontalNeighbourSim && verticalNeighbourSim < actualSimilarity)
		output = std::pair<Point, double>(verticalNeighbourOffset, verticalNeighbourSim);
	else if (horizontalNeighbourSim < verticalNeighbourSim && horizontalNeighbourSim < actualSimilarity)
		output = std::pair<Point, double>(horizontalNeighbourOffset, horizontalNeighbourSim);

	//printf(" - End\n");
	return output;
}

Point randomSearchAlg(Mat* firstImage, Mat* secondImage, int windowSize, Point actualPoint, std::pair<Point, double> actualOffsetPair)
{
	using namespace std;
	//printf("RandomSearch: Start");

	int iteration = 0;
	int maxSearchRadius = max(firstImage->rows, firstImage->cols) / 2;
	vector<pair<Point, double> > randomPointsList;
	pair<Point, double> bestOffsetPair = actualOffsetPair;
	// Iterate while maxSearchRadius * SEARCH_RATIO^iteration < 1
	do
	{
		Point randomPoint = Point_<float>((static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2)) - 1,
										  (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2)) - 1);
		randomPoint *= maxSearchRadius*pow(SEARCH_RATIO, iteration++);
		Point randomOffset = Point(static_cast<int>(randomPoint.x) + actualOffsetPair.first.x, 
								   static_cast<int>(randomPoint.y) + actualOffsetPair.first.y);
		pair<Point, double> randomOffsetPair = pair<Point, double>(randomOffset,
																   colorSSD(firstImage, secondImage, actualPoint, windowSize, randomOffset));
		// Choose the best offset and test if it is better than the actual one
		if (randomOffsetPair.second < bestOffsetPair.second)
			bestOffsetPair = randomOffsetPair;
	}
	while (maxSearchRadius*pow(SEARCH_RATIO, iteration) >= 1);

	//printf(" - End\n");
	return bestOffsetPair.first;
}

Mat warpImage(Mat* firstImage, Mat* opticalFlow, String filename)
{
	std::ofstream myfile;
	myfile.open(filename);
	myfile << *opticalFlow;
	myfile.close();

	Mat outputImage = Mat_<Vec3b>(firstImage->rows, firstImage->cols);

	for (int cY = 0; cY < firstImage->rows; cY++)
		for (int cX = 0; cX < firstImage->cols; cX++)
		{
			Point testIn = opticalFlow->at<Point>(cY, cX);
			Point testOut = Point(opticalFlow->at<Point>(cY, cX).x + cX, opticalFlow->at<Point>(cY, cX).y + cY);
			try {
			outputImage.at<Vec3b>(opticalFlow->at<Point>(cY, cX).y + cY, opticalFlow->at<Point>(cY, cX).x + cX)
				= firstImage->at<Vec3b>(cY, cX);
		}
			catch (Exception e) 
			{
				printf("");
			}
		}

	return outputImage;
}

void saveImage(Mat* outputImage, String fileName)
{
	imwrite(fileName, *outputImage);
}