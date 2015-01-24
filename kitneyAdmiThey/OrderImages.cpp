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
		//cout << imFolderPath << endl;
	}
	else
	{
		imFolderPath = tPath;
		//cout << imFolderPath << endl;
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
				//printf("%s\n", ent->d_name);
				imNameList.push_back(ent->d_name);
				/*for (uint8 i = 0; i < imNameList.size(); i++)
				{
					cout << imNameList[i] << endl;
				}*/
			}
		}
		closedir(dir);
		//cout << imNameList.size() << endl;
		(powerOfTwo((uint16)imNameList.size())) ? NUMBER_OF_PYRAMIDS = (uint8) log2(imNameList.size()) : NUMBER_OF_PYRAMIDS = (uint8)floor(log2(imNameList.size())) + 1;
		//cout << (int) NUMBER_OF_PYRAMIDS << endl;
	}
	else {
		/* could not open directory */
		cout << "Cannot open directory. Check if it is either read protected or an invalid path." << endl;
		return;
	}
}

bool OrderImages::powerOfTwo(uint16 x)
{
	return !(x == 0) && !( x & (x - 1) );
}