// kitneyAdmiThey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OrderImages.h"


int _tmain(int argc, _TCHAR* argv[])
{
	//OrderImages oIm("D:\\Khurram\\Copy\\Copy\\Thesis\\ShortestPath\\Shape from Shadow - Super=8000\\Images");
	//OrderImages oIm("D:\\Khurram\\UTILS\\Pandora\\Images\\NewsPaper\\LR");
	OrderImages oIm("D:\\Khurram\\UTILS\\Pandora\\Images\\REDC");
	bool sIm = oIm.readImFolderContents();
	bool sPy = oIm.buildImPyramid();
	bool sSPy = oIm.buildSIFTPyramid();

	/*vector<vector<DMatch>> match, good_match;
	oIm.matchDespMOCK(match, good_match);

	Mat H = oIm.computeHMOCK(good_match[0]);

	float oA = oIm.computeAreaMOCK(0, 0, H);*/
	
	oIm.findNNimagesMOCK();

	waitKey(0);
	return 0;
}

