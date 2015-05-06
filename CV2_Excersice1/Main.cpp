#include "Main.h"

using namespace cv;

int main (int argc, const char** argv)
{
	const char* keyMap;
	//Standard image that will be used if dont exist arguments
	keyMap = "{path     |   |../Bilder/haus.jpg }";

	//Reading the Callingarguments
	CommandLineParser parser (argc, argv, keyMap);
	String filename = parser.get<String> ("path");

	//Creating the image and testing if it is empty or not
	Mat image = imread(filename);
	if (image.empty ())
	{
		printf ("Cannot read the image %s\n", filename.c_str ());
		return -1;
	}
}