#include "stdafx.h"
#include "TracksGraph.h"

// Constructors

// Deafult Empty constructor
TracksGraph::TracksGraph() : NUM_OF_IMAGES(-1), NUM_OF_TRACKS(-1){}

// 
TracksGraph::TracksGraph(const int numImages) : NUM_OF_IMAGES(numImages), NUM_OF_TRACKS(-1) {}


// add tracks of iamge i
void TracksGraph::addTracksImg(const int img1Num, const int img2Num, const vector<KeyPoint>& kp1, const vector<KeyPoint>& kp2, const vector<DMatch>& goodMatches)
{
	unsigned int numMatches = goodMatches.size();
	for (unsigned int i = 0; i < numMatches; i++)
	{
		Ring r1_(img1Num, goodMatches[i].queryIdx, kp1[goodMatches[i].queryIdx]);
		Ring r2_(img2Num, goodMatches[i].trainIdx, kp2[goodMatches[i].trainIdx]);
		Track t1_;
		t1_._track.push_back(r1_);
		t1_._track.push_back(r2_);

		int r1Exists = checkRingExistence(_tracksAll, r1_);
		int r2Exists = checkRingExistence(_tracksAll, r2_);

		if (r1Exists == r2Exists && r1Exists == -1)
		{
			//none of the rings exists, make a track consisting of r1_ and r2_ and push banck in _tracksAll
			_tracksAll.push_back(t1_);
		}
		else if (r1Exists == r2Exists && r1Exists != -1)
		{
			//both of the rings exists in same track, so do nothing
		}
		else if (r1Exists == -1 && r2Exists != -1)
		{
			//Ring2 Exists in track r2Exists, push Ring1 in track number r2Exists
			_tracksAll[r2Exists]._track.push_back(r1_);
		}
		else if (r1Exists != -1 && r2Exists == -1)
		{
			//Ring1 Exists in track r1Exists, push Ring2 in Track number r1Exists
			_tracksAll[r1Exists]._track.push_back(r2_);
		}
	}
		
		
}


int TracksGraph::checkRingExistence(const vector<Track>& tracksToSearchIn, const Ring& ringToCheck)
{
	int numTracks = tracksToSearchIn.size();
	for (unsigned int i = 0; i < numTracks; i++)
	{
		int numRings = tracksToSearchIn[i]._track.size();
		for (unsigned int j = 0; j < numRings; j++)
		{
			// Check if current ring in the track[i] is equal to the ringToCheck
			if ( tracksToSearchIn[i]._track[j] == ringToCheck)
				return i;
		}
	}
	return -1;
}


void TracksGraph::setNumImages(const int numImages)
{
	NUM_OF_IMAGES = numImages;
	NUM_OF_TRACKS = 0;
	_tracksAll.resize(1, Track());
}


int TracksGraph::getNumImages()
{
	return NUM_OF_IMAGES;
}


void TracksGraph::pruneTracksCyclic()
{
	// For all tracks in _tracksAll, check iff any track spans a-single image more than once
	// if it does: check if the rings are same
	//				if are same: then remove one of the rings
	//				if are not : then remove both of the rings
	// if it doesn't: go to next track
	vector<Track>::const_iterator allT_it;
	for ( allT_it = _tracksAll.begin(); allT_it != _tracksAll.end(); allT_it++ )
	{
		vector<Point2i> outIdx;
		bool dup_stat = cyclicRinginTrack(allT_it, outIdx);
		if (dup_stat)
		{
			// Check if both rings are the same
			if (allT_it->_track[outIdx[0].x] == allT_it->_track[outIdx[0].y])
			{
				
			}
		}
	}
}


bool TracksGraph::cyclicRinginTrack(vector<Track>::const_iterator inTrack, vector<Point2i>& outIdxPairVec)
{
	bool returnValue = false;
	vector<Ring>::const_iterator track_it, ring_it;
	for (track_it = inTrack->_track.begin(); track_it != inTrack->_track.end(); track_it++)
	{
		for (ring_it = track_it + 1; ring_it != inTrack->_track.end(); ring_it++)
		{
			if (track_it == ring_it)
			{
				returnValue = true;
				int index1 = distance( inTrack->_track.begin(), track_it );
				int index2 = distance( inTrack->_track.begin(), ring_it  );
				outIdxPairVec.push_back( Point2i(index1, index2) );
			}
		}
	}
	return returnValue;
}