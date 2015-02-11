// kitneyAdmiThey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OrderImages.h"
#include "TracksGraph.h"

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
	
	TracksGraph tracksPlanar;
	tracksPlanar.addTracksImg(0, 1, oIm.KEYPOINTS[0][0], oIm.KEYPOINTS[0][1], good_match[1]);
	tracksPlanar.addTracksImg(0, 2, oIm.KEYPOINTS[0][0], oIm.KEYPOINTS[0][2], good_match[2]);
	tracksPlanar.addTracksImg(0, 3, oIm.KEYPOINTS[0][0], oIm.KEYPOINTS[0][3], good_match[3]);
	tracksPlanar.addTracksImg(0, 0, oIm.KEYPOINTS[0][0], oIm.KEYPOINTS[0][0], good_match[0]);

	waitKey(0);
	return 0;
}

