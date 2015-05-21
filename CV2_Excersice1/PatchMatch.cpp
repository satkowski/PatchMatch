#include "PatchMatch.h"

using namespace cv;

void calculateOpticalFlow(Mat* firstImage, Mat* secondImage, int windowSize, Mat* inputOpticalFlow, String filenamePart)
{
	Mat convFirstImage, tempSecondImage, outputImage, opticalFlow;
	firstImage->convertTo(convFirstImage, CV_64FC3);
	secondImage->convertTo(tempSecondImage, CV_64FC3);

	// Create an new random optical flow is empty or the pointer doesn't exists
	if (inputOpticalFlow == nullptr || (inputOpticalFlow->cols == 0 && inputOpticalFlow->rows == 0))
		opticalFlow = createInitializationForColorSSD(firstImage);
	else										
		inputOpticalFlow->copyTo(opticalFlow);

	// Loop for all iterations
	for (int iterationIndex = 0; iterationIndex < ITERATION_PATCHMATCH; iterationIndex++)
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
				std::pair<Point, double> actualOffset = propagationAlg(&convFirstImage, &tempSecondImage, windowSize,
																	   PROPAGATION_EVEN, Point(cX, cY), opticalFlowRowP[cX], &opticalFlow);
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlowRowP[cX] = randomSearchAlg(&convFirstImage, &tempSecondImage, windowSize, Point(cX, cY), actualOffset).first;
				printf("");
			}
		}
		outputImage = warpImage(firstImage, &opticalFlow, filenamePart + "OpticalFlow_" + std::to_string(iterationIndex) + ".txt");
		saveImage(&outputImage, filenamePart + "WarpedImage_" + std::to_string(iterationIndex) + ".jpeg");
		// Cancel the next part if the number of Iterations is reached
		if (++iterationIndex >= ITERATION_PATCHMATCH)
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
				std::pair<Point, double> actualOffsetPair = propagationAlg(&convFirstImage, &tempSecondImage, windowSize,
																		   PROPAGATION_ODD, Point(cX, cY), opticalFlowRowP[cX], &opticalFlow);
				// RandomSearch step: Change tha last offset to the computed best offset
				opticalFlowRowP[cX] = randomSearchAlg(&convFirstImage, &tempSecondImage, windowSize, Point(cX, cY), actualOffsetPair).first;
				printf("");
			}
		}
		outputImage = warpImage(firstImage, &opticalFlow, filenamePart + "OpticalFlow_" + std::to_string(iterationIndex) + ".txt");
		saveImage(&outputImage, filenamePart + "WarpedImage_" + std::to_string(iterationIndex) + ".jpeg");
	}
	//toFlowFile(&opticalFlow, "test");
	if (inputOpticalFlow != nullptr)
		opticalFlow.copyTo(*inputOpticalFlow);
}

std::pair<Point, double> propagationAlg(Mat* firstImage, Mat* secondImage, int windowSize, int propegationDirection, Point actualPoint, Point actualOffset, Mat* opticalFlow)
{
	double actualSimilarity = colorSSD(firstImage, secondImage, actualPoint, windowSize, actualOffset);

	Point verticalNeighbourOffset; 
	double verticalNeighbourSim = std::numeric_limits<double>::infinity();
	// Look for the vertical neighbour of the actual point
	if (actualPoint.y + propegationDirection >= 0 && actualPoint.y + propegationDirection < opticalFlow->rows)
	{
		// Get the offset of the neighbour
		verticalNeighbourOffset = opticalFlow->ptr<Point>(actualPoint.y + propegationDirection)[actualPoint.x];
		// Get the vertical neighbour
		verticalNeighbourOffset.y -propegationDirection;
		double verticalNeighbourSim = colorSSD(firstImage, secondImage, actualPoint, windowSize, verticalNeighbourOffset);
	}
	Point horizontalNeighbourOffset;
	double horizontalNeighbourSim = std::numeric_limits<double>::infinity();
	// Look for the horizontal neighbour of the actual point
	if (actualPoint.x + propegationDirection >= 0 && actualPoint.x + propegationDirection < opticalFlow->cols)
	{
		// Get the offset of the neighbour
		horizontalNeighbourOffset = opticalFlow->ptr<Point>(actualPoint.y)[actualPoint.x + propegationDirection];
		// Get the horizontal neighbour
		horizontalNeighbourOffset.x = -propegationDirection;
		horizontalNeighbourSim = colorSSD(firstImage, secondImage, actualPoint, windowSize, horizontalNeighbourOffset);
	}

	std::pair<Point, double> output;
	// Decide which has the best energy and save it as new offset/flow
	if (verticalNeighbourSim <= actualSimilarity && verticalNeighbourSim <= horizontalNeighbourSim)
		output = std::pair<Point, double>(verticalNeighbourOffset, verticalNeighbourSim);
	else if (horizontalNeighbourSim <= actualSimilarity && horizontalNeighbourSim < verticalNeighbourSim)
		output = std::pair<Point, double>(horizontalNeighbourOffset, horizontalNeighbourSim);
	else
		output = std::pair<Point, double>(actualOffset, actualSimilarity);

	return output;
}

std::pair<Point, double> randomSearchAlg(Mat* firstImage, Mat* secondImage, int windowSize, Point actualPoint, std::pair<Point, double> actualOffsetPair)
{
	using namespace std;
	//printf("RandomSearch: Start");

	int iteration = 0;
	int maxSearchRadius = max(firstImage->rows, firstImage->cols) / 2;
	pair<Point, double> bestOffsetPair = actualOffsetPair;
	// Iterate while maxSearchRadius * SEARCH_RATIO^iteration < 1
	do
	{
		Vec2f randomPoint = Vec2f((static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2)) - 1,
								  (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2)) - 1);
		randomPoint *= maxSearchRadius*pow(SEARCH_RATIO, iteration++);
		Point randomOffset = Point(randomPoint[0] + actualOffsetPair.first.x, 
								   randomPoint[1] + actualOffsetPair.first.y);
		pair<Point, double> randomOffsetPair = pair<Point, double>(randomOffset,
																   colorSSD(firstImage, secondImage, actualPoint, windowSize, randomOffset));
		// Choose the best offset and test if it is better than the actual one
		if (randomOffsetPair.second < bestOffsetPair.second)
			bestOffsetPair = randomOffsetPair;
	}
	while (maxSearchRadius*pow(SEARCH_RATIO, iteration) >= 1);

	//printf(" - End\n");
	return bestOffsetPair;
}

Mat warpImage(Mat* secondImage, Mat* opticalFlow, String filename)
{
	std::ofstream myfile;
	myfile.open(filename);
	myfile << *opticalFlow;
	myfile.close();

	Mat outputImage = Mat_<Vec3b>(secondImage->rows, secondImage->cols);

	for (int cY = 0; cY < secondImage->rows; cY++)
	{
		// Creating pointer
		Point* opticalFlowRowP = opticalFlow->ptr<Point>(cY);
		Vec3b* inputImageRowP = secondImage->ptr<Vec3b>(cY);
		for (int cX = 0; cX < secondImage->cols; cX++)
		{
			Point actualFlow = opticalFlow->at<Point>(cY, cX);
			outputImage.ptr<Vec3b>(cY)[cX] = secondImage->ptr<Vec3b>(actualFlow.y + cY)[actualFlow.x + cX];
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