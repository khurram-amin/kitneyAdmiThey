
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
	bool computeHomographyRANSAC(const vector<KeyPoint>& im1_kp, const vector<KeyPoint>& im2_kp, const vector<DMatch>& matches, Mat& outH);
	Mat computeHMOCK(const vector<DMatch>&);
	void inOrOut(const vector<Point2f>& line, const vector<Point2f>& vectices2Label, const Point2f wMid, vector<bool>& outLabels);
	void normalOfLine(const vector<Point2f>& inLine, const Point2f& inPoint, Point2f& outNormal);
	bool intersBW2lines(const vector<Point2f>&, const vector<Point2f>&, Point2f&);
	void overlappingArea(const vector<Point2f>& C1, const Point2f C1mid, const vector<Point2f>& C2, vector<Point2f>& overlappedRegion);
	float computeOverlappedArea(const Size, const Size, const Mat);
	float computeAreaMOCK(uint8, uint8, Mat);
	void findNNimage(const vector<String>&, const uint8 level, vector<String>&);
	uint16 findStringidx(const vector<String>& inStrVec, const String& inStr);
	void findNNimagesMOCK();
};
