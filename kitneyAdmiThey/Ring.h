#include "stdafx.h"
#include <opencv2\core\core.hpp>
#include <opencv2\features2d\features2d.hpp>



using namespace std;
using namespace cv;

//Ring will hold one ring of the chain
class Ring
{
public:
	int _imgIdx; // Image Number
	int _kpIdx; //  KeyPoint Index
	KeyPoint _KP;	// Query KeyPoint

	Ring() : _imgIdx(-1), _kpIdx(-1), _KP(KeyPoint()) {}
	Ring(const int imIdx, KeyPoint KP) : _imgIdx(imIdx), _kpIdx(-1), _KP(KP) {}
	Ring(const int imIdx, const int kpIdx, KeyPoint KP) : _imgIdx(imIdx), _kpIdx(kpIdx), _KP(KP) {}

	const bool operator== (const Ring&) const;
};

