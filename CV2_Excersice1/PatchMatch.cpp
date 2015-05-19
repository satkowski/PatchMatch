#include "PatchMatch.h"

using namespace cv;

void calculateOpticalFlow(Mat* firstImage, Mat* secondImage, int windowSize)
{
	Mat tempFirstImage, tempSecondImage, outputImage;
	firstImage->convertTo(tempFirstImage, CV_64FC3);
	secondImage->convertTo(tempSecondImage, CV_64FC3);

	Mat opticalFlow = createInitializationForColorSSD(firstImage);

	// Loop for all iterations
	for (int iterationIndex = 0; iterationIndex < ITERATION_TIMES; iterationIndex++)
	{
		printf("Iteration %d\n", iterationIndex);
		// Iteration of all pixels/patches
		// EVEN Iteration
		for (int cY = 0; cY < firstImage->rows; cY++)
		{
			// Creating pointer
			Point* opticalFlowRowP = opticalFlow.ptr<Point>(cY);
			for (int cX = 0; cX < firstImage->cols; cX++)
			{
				// Propagation step
				std::pair<Point, double> actualOffset = propagationAlg(&tempFirstImage, &tempSecondImage, windowSize,
																	   PROPAGATION_EVEN, Point(cX, cY), opticalFlowRowP[cX]);
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlowRowP[cX] = randomSearchAlg(&tempFirstImage, &tempSecondImage, windowSize, Point(cX, cY), actualOffset);
			}
		}
		outputImage = warpImage(firstImage, &opticalFlow, "OpticalFlow_" + std::to_string(iterationIndex) + ".txt");
		saveImage(&outputImage, "WarpedImage_" + std::to_string(iterationIndex) + ".jpeg");
		// Cancel the next part if the number of Iterations is reached
		if (++iterationIndex >= ITERATION_TIMES)
			break;

		printf("Iteration %d\n", iterationIndex);
		// Iteration of all pixels/patches
		// ODD Iteration
		for (int cY = firstImage->rows - 1; cY > 0; cY--)
		{
			// Creating pointer
			Point* opticalFlowRowP = opticalFlow.ptr<Point>(cY);
			for (int cX = firstImage->cols - 1; cX > 0; cX--)
			{
				// Propagation step
				std::pair<Point, double> actualOffsetPair = propagationAlg(&tempFirstImage, &tempSecondImage, windowSize,
																		   PROPAGATION_ODD, Point(cX, cY), opticalFlowRowP[cX]);
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlowRowP[cX] = randomSearchAlg(&tempFirstImage, &tempSecondImage, windowSize, Point(cX, cY), actualOffsetPair);
			}
		}
		outputImage = warpImage(firstImage, &opticalFlow, "OpticalFlow_" + std::to_string(iterationIndex) + ".txt");
		saveImage(&outputImage, "WarpedImage_" + std::to_string(iterationIndex) + ".jpeg");
	}
	//toFlowFile(&opticalFlow, "test");
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
	{
		// Creating pointer
		Point* opticalFlowRowP = opticalFlow->ptr<Point>(cY);
		Vec3b* inputImageRowP = firstImage->ptr<Vec3b>(cY);

		for (int cX = 0; cX < firstImage->cols; cX++)
		{
			Point actualFlow = opticalFlowRowP[cX];
			outputImage.ptr<Vec3b>(actualFlow.y + cY)[actualFlow.x + cX] = inputImageRowP[cX];
		}
	}

	return outputImage;
}

void saveImage(Mat* outputImage, String fileName)
{
	imwrite(fileName, *outputImage);
}

//void toFlowFile(Mat* opticalFlow, String filename)
//{
//	int opticalFlowRow = opticalFlow->rows;
//	int opticalFlowCol = opticalFlow->cols;
//
//	std::cout << opticalFlowRow << "," << opticalFlowCol << std::endl;
//
//	// combine path + filename to an arg for fopen
//	String combinedFn = /*m_outPath + "/" +*/ filename;
//	std::FILE *stream = std::fopen(combinedFn.c_str(), "wb");
//
//	// write the header
//	fprintf(stream, "PIEH");
//	fwrite(&opticalFlowCol, sizeof(int), 1, stream);
//	fwrite(&opticalFlowRow, sizeof(int), 1, stream);
//
//	for (int cY = 0; cY < opticalFlowRow; cY++)
//	{
//		// Create pointer
//		Point* opticalFlowRowP = opticalFlow->ptr<Point>(cY);
//		for (int cX = 0; cX < opticalFlowCol; cX++)
//		{
//			float band0 = (float)opticalFlowRowP[cX].x;
//			fwrite(&band0, sizeof(float), 1, stream);
//
//			float band1 = (float)opticalFlowRowP[cX].y;
//			fwrite(&band1, sizeof(float), 1, stream);
//		}
//	}
//
//	// close the file
//	fclose(stream);
//}