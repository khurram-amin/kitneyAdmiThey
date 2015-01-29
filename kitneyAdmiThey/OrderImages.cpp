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
			if ((strstr(ent->d_name, ".JPG")) || (strstr(ent->d_name, ".jpg")))
			{

				imNameList.push_back(ent->d_name);

			}
		}
		closedir(dir);

		(powerOfTwo((uint16)imNameList.size())) ? NUMBER_OF_PYRAMIDS = (uint8)log2(imNameList.size()) : NUMBER_OF_PYRAMIDS = (uint8)floor(log2(imNameList.size())) + 1;

	}
	else {

		cout << "Cannot open directory. Check if it is either read protected or an invalid path." << endl;
		return;
	}
}


bool OrderImages::powerOfTwo(uint16 x)
{
	return !(x == 0) && !(x & (x - 1));
}


void OrderImages::buildImPyramid()
{
	numImages = (uint8)imNameList.size();
	PYRAMID.resize(NUMBER_OF_PYRAMIDS, vector<Mat>(numImages, Mat::eye(3, 3, CV_8UC1)));

	string imName = "";
	for (uint8 ii = 0; ii < numImages; ii++)
	{
		imName = imFolderPath + "\\" + imNameList[ii];
		Mat im = imread(imName, CV_LOAD_IMAGE_GRAYSCALE);
		for (uint8 jj = 0; jj < NUMBER_OF_PYRAMIDS; jj++)
		{
			resize(im, PYRAMID[jj][ii], Size(0, 0), (double)((double)(jj + 1) / (double)NUMBER_OF_PYRAMIDS), (double)((double)(jj + 1) / (double)NUMBER_OF_PYRAMIDS), INTER_LINEAR);
		}
		imName = "";
	}
}


void OrderImages::buildSIFTPyramid()
{
	SIFT siftObj;
	Mat im;
	KEYPOINTS.resize(NUMBER_OF_PYRAMIDS, vector<vector<KeyPoint>>(numImages, vector<KeyPoint>(1, KeyPoint())));
	DESCRIPTORS.resize(NUMBER_OF_PYRAMIDS, vector<Mat>(numImages, Mat::zeros(3, 3, CV_32FC1)));
	for (uint8 ii = 0; ii < numImages; ii++)
	{
		for (uint8 jj = 0; jj < NUMBER_OF_PYRAMIDS; jj++)
		{

			siftObj.operator()(PYRAMID[jj][ii], Mat(), KEYPOINTS[jj][ii], DESCRIPTORS[jj][ii]);
			drawKeypoints(PYRAMID[jj][ii], KEYPOINTS[jj][ii], im);
			//string fileName = writePath + "\\kp_" + to_string((_ULonglong)jj) + "_" + imNameList[ii];
			//imwrite(fileName, im);
		}
	}
}


void OrderImages::findMatchesFLANN(Mat desp1, vector<Mat>& otherDesp, vector<vector<DMatch>>& outMatches, vector<vector<DMatch>>& outGoodMatches)
{
	FlannBasedMatcher fMatcherObj;
	uint8 numOfImages_atClevel = (uint8)otherDesp.size();
	outMatches.resize(numOfImages_atClevel, vector<DMatch>(1, DMatch()));
	outGoodMatches.resize(numOfImages_atClevel, vector<DMatch>(1, DMatch()));
	for (uint8 i = 0; i < numOfImages_atClevel; i++)
	{
		fMatcherObj.match(desp1, otherDesp[i], outMatches[i]);
		double max_dist = 0; double min_dist = 100;
		for (uint16 j = 0; j < desp1.rows; j++)
		{
			double dist = outMatches[i][j].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}
		
		for (uint16 k = 0; k < desp1.rows; k++)
		{
			if (outMatches[i][k].distance <= max(3 * min_dist, 0.99))
			{
				outGoodMatches[i].push_back(outMatches[i][k]);
			}
		}
		
	}
	for (uint16 i = 0; i < outGoodMatches.size(); i++)
	{
		outMatches[i].erase(outMatches[i].begin());
		outGoodMatches[i].erase(outGoodMatches[i].begin());
	}
}


void OrderImages::matchDespMOCK(vector<vector<DMatch>>& outMatches, vector<vector<DMatch>>& outGoodMatches)
{
	findMatchesFLANN(DESCRIPTORS[0][0], DESCRIPTORS[0], outMatches, outGoodMatches);
}


