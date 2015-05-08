#include "SimilarityMeasure.h"

using namespace cv;

double colorSSD(const Mat* firstImage, const Mat* secondImage, Point imagePoint, int windowSize, Point offset)
{
	// Return infinity if the image point in the first image is outside the image
	if (imagePoint.x < 0 || imagePoint.y < 0 ||
		imagePoint.x >= firstImage->cols || imagePoint.y >= firstImage->rows)
		return std::numeric_limits<double>::infinity();
	// Return infinity if the image point in the first image is outside the image
	if (imagePoint.x + offset.x < 0 || imagePoint.y + offset.y < 0 ||
		imagePoint.x + offset.x >= secondImage->cols || imagePoint.y + offset.y >= secondImage->rows)
		return std::numeric_limits<double>::infinity();

	double ssdPatch = 0;
	// Iterate through the patch
	for (int cY = -windowSize; cY < windowSize; cY++)
		for (int cX = -windowSize; cX < windowSize; cX++)
		{
			double ssd;
			int pixelsAdded = 0;
			// Try if all pixel are inside the image
			try
			{
				// TODO: divide the output with the times a pixel was added
				Point imagePatchPixel = Point(imagePoint.x + cX, imagePoint.y + cY);
				Point offsetPatchPixel = Point(imagePoint.x + offset.x + cX, imagePoint.y + offset.y + cY);

				Vec3d difference = firstImage->at<Vec3d>(imagePatchPixel.y, imagePatchPixel.x) - 
								   secondImage->at<Vec3d>(offsetPatchPixel.y, offsetPatchPixel.x);
				ssd = difference[0] * difference[0] +
					  difference[1] * difference[1] +
					  difference[2] * difference[2];

				pixelsAdded++;
			}
			catch (Exception e)
			{
				continue;
			}
			ssdPatch += ssd / pixelsAdded;
		}

	if (ssdPatch == 0)
		return -1;

	return ssdPatch;
}