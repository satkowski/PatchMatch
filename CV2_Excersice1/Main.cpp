// -path1=C:/Users/Marc/Documents/Workspaces/C++_VS/CV2_Excersice1/Ressourcen/frame07.png -path2=C:/Users/Marc/Documents/Workspaces/C++_VS/CV2_Excersice1/Ressourcen/frame08.png -window=3							
#include "Main.h"

using namespace cv;

int main (int argc, const char** argv)
{
	const char* keyMap;
	// Creating a keymap for all the arguments that can passed to that programm
	keyMap = "{path1 | | }"
			 "{path2 | | }"
			 "{window |1 |2 }";

	// Reading the calling arguments
	CommandLineParser parser (argc, argv, keyMap);
	String inputFileName1 = parser.get<String>("path1");
	String inputFileName2 = parser.get<String>("path2");
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
	srand(static_cast<unsigned>(time(0)));



	printf("Start the PatchMatch\n");

	Mat output = calculateOpticalFlow(&inputImage1, &inputImage2, inputWindowSize);

	printf("Finished PatchMatch\n");
	imwrite("warpedImage.jpeg", output);
}