bool OrderImages::computeHomographyRANSAC(const vector<KeyPoint>& im1_kp, const vector<KeyPoint>& im2_kp, const vector<DMatch>& matches, Mat& outH)
{
	vector<Point2f> im1_filtered, im2_filtered;
	if ((matches.size() > 0.1*im1_kp.size()) && (matches.size() > 0.1*im2_kp.size()))
	{
		for (uint16 i = 0; i < matches.size(); i++)
		{
			// Assumption: im1_kp is the first image i.e. the image who is compared with all other images.
			// Assumption: queryIdx belongs to first image i.e. the image who is compared with all other images.
			im1_filtered.push_back(im1_kp[matches[i].queryIdx].pt);
			im2_filtered.push_back(im2_kp[matches[i].trainIdx].pt);
		}

		outH = findHomography(im1_filtered, im2_filtered, CV_RANSAC);

		return true;
	}
	else
		return false;
}


Mat OrderImages::computeHMOCK(const vector<DMatch>& matches)
{
	Mat H;
	computeHomographyRANSAC(KEYPOINTS[0][0], KEYPOINTS[0][0], matches, H);
	return H;
}


void OrderImages::inOrOut(const vector<Point2f>& line, const vector<Point2f>& vectices2Label, const Point2f wMid, vector<bool>& outLabels)
{
	Point2f lineNormal;
	normalOfLine(line, wMid, lineNormal);
	Point2f lineMid((line[0] + line[1]).x / 2, (line[0] + line[1]).y / 2);

	outLabels.resize(vectices2Label.size(), false);
	for (uint8 i = 0; i < vectices2Label.size() - 1; i++)
	{
		( (lineNormal).dot(vectices2Label[i] - lineMid) > -2e-1 ) ? outLabels[i] = true : outLabels[i] = false;
 	}
	outLabels[outLabels.size() - 1] = outLabels[0];
}


void OrderImages::normalOfLine(const vector<Point2f>& inLine, const Point2f& inPoint, Point2f& outNormal)
{
	Point2f P = inLine[0];
	Point2f Q = inLine[1];
	Point2f mid = Q + P;
	mid.x = mid.x / 2; // (float)norm(diff);
	mid.y = mid.y / 2; // (float)norm(diff);

	outNormal = inPoint - mid;
	outNormal.x = outNormal.x / (float) norm(outNormal);
	outNormal.y = outNormal.y / (float) norm(outNormal);
}


bool OrderImages::intersBW2lines(const vector<Point2f>& L1, const vector<Point2f>& L2, Point2f& outIntersection)
{
	Point2f lengthL1 = L1[1] - L1[0];
	Point2f lengthL2 = L2[1] - L2[0];
	Point2f diffX = L2[0] - L1[0];
	float cross = lengthL1.x * lengthL2.y - lengthL1.y * lengthL2.x;
	if (abs(cross) < 1e-8)
	{
		return false;
	}
	float t = (diffX.x * lengthL2.y - diffX.y * lengthL2.x) / cross;
	outIntersection = L1[0] + t * lengthL1;
	return true;
}


void OrderImages::overlappingArea(const vector<Point2f>& C1, const Point2f C1mid, const vector<Point2f>& C2, vector<Point2f>& overlappedRegion)
{
	vector<Point2f> EVW = C2;
	vector < bool > vCW_inOutStatus;
	vector<Point2f> refEdgeVW;
	// For each edge of the viewing window
	for (uint8 i = 0; i < C1.size()-1; i++)
	{
		refEdgeVW.push_back(C1[i]);		refEdgeVW.push_back(C1[i+1]);
		inOrOut(refEdgeVW, EVW, C1mid, vCW_inOutStatus);

		for (uint8 j = 0; j < vCW_inOutStatus.size()-1; j++)
		{
			if (vCW_inOutStatus[j])
				if (vCW_inOutStatus[j + 1])
					overlappedRegion.push_back(EVW[j + 1]);
				else
				{
					Point2f interPt_;
					vector<Point2f> L1, L2;
					L1.push_back(C1[i]);	L1.push_back(C1[i+1]);
					L2.push_back(EVW[j]);	L2.push_back(EVW[j + 1]);
					intersBW2lines(L1, L2, interPt_);
					overlappedRegion.push_back(interPt_);
				}
			else
			{
				if (vCW_inOutStatus[j + 1])
				{
					Point2f interPt_;
					vector<Point2f> L1, L2;
					L1.push_back(C1[i]);	L1.push_back(C1[i + 1]);
					L2.push_back(EVW[j]);	L2.push_back(EVW[j + 1]);
					intersBW2lines(L1, L2, interPt_);
					overlappedRegion.push_back(interPt_);
					overlappedRegion.push_back(EVW[j + 1]);
				}
			}
		}
		overlappedRegion.push_back(overlappedRegion[0]);
		refEdgeVW.clear();
		EVW.clear();
		EVW = overlappedRegion;
		overlappedRegion.clear();
		vCW_inOutStatus.clear();

	}
	overlappedRegion = EVW;
}


