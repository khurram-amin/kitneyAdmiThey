#include "stdafx.h"
#include "OrderImages.h"

// Default constructor. Sets the deafult image_folder path if none is prvided.
OrderImages::OrderImages()
{
	// Default folder path to load images
	imFolderPath = "D:\\Khurram\\UTILS\\Pandora\\Images\\New folder";
}

// Single input overloaded constructor. Allows the user to set the image_folder path while creating an object.
OrderImages::OrderImages(String tPath)
{
	DIR* dir;
	if (tPath.empty() || ((dir = opendir(tPath.c_str())) == NULL))
	{
		cout << "Please enter a valid patd. Switching to the default path." << endl;
		OrderImages::OrderImages();
	}
	else
	{
		imFolderPath = tPath;
		cout << "imageFolderPath set to: " << tPath << endl;
	}
}

// Reads the content of the image folder and writes the file name in imNameList
bool OrderImages::readImFolderContents()
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
		numImages = (uint8)imNameList.size();

	}
	else {
		cout << "Cannot open directory. Check if it is either read protected or an invalid path." << endl;
	}

	if ((numImages > 0) && (NUMBER_OF_PYRAMIDS > 0))
	{
		cout << "NUMBER_OF_IMAGES: " << (int)numImages << endl;
		cout << "NUMBER_OF_PYRAMID_LEVELS: " << (int)NUMBER_OF_PYRAMIDS << endl;
		return TRUE;
	}
	else
		return FALSE;
}

// Checks if a number/integer is a power of two or not.
bool OrderImages::powerOfTwo(uint16 x)
{
	return !(x == 0) && !(x & (x - 1));
}


