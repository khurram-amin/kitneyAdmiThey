
#include "stdafx.h"
#include <iostream>
#include <stdint.h>
#include <dirent.h>
#include <string>
#include <cmath>

#include <opencv2\core\core.hpp>

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

public:
	OrderImages();
	OrderImages(String);
	void readImFolderContents();
	bool powerOfTwo(uint16);	

};