float OrderImages::computeOverlappedArea(const Size im1, const Size im2, const Mat H)
{
	vector<Point2f> C1, C2, C2t_;
	C1.push_back(Point2f(0, 0));	C1.push_back(Point2f(0, im1.width - 1));	C1.push_back(Point2f(im1.height - 1, im1.width - 1));	C1.push_back(Point2f(im1.height - 1, 0));	C1.push_back(Point2f(0, 0));
	C2.push_back(Point2f(0, 0));	C2.push_back(Point2f(0, im2.width - 1));	C2.push_back(Point2f(im2.height - 1, im2.width - 1));	C2.push_back(Point2f(im2.height - 1, 0));	C2.push_back(Point2f(0, 0));

	perspectiveTransform(C2, C2t_, H);

	vector<Point2f> ROI;
	overlappingArea(C1, Point2f( (im1.height - 1 ) / 2, ( im1.width - 1 ) / 2 ), C2t_, ROI);
	
	double areaC1 = im1.width * im1.height;
	double areaROI = contourArea(ROI);

	//return (float)areaROI;
	return (float) ( 100 * areaROI / areaC1 );
}

float OrderImages::computeAreaMOCK(uint8 im1N, uint8 im2N, Mat H)
{
	Size im1, im2;
	im1 = PYRAMID[0][im1N].size();
	im2 = PYRAMID[0][im2N].size();
	return computeOverlappedArea(im1, im2, H);
}


void OrderImages::findNNimage(const vector<String>& inTODO, const uint8 level, vector<String>& outNNList)
{
	vector<String> todo_ = inTODO;
	uint16 im1 = findStringidx(imNameList, todo_[0]);
	outNNList.push_back(todo_[0]);
	todo_.erase(todo_.begin());
	while (todo_.size() != 0)
	{
		vector< Mat > desp_, pyr_;
		vector<vector<KeyPoint>> kp_;
		desp_.resize(todo_.size(), Mat::zeros(3, 3, CV_32FC1));
		pyr_.resize(todo_.size(), Mat::zeros(3, 3, CV_32FC1));
		kp_.resize(todo_.size(), vector<KeyPoint>(1, KeyPoint()));
		uint16 imIdx_;
		for (uint8 i = 0; i < todo_.size(); i++)
		{
			imIdx_ = findStringidx(imNameList, todo_[i]);
			pyr_[i] = PYRAMID[level][imIdx_];
			desp_[i] = DESCRIPTORS[level][imIdx_];
			kp_[i] = KEYPOINTS[level][imIdx_];
		}
		vector<vector<DMatch>> match, good_match;
		findMatchesFLANN(DESCRIPTORS[level][im1], desp_, match, good_match);
		uint16 best_match;
		float prevBestOverlap = 0;

		for (uint8 i = 0; i < todo_.size(); i++)
		{
			Mat H;
			if (computeHomographyRANSAC(KEYPOINTS[level][im1], kp_[i], good_match[i], H))
			{

				float perOverlap_ = computeOverlappedArea(PYRAMID[level][im1].size(), pyr_[i].size(), H);
				cout << perOverlap_ << endl;
				if (perOverlap_ > prevBestOverlap)
				{
					best_match = i;
					prevBestOverlap = perOverlap_;
				}
			}
		}
		outNNList.push_back(todo_[best_match]);
		im1 = findStringidx(imNameList, todo_[best_match]);
		todo_.erase(todo_.begin() + best_match);

	} 
	for (uint8 i = 0; i < outNNList.size(); i++)
		cout << outNNList[i] << endl;
}

uint16 OrderImages::findStringidx(const vector<string>& inStrVec, const String& inStr)
{
	for (uint16 i = 0; i < inStrVec.size(); i++)
	{
		if ((inStrVec[i].size() == inStr.size()) && (equal(inStrVec[i].begin(), inStrVec[i].end(), inStr.begin())))
		{
			return i;
		}
	}
	return -1;
}

void OrderImages::findNNimagesMOCK()
{
	vector<String> outNNList;
	findNNimage(imNameList, 1, outNNList);
}