bool OrderImages::buildImPyramid()
{
	if ((numImages > 0) && (NUMBER_OF_PYRAMIDS > 0))
	{
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
	else
	{
		cout << "It seems like you havent called read_image_folder_contenets yet." << endl;
	}

	if (PYRAMID.size() == NUMBER_OF_PYRAMIDS && PYRAMID[0].size() == numImages)
	{
		cout << "PYRAMID BUILT !!! " << endl;
		return TRUE;
	}
	else
		return FALSE;
}


bool OrderImages::buildSIFTPyramid()
{
	if (PYRAMID.size() == NUMBER_OF_PYRAMIDS && PYRAMID[0].size() == numImages)
	{
		SIFT siftObj;
		Mat im;
		KEYPOINTS.resize(NUMBER_OF_PYRAMIDS, vector<vector<KeyPoint>>(numImages, vector<KeyPoint>(1, KeyPoint())));
		DESCRIPTORS.resize(NUMBER_OF_PYRAMIDS, vector<Mat>(numImages, Mat::zeros(3, 3, CV_32FC1)));

		cout << "KPs & Descp computed for images: ";

		for (uint8 ii = 0; ii < numImages; ii++)
		{
			for (uint8 jj = 0; jj < NUMBER_OF_PYRAMIDS; jj++)
			{

				siftObj.operator()(PYRAMID[jj][ii], Mat(), KEYPOINTS[jj][ii], DESCRIPTORS[jj][ii]);
				//drawKeypoints(PYRAMID[jj][ii], KEYPOINTS[jj][ii], im);
				//string fileName = writePath + "\\kp_" + to_string((_ULonglong)jj) + "_" + imNameList[ii];
				//imwrite(fileName, im);
			} 
			cout << " " << (int)ii << " ";
		}
		cout << endl;
		cout << endl;

		cout << "SIFT keypoints & descriptor computed for whole PYRAMID !!! " << endl;
	}

	if (KEYPOINTS.size() == PYRAMID.size() && KEYPOINTS[0].size() == PYRAMID[0].size() && DESCRIPTORS.size() == PYRAMID.size() && DESCRIPTORS[0].size() == PYRAMID[0].size())
		return TRUE;
	else
		return FALSE;

}


bool OrderImages::findMatchesFLANN(Mat desp1, vector<Mat>& otherDesp, vector<vector<DMatch>>& outMatches, vector<vector<DMatch>>& outGoodMatches)
{
	if (desp1.rows <= 0 || desp1.cols <= 0 || otherDesp.size() <= 0)
		return false;

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
	return true;
}


void OrderImages::matchDespMOCK(vector<vector<DMatch>>& outMatches, vector<vector<DMatch>>& outGoodMatches)
{
	findMatchesFLANN(DESCRIPTORS[0][0], DESCRIPTORS[0], outMatches, outGoodMatches);
}


bool OrderImages::computeHomographyRANSAC(const vector<KeyPoint>& im1_kp, const vector<KeyPoint>& im2_kp, const vector<DMatch>& matches, Mat& outH)
{
	vector<Point2f> im1_filtered, im2_filtered;
	if ((matches.size() > PER_KP_MATCHES*im1_kp.size()) && (matches.size() > PER_KP_MATCHES*im2_kp.size()))
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
		((lineNormal).dot(vectices2Label[i] - lineMid) > -2e-1) ? outLabels[i] = true : outLabels[i] = false;
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
	outNormal.x = outNormal.x / (float)norm(outNormal);
	outNormal.y = outNormal.y / (float)norm(outNormal);
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
	for (uint8 i = 0; i < C1.size() - 1; i++)
	{
		refEdgeVW.push_back(C1[i]);		refEdgeVW.push_back(C1[i + 1]);
		inOrOut(refEdgeVW, EVW, C1mid, vCW_inOutStatus);

		for (uint8 j = 0; j < vCW_inOutStatus.size() - 1; j++)
		{
			if (vCW_inOutStatus[j])
				if (vCW_inOutStatus[j + 1])
					overlappedRegion.push_back(EVW[j + 1]);
				else
				{
					Point2f interPt_;
					vector<Point2f> L1, L2;
					L1.push_back(C1[i]);	L1.push_back(C1[i + 1]);
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
		if (overlappedRegion.size() < 1)
			break;
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
	overlappingArea(C1, Point2f((im1.height - 1) / 2, (im1.width - 1) / 2), C2t_, ROI);
	if (ROI.size() < 4)
		return -1;
	double areaC1 = im1.width * im1.height;
	double areaROI = contourArea(ROI);

	//return (float)areaROI;
	return (float)(100 * areaROI / areaC1);
}

float OrderImages::computeAreaMOCK(uint8 im1N, uint8 im2N, Mat H)
{
	Size im1, im2;
	im1 = PYRAMID[0][im1N].size();
	im2 = PYRAMID[0][im2N].size();
	return computeOverlappedArea(im1, im2, H);
}


void OrderImages::findNNimage(const vector<string>& inTODO, const uint8 level, vector<String>& outNNList)
{
	cout << endl;
	cout << "Computing Nearest Neighour Image . . . " << endl;
	vector<string> todo_ = inTODO;
	uint16 im1 = findStringidx(imNameList, todo_[0]);
	outNNList.push_back(todo_[0]);
	todo_.erase(todo_.begin());
	while (todo_.size() != 0)
	{
		cout << endl;
		cout << "current state of Todo_ list at the beginning of the while loop iteration ... " << endl;
		for (uint8 i = 0; i < todo_.size(); i++)
			cout << todo_[i] << "  ";
		cout << endl;
		cout << endl;

		cout << endl;
		cout << "current state of Todo_ list at the beginning of the while loop iteration ... " << endl;
		for (uint8 i = 0; i < outNNList.size(); i++)
			cout << outNNList[i] << "  ";
		cout << endl;
		cout << endl;

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
		uint16 best_match = -1;
		float prevBestOverlap = 0;

		for (uint8 i = 0; i < todo_.size(); i++)
		{
			Mat H;
			if (computeHomographyRANSAC(KEYPOINTS[level][im1], kp_[i], good_match[i], H))
			{

				float perOverlap_ = computeOverlappedArea(PYRAMID[level][im1].size(), pyr_[i].size(), H);
				cout << endl;
				cout << "Overlap between image " << imNameList[im1] << " and image " << todo_[i] << " is: " << perOverlap_ << endl;
				if (perOverlap_ > prevBestOverlap)
				{
					best_match = i;
					prevBestOverlap = perOverlap_;
				}
			}
		}
		if (todo_.size() == 1)
		{
			outNNList.push_back(todo_[0]);
			im1 = findStringidx(imNameList, todo_[0]);
			todo_.erase(todo_.begin());
			break;
		}

		outNNList.push_back(todo_[best_match]);
		im1 = findStringidx(imNameList, todo_[best_match]);
		todo_.erase(todo_.begin() + best_match);

	}
	for (uint8 i = 0; i < outNNList.size(); i++)
		cout << outNNList[i] << endl;
}

uint16 OrderImages::findStringidx(const vector<string>& inStrVec, const string& inStr)
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
	findNNimage(imNameList, (uint8)NUMBER_OF_PYRAMIDS - 2, outNNList);
}

bool OrderImages::computeSIFTim(string imDirPath, string imName, uint8 pyr_level)
{
	// verify if the imDirPath is a valid directory path. if no return an error.
	if (imDirPath.empty())
	{
		cout << "imDirPath is empty. " << imDirPath << "  computeSIFTim" << endl;
		return false;
	}

	if (!dirExists(imDirPath))
	{
		cout << "imDirPath does not exists. " << imDirPath << "  computeSIFTim" << endl;
		return false;
	}
	// verify if the imName image exists in the provided image directroy path. if no return an error.
	string imPath = imDirPath + "\\" + imName;

	if (!fileExists(imPath))
	{
		cout << "A file with imName = " << imPath << " does not exists. computeSIFTim" << endl;
		return false;
	}

	Mat im = imread(imPath, CV_LOAD_IMAGE_GRAYSCALE);
	if (im.empty())
	{
		cout << "invalid image file. " << imPath << "  computeSIFTim" << endl;
		return false;
	}
	resize(im, im, Size(0, 0), (double)((double)(pyr_level) / (double)NUMBER_OF_PYRAMIDS), (double)((double)(pyr_level) / (double)NUMBER_OF_PYRAMIDS), INTER_LINEAR);
	//resize(im, PYRAMID[jj][ii], Size(0, 0), (double)((double)(jj + 1) / (double)NUMBER_OF_PYRAMIDS), (double)((double)(jj + 1) / (double)NUMBER_OF_PYRAMIDS), INTER_LINEAR);

	// check if the +private directory exists in the provided imDirPath. if no, create it.
	string pvt_dir = imDirPath + "\\+private";
	if (!dirExists(pvt_dir))
	{
		string make = "mkdir " + pvt_dir;
		system(make.c_str());
	}
	// check if the imName directory exists in the +private directory. if no create it.
	string im_pvt_dir = imDirPath + "\\+private\\" + imName;
	if (!dirExists(im_pvt_dir))
	{
		string make = "mkdir " + im_pvt_dir;
		system(make.c_str());
	}
	// check if the imSize.kp file exists in the +imName directory. if yes load it. parse it. else compute kps and save it in two seperate files.
	string kp_f = to_string(im.rows) + "_" + to_string(im.cols);
	string kp_fname = im_pvt_dir + "\\kp_" + kp_f + ".yml";
	string descp_fname = im_pvt_dir + "\\descp_" + kp_f + ".yml";
	Mat kpsR, descpR;
	vector<KeyPoint> kpsR_conv;
	if (fileExists(kp_fname))
	{
		// load and parse the kp file and assign it to the output kp variable
		cv::FileStorage fs1(kp_fname, cv::FileStorage::READ);
		fs1["KeyPoint Matrix"] >> kpsR;
		fs1.release();
		Point2f temp_;
		KeyPoint tempKP_;
		for (int i = 0; i < kpsR.rows; i++)
		{
			temp_ = kpsR.at<Point2f>(i);
			tempKP_.pt = temp_;
			kpsR_conv.push_back(tempKP_);
		}

	}
	else
	{
		// compute kps and save it in the file kp_fname
		//siftObj.operator()(PYRAMID[jj][ii], Mat(), KEYPOINTS[jj][ii], DESCRIPTORS[jj][ii]);
		SIFT siftobj;
		vector<KeyPoint> kps;
		Mat descp;
		siftobj.operator()(im, Mat(), kps, descp, false);
		std::vector<cv::KeyPoint>::iterator it;
		std::vector<cv::Point2f> points;

		for (it = kps.begin(); it != kps.end(); it++)
		{
			points.push_back(it->pt);
		}
		Mat kpsMat(points);

		cv::FileStorage fs1(kp_fname, cv::FileStorage::WRITE);
		time_t rawtime;
		assert(fs1.isOpened());
		time(&rawtime);
		fs1 << "Computing Date" << asctime(localtime(&rawtime));
		fs1 << "KeyPoint Matrix" << kpsMat;
		fs1.release();

		time(&rawtime);
		cv::FileStorage fs2(descp_fname, FileStorage::WRITE);
		fs2 << "Computing Date" << asctime(localtime(&rawtime));
		fs2 << "Descriptor Matrix" << descp;
		fs2.release();

	}

	// check if the +imSize.descp file exists in the +imName directory. if yes load it. parse it. else compute descriptor for given kps.

	if (fileExists(descp_fname))
	{
		// load and parse the descp file and assign it to the output descp variable
		cv::FileStorage fs1(descp_fname, cv::FileStorage::READ);
		fs1["Descriptor Matrix"] >> descpR;
		fs1.release();
	}
	else
	{
		// compute descp for the above kps and save it in the file descp_fname
		SIFT siftobj;
		siftobj.operator()(im, Mat(), kpsR_conv, descpR, false);
		time_t rawtime;
		time(&rawtime);
		cv::FileStorage fs2(descp_fname, FileStorage::WRITE);
		fs2 << "Computing Date" << asctime(localtime(&rawtime));
		fs2 << "Descriptor Matrix" << descpR;
		fs2.release();

	}
}

bool OrderImages::dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

bool OrderImages::fileExists(const std::string& fileName_in)
{
	DWORD ftyp = GetFileAttributesA(fileName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_ARCHIVE)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}