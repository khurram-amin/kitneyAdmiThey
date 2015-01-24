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