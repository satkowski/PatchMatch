#include "ImagePyramid.h"

using namespace cv;

void calculateImagePiramide(Mat* firstImage, Mat* secondImage, int windowSize)
{
	// Copy the images to temp Mat
	Mat pyramideFirstI, pyramideSecondI, tempImage;
	firstImage->copyTo(pyramideFirstI);
	secondImage->copyTo(pyramideSecondI);
	Mat opticalFlow = Mat_<Point>(0, 0);
	
	// Downsampling both images
	pyrDown(tempImage, pyramideFirstI, Size(static_cast<int>(tempImage.cols / pow(2, ITERATION_IMAGEPYRAMID)), 
											static_cast<int>(tempImage.rows / pow(2, ITERATION_IMAGEPYRAMID))));
	pyrDown(tempImage, pyramideFirstI, Size(static_cast<int>(tempImage.cols / pow(2, ITERATION_IMAGEPYRAMID)), 
											static_cast<int>(tempImage.rows / pow(2, ITERATION_IMAGEPYRAMID))));

	for (int iterationIndex = 1; iterationIndex <= ITERATION_IMAGEPYRAMID; iterationIndex++)
	{
		printf("Pyramidplain %d\n", iterationIndex);
		calculateOpticalFlow(&pyramideFirstI, &pyramideSecondI, windowSize, &opticalFlow);

		// Downsampling both the images
		pyramideFirstI.copyTo(tempImage);
		pyrDown(tempImage, pyramideFirstI, Size(static_cast<int>(tempImage.cols / pow(2, ITERATION_IMAGEPYRAMID - iterationIndex)),
												static_cast<int>(tempImage.rows / pow(2, ITERATION_IMAGEPYRAMID - iterationIndex))));
		pyramideSecondI.copyTo(tempImage);
		pyrDown(tempImage, pyramideFirstI, Size(static_cast<int>(tempImage.cols / pow(2, ITERATION_IMAGEPYRAMID - iterationIndex)),
												static_cast<int>(tempImage.rows / pow(2, ITERATION_IMAGEPYRAMID - iterationIndex))));

		Mat tempOpticalFlow;
		opticalFlow.copyTo(tempOpticalFlow);
		opticalFlow = Mat_<Point>(pyramideFirstI.rows, pyramideFirstI.cols);
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