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
		cout << "-- Max dist : " << max_dist << endl;
		cout << "-- Min dist : " << min_dist << endl;

		for (uint16 k = 0; k < desp1.rows; k++)
		{
			if (outMatches[i][k].distance <= max(2 * min_dist, 0.99))
			{
				outGoodMatches[i].push_back(outMatches[i][k]);
			}
		}
		
	}
}


void OrderImages::matchDespMOCK(vector<vector<DMatch>>& outMatches, vector<vector<DMatch>>& outGoodMatches)
{
	findMatchesFLANN(DESCRIPTORS[0][0], DESCRIPTORS[0], outMatches, outGoodMatches);
}


Mat OrderImages::computeHomographyRANSAC(vector<KeyPoint> im1_kp, vector<KeyPoint> im2_kp, vector<DMatch> matches)
{
	vector<Point2f> im1_filtered, im2_filtered;
	for (uint16 i = 0; i < matches.size(); i++)
	{
		// Assumption: im1_kp is the first image i.e. the image who is compared with all other images.
		// Assumption: queryIdx belongs to first image i.e. the image who is compared with all other images.
		im1_filtered.push_back( im1_kp[ matches[i].queryIdx ].pt );
		im2_filtered.push_back( im2_kp[ matches[i].trainIdx ].pt );
	}

	Mat H = findHomography(im1_filtered, im2_filtered, CV_RANSAC);

	return H;
}