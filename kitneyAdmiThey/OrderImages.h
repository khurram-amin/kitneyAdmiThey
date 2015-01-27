
#include "stdafx.h"
#include <iostream>
#include <stdint.h>
#include <dirent.h>
#include <string>
#include <cmath>
#include <vector>


#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\flann\flann.hpp>
#include <opencv2\calib3d\calib3d.hpp>

using namespace std;
using namespace cv;

#define uint8 uint8_t
#define uint16 uint16_t

class OrderImages 
{
private:
	string imFolderPath;
	//string writePath = "D:\\Khurram\\Copy\\Copy\\Thesis\\ShortestPath\\Shape from Shadow - Super=8000\\Images\\KEYPOINTS_";
	vector<std::string> imNameList;
	uint8 NUMBER_OF_PYRAMIDS;
	uint8 numImages;
	vector< vector< Mat > > PYRAMID;
	vector< vector< vector<KeyPoint> > > KEYPOINTS;
	vector< vector< Mat > > DESCRIPTORS;

public:
	OrderImages();
	OrderImages(String);
	void readImFolderContents();
	bool powerOfTwo(uint16);
	void buildImPyramid();
	void buildSIFTPyramid();
	void findMatchesFLANN(Mat, vector<Mat>&, vector<vector<DMatch>>&, vector<vector<DMatch>>&);
	void matchDespMOCK(vector<vector<DMatch>>&, vector<vector<DMatch>>&);
	Mat computeHomographyRANSAC(const vector<KeyPoint>& im1_kp, const vector<KeyPoint>& im2_kp, const vector<DMatch>& matches);
	Mat computeHMOCK(const vector<DMatch>&);

};
