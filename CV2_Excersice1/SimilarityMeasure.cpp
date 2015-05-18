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
			long ssd;
			int pixelsAdded = 0;
			Point imagePatchPixel = Point(imagePoint.x + cX, imagePoint.y + cY);
			Point offsetPatchPixel = Point(imagePoint.x + offset.x + cX, imagePoint.y + offset.y + cY);

			// Try if all pixel are inside the image
			if (imagePatchPixel.x < 0 || imagePatchPixel.x >= firstImage->cols ||
				imagePatchPixel.y < 0 || imagePatchPixel.y >= firstImage->rows ||
				offsetPatchPixel.x < 0 || offsetPatchPixel.x >= secondImage->cols ||
				offsetPatchPixel.y < 0 || offsetPatchPixel.y >= secondImage->rows)
				continue;

			Vec3d difference = firstImage->at<Vec3d>(imagePatchPixel.y, imagePatchPixel.x) - 
								secondImage->at<Vec3d>(offsetPatchPixel.y, offsetPatchPixel.x);
			ssd = static_cast<long>(difference[0]) * static_cast<long>(difference[0]) +
				  static_cast<long>(difference[1]) * static_cast<long>(difference[1]) +
				  static_cast<long>(difference[2]) * static_cast<long>(difference[2]);

			pixelsAdded++;
			ssdPatch += static_cast<double>(ssd) / static_cast<double>(pixelsAdded);
		}

	if (ssdPatch == 0)
		return -1;

	return ssdPatch;
}