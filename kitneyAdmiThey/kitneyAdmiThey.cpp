// kitneyAdmiThey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OrderImages.h"


int _tmain(int argc, _TCHAR* argv[])
{
	//OrderImages oIm("D:\\Khurram\\Copy\\Copy\\Thesis\\ShortestPath\\Shape from Shadow - Super=8000\\Images");
	OrderImages oIm;
	oIm.readImFolderContents();
	oIm.buildImPyramid();

	return 0;
}

