#include "SimilarityMeasure.h"

using namespace cv;

long colorSSD(const Mat* firstImage, const Mat* secondImage, Point imagePoint, int windowSize, Point offset)
{
	// Return infinity if the image point in the first image is outside the image
	if (imagePoint.x < 0 || imagePoint.y < 0 ||
		imagePoint.x >= firstImage->cols || imagePoint.y >= firstImage->rows)
		return std::numeric_limits<long>::infinity();
	// Return infinity if the image point in the first image is outside the image
	if (imagePoint.x + offset.x < 0 || imagePoint.y + offset.y < 0 ||
		imagePoint.x + offset.x >= secondImage->cols || imagePoint.y + offset.y >= secondImage->rows)
		return std::numeric_limits<long>::infinity();

	long ssdPatch = 0;
	// Iterate through the patch
	for (int cY = -windowSize; cY < windowSize; cY++)
		for (int cX = -windowSize; cX < windowSize; cX++)
		{
			long ssd;
			// Try if all pixel are inside the image
			try
			{
				Point imagePatchPixel = Point(imagePoint.x + cX, imagePoint.y + cY);
				Point offsetPatchPixel = Point(imagePoint.x + offset.x + cX, imagePoint.y + offset.y + cY);

				Vec3d difference = firstImage->at<Vec3d>(imagePatchPixel.y, imagePatchPixel.x) - 
								   secondImage->at<Vec3d>(offsetPatchPixel.y, offsetPatchPixel.x);
				ssd = (long)difference[0] * (long)difference[0] +
					  (long)difference[1] * (long)difference[1] +
					  (long)difference[2] * (long)difference[2];
			}
			catch (Exception e)
			{
				continue;
			}
			ssdPatch += ssd;
		}

	if (ssdPatch == 0)
		return -1;

	return ssdPatch;
}