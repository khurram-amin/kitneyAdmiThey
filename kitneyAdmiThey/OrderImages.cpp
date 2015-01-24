#include "stdafx.h"
#include "OrderImages.h"

OrderImages::OrderImages( )
{
	imFolderPath = "D:\\Khurram\\UTILS\\Pandora\\Images\\New folder";
	//cout << imFolderPath << endl;
}

OrderImages::OrderImages(String tPath)
{
	if ( tPath.empty() )
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