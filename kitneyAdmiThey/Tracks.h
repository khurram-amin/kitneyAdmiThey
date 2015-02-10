#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>

#include "Utills.h"

using namespace std;
using namespace cv;

class Tracks
{
private:
	int NUM_OF_IMAGES;
	int NUM_OF_ADDED_IMAGES;
	int NUM_OF_TRACKS;
	vector<Track> _tracksAll; // will hold tracks for all images,
public:
	// Constructors
	Tracks();
	Tracks(const int numImages);
	//Tracks(const int queryImgNum, const int trainImgNum, const vector<KeyPoint>& queryKps, const vector<KeyPoint>& trainKps, const vector<DMatch>& goodMatches);

	// add tracks of iamge i
	void addTracksImg(const int queryImgNum, const int trainImgNum, const vector<KeyPoint>& queryKps, const vector<KeyPoint>& trainKps, const vector<DMatch>& goodMatches);

	// Set number of images
	void setNumImages(const int numImages);

	// Get number of images
	int getNumImages();

	// Returns total number of tracks currently holded by the structure
	int getTotalNumTracks();

	// Returns total number of images whose tracks have been added to this structure
	int getTotalNumAddedImages();

	// Prune/Remove tracks spanning over one image more than once
	void pruneTracksCyclic();

	// Prune/Remove duplicate tracks
	void pruneDuplicateTracks();

	// Prune/Remove tracks spanning over images less than the given number
	void pruneTracksLTnumIm(const int lessThan);
	

	// Return image having max number of tracks passing through it.
	int maxTrackImg();

	// Return NN image based on the given/input tracks
	int NNImgTracks(const vector<Track>& queryTracks);

	// Will plot & save the image after reconsructing all the tracks passing through image imNameList[imNum].
	void plotTracks(const int imNum,  const std::string& dirPath, const vector<std::string>& imNameList, const std::string& saveDirPath, const std::string& saveImName);

};