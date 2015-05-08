#include "SimilarityMeasure.h"

using namespace cv;

double colorSSD(const Mat* firstImage, const Mat* secondImage, Point imagePoint, int windowSize, Point offset)
{
	double ssdPatch = 0;
	for (int cY = -windowSize; cY < windowSize; cY++)
	{
		for (int cX = -windowSize; cX < windowSize; cX++)
		{
			double ssd;
			try
			{
				Point imagePatchPixel = Point(imagePoint.x + cY, imagePoint.y + cX);
				Point offsetPatchPixel = Point(imagePoint.x + offset.x + cY, imagePoint.y +  offset.y + cX);

				Vec3d difference = firstImage->at<Vec3d>(imagePatchPixel) - secondImage->at<Vec3d>(offsetPatchPixel);
				ssd = difference[0] * difference[0] +
					  difference[1] * difference[1] +
					  difference[2] * difference[2];
			}
			catch (Exception e)
			{
				continue;
			}
			ssdPatch += ssd;
		}
	}

	if (ssdPatch == 0)
		return -1;

	return ssdPatch;
}