#include "ImagePyramid.h"

using namespace cv;

void calculateImagePiramide(Mat* firstImage, Mat* secondImage, int windowSize)
{
	// Copy the images to temp Mat
	std::vector<Mat> firstImages, secondImages;
	firstImages.push_back(*firstImage);
	secondImages.push_back(*secondImage);
	Mat opticalFlow = Mat_<Point>(0, 0);

	printf("Initialize ImagePyramides: Start");
	// Downsampling of all images within the range of the constant and save them in a vector
	for (int iterationIndex = 0; iterationIndex < ITERATION_IMAGEPYRAMID; iterationIndex++)
	{
		Mat tempImage;
		pyrDown(firstImages.back(), tempImage, Size(static_cast<int>(firstImages.back().cols / 2),
													static_cast<int>(firstImages.back().rows / 2)));
		firstImages.push_back(tempImage);
		pyrDown(secondImages.back(), tempImage, Size(static_cast<int>(secondImages.back().cols / 2),
													 static_cast<int>(secondImages.back().rows / 2)));
		secondImages.push_back(tempImage);
	}
	printf(" - End\n");

	for (int iterationIndex = 0; iterationIndex < ITERATION_IMAGEPYRAMID + 1; iterationIndex++)
	{
		printf("Pyramidplain %d\n", iterationIndex);
		calculateOpticalFlow(&firstImages.back(), &secondImages.back(), windowSize, &opticalFlow, "IP" + std::to_string(iterationIndex));
		// Delete the used images
		firstImages.pop_back();
		secondImages.pop_back();

		if (iterationIndex == ITERATION_IMAGEPYRAMID)	break;

		Mat tempOpticalFlow;
		opticalFlow.copyTo(tempOpticalFlow);
		opticalFlow = Mat_<Point>(firstImages.back().rows, secondImages.back().cols);
		// Upsample the opticalFlow mat
		for (int cY = 0; cY < tempOpticalFlow.rows; cY++)
		{
			// Creating pointer
			Point* tempOpticalFlowRowP = tempOpticalFlow.ptr<Point>(cY);
			Point* opticalFlowRowP1 = opticalFlow.ptr<Point>(cY * 2);
			Point* opticalFlowRowP2 = opticalFlow.ptr<Point>(cY * 2 + 1);
			for (int cX = 0; cX < tempOpticalFlow.cols; cX++)
			{
				Point actualFlow = tempOpticalFlowRowP[cX];
				// Expand the existing flow on all four pixels
				opticalFlowRowP1[cX * 2] = actualFlow;
				opticalFlowRowP1[cX * 2 + 1] = actualFlow;
				opticalFlowRowP2[cX * 2] = actualFlow;
				opticalFlowRowP2[cX * 2 + 1] = actualFlow;
			}
		}
	}
}