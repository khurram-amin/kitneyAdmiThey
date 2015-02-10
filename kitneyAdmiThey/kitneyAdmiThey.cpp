// kitneyAdmiThey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OrderImages.h"
#include "Tracks.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//OrderImages oIm("D:\\Khurram\\Copy\\Copy\\Thesis\\ShortestPath\\Shape from Shadow - Super=8000\\Images");
	//OrderImages oIm("D:\\Khurram\\UTILS\\Pandora\\Images\\NewsPaper\\HR");
	//OrderImages oIm("D:\\Khurram\\UTILS\\Pandora\\Images\\REDC");
	OrderImages oIm("D:\\Khurram\\UTILS\\Pandora\\Images\\New folder");
	bool sIm = oIm.readImFolderContents();

	//oIm.computeSIFTim("D:\\Khurram\\UTILS\\Pandora\\Images\\NewsPaper\\HR", "002.JPG", 1);
	bool sPy = oIm.buildImPyramid();
	bool sSPy = oIm.buildSIFTPyramid();

	vector<vector<DMatch>> match, good_match;
	oIm.matchDespMOCK(match, good_match);

	Mat H = oIm.computeHMOCK(good_match[0]);

	//float oA = oIm.computeAreaMOCK(0, 0, H);
	
	//oIm.findNNimagesMOCK();
	Tracks t1;
	t1.setNumImages(4);
	cout << t1.getNumImages() << endl;

	//t1.addTracksImg(0, 0, oIm.KEYPOINTS[0][0], oIm.KEYPOINTS[0][0], good_match[0]);
	t1.addTracksImg(0, 1, oIm.KEYPOINTS[0][0], oIm.KEYPOINTS[0][1], good_match[1]);

	waitKey(0);
	return 0;
}

