#include "SimilarityMeasure.h"

using namespace cv;

double colorSSD(const Mat* firstImage, const Mat* secondImage, Point imagePoint, int windowSize, Point offsetPoint)
{
	double ssdPatch = 0;
	for (int cY = -windowSize; cY < windowSize; cY++)
	{
		for (int cX = -windowSize; cX < windowSize; cX++)
		{
			Mat tempFirstImage, tempSecondImage;
			firstImage->convertTo(tempFirstImage, CV_64FC3);
			secondImage->convertTo(tempSecondImage, CV_64FC3);

			double ssd;
			try
			{
				Point offsetPatchPixel = Point(offsetPoint.x + cY, offsetPoint.y + cX);
				Point imagePatchPixel = Point(imagePoint.x + cY, imagePoint.y + cX);

				Vec3d difference = tempFirstImage.at<Vec3d>(imagePatchPixel) - tempSecondImage.at<Vec3d>(offsetPatchPixel);
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