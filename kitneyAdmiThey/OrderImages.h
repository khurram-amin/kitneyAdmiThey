
#include "stdafx.h"
#include <iostream>
#include <stdint.h>
#include <dirent.h>
#include <string>
#include <cmath>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;

#define uint8 uint8_t
#define uint16 uint16_t

class OrderImages 
{
private:
	string imFolderPath;
	vector<std::string> imNameList;
	uint8 NUMBER_OF_PYRAMIDS;
	vector< vector< Mat > > PYRAMID;

public:
	OrderImages();
	OrderImages(String);
	void readImFolderContents();
	bool powerOfTwo(uint16);
	void buildImPyramid();

};
