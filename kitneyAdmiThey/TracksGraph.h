#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>

#include "Track.h"

using namespace std;
using namespace cv;

class TracksGraph
{
private:
	int NUM_OF_IMAGES; // Total number of images
	int NUM_OF_TRACKS; // Total tracks 
	vector<Track> _tracksAll; // will hold tracks
public:
	// Constructors
	TracksGraph(); // Deafult
	TracksGraph(const int numImages);


	// add tracks of image i
	void addTracksImg(const int img1Num, const int img2Num, const vector<KeyPoint>& kp1, const vector<KeyPoint>& kp2, const vector<DMatch>& goodMatches);

	// Check if the Ring exists in the tracks
	int checkRingExistence(const vector<Track>& tracksToSearchIn, const Ring& ringToCheck);

	// Set number of images
	void setNumImages(const int numImages);

	// Get number of images
	int getNumImages();

	// Returns total number of tracks currently holded by the structure
	int getTotalNumTracks();

	// Prune/Remove tracks spanning over one image more than once
	void pruneTracksCyclic();
	bool cyclicRinginTrack(vector<Track>::const_iterator inTrack, vector<Point2i>& outIdxPairVec);

	// Prune/Remove duplicate tracks
	void pruneDuplicateTracks();

	// Return Tracks which span over images more than the given number
	void pruneTracksLTnumIm(const int lessThan, vector<vector<Ring>>& outTracks);
	

	// Return image having max number of tracks passing through it.
	int maxTrackImg();

	// Return NN image based on the given/input rings
	int NNImgTracks(const vector<vector<Ring>>& queryTracks);

	// Will plot & save the image after reconsructing all the tracks passing through image imNameList[imNum].
	void plotTracks(const int imNum,  const std::string& dirPath, const vector<std::string>& imNameList, const std::string& saveDirPath, const std::string& saveImName);

};