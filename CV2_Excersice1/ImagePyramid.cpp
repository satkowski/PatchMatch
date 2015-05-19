#include "ImagePyramid.h"

using namespace cv;

void calculateImagePiramide(Mat* firstImage, Mat* secondImage, int windowSize)
{
	// Copy the images to temp Mat
	Mat pyramideFirstI, pyramideSecondI, tempImage;
	firstImage->copyTo(pyramideFirstI);
	secondImage->copyTo(pyramideSecondI);

	for (int iterationIndex = 0; iterationIndex < ITERATION_IMAGEPYRAMID; iterationIndex++)
	{
		printf("Pyramidplain %d\n", iterationIndex);
		calculateOpticalFlow(&pyramideFirstI, &pyramideSecondI, windowSize);

		// Downsampling both the images
		pyramideFirstI.copyTo(tempImage);
		cvPyrDown(&tempImage, &pyramideFirstI);
		pyramideSecondI.copyTo(tempImage);
		cvPyrDown(&tempImage, &pyramideSecondI);
	}
}