// -path1=C:/Users/Marc/Documents/Workspaces/C++_VS/CV2_Excersice1/Ressourcen/frame07.png -path2=C:/Users/Marc/Documents/Workspaces/C++_VS/CV2_Excersice1/Ressourcen/frame08.png -pixelX=20 -pixelY=20 -offsetX=50 -offsetY=50 -window=3							
#include "Main.h"

using namespace cv;

int main (int argc, const char** argv)
{
	const char* keyMap;
	// Creating a keymap for all the arguments that can passed to that programm
	keyMap = "{path1 | | }"
			 "{path2 | | }"
			 "{pixelX |1 |0 }"
			 "{pixelY |1 |0 }"
			 "{offsetX |1 |0 }"
			 "{offsetY |1 |0 }"
			 "{window |1 |2 }";

	// Reading the calling arguments
	CommandLineParser parser (argc, argv, keyMap);
	String inputFileName1 = parser.get<String>("path1");
	String inputFileName2 = parser.get<String>("path2");
	Point inputImagePoint = Point(parser.get<int>("pixelX"), parser.get<int>("pixelY"));
	Point inputOffsetImagePoint = Point(parser.get<int>("offsetX"), parser.get<int>("offsetY"));
	int inputWindowSize = parser.get<int>("window");

	// Creating the images and testing if there are empty or not
	Mat inputImage1 = imread(inputFileName1);
	if (inputImage1.empty ())
	{
		printf("Cannot read the image %s\n", inputFileName1.c_str());
		return -1;
	}
	Mat inputImage2 = imread(inputFileName2);
	if (inputImage1.empty())
	{
		printf("Cannot read the image %s\n", inputFileName2.c_str());
		return -1;
	}

	// Initializies random
	srand((uint)time(0));
}