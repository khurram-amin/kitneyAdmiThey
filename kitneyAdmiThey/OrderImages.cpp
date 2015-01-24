#include "stdafx.h"
#include "OrderImages.h"

OrderImages::OrderImages()
{
	imFolderPath = "D:\\Khurram\\UTILS\\Pandora\\Images\\New folder";
	//cout << imFolderPath << endl;
}

OrderImages::OrderImages(String tPath)
{
	if (tPath.empty())
	{
		cout << "Please enter a valid patd. Switching to the default path." << endl;
		OrderImages::OrderImages();
		
	}
	else
	{
		imFolderPath = tPath;
		
	}
}

void OrderImages::readImFolderContents()
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(imFolderPath.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (strstr(ent->d_name, ".jpg"))
			{
				
				imNameList.push_back(ent->d_name);
				
			}
		}
		closedir(dir);
		
		(powerOfTwo((uint16)imNameList.size())) ? NUMBER_OF_PYRAMIDS = (uint8) log2(imNameList.size()) : NUMBER_OF_PYRAMIDS = (uint8)floor(log2(imNameList.size())) + 1;
		
	}
	else {
		
		cout << "Cannot open directory. Check if it is either read protected or an invalid path." << endl;
		return;
	}
}

bool OrderImages::powerOfTwo(uint16 x)
{
	return !(x == 0) && !( x & (x - 1) );
}

void OrderImages::buildImPyramid()
{
	namedWindow("Display window", WINDOW_AUTOSIZE);
	uint8 numImages = (uint8) imNameList.size();
	PYRAMID.resize(NUMBER_OF_PYRAMIDS, vector<Mat>(numImages, Mat::eye(3, 3, CV_8UC1)));
	
	string imName = "";
	for (uint8 ii = 0; ii < numImages; ii++)
	{
		imName = imFolderPath + "\\" + imNameList[ii];
		Mat imt_, im = imread(imName, CV_LOAD_IMAGE_GRAYSCALE);
		for (uint8 jj = 0; jj < NUMBER_OF_PYRAMIDS; jj++)
		{			
			resize(im, PYRAMID[jj][ii], Size(0, 0), (double)((double)(jj+1) / (double)NUMBER_OF_PYRAMIDS), (double)((double)(jj+1) / (double)NUMBER_OF_PYRAMIDS) , INTER_LINEAR );
		}
		imName = "";
	}
}