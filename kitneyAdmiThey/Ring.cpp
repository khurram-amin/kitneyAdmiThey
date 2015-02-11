#include "stdafx.h"

#include "Ring.h"

bool const Ring::operator== ( const Ring& tR) const
{
	return (_imgIdx == tR._imgIdx && _kpIdx == tR._kpIdx);
}
