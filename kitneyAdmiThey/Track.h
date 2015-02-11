#include "stdafx.h"

#include "Utills.h"
#include "Ring.h"


using namespace std;
using namespace cv;


//Track will hold rings
class Track
{
public:
	unsigned int _TrackID;
	vector<Ring> _track;

	Track() : _TrackID(-1) {}
	Track(const int tID) : _TrackID(tID){}
	Track(const int tID, vector<Ring> tr) : _TrackID(tID), _track(tr){}

};
