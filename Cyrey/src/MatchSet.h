#ifndef _MATCHSET_CYREY_HEADER
#define _MATCHSET_CYREY_HEADER

#include "Piece.hpp"

namespace Cyrey
{
	class MatchSet
	{
	public:
		std::vector<Piece*> mPieces;
		//the swapped in piece(s), or the ones that falled in, important for deciding where specials are created
		std::vector<Piece*> mAddedPieces;
	};
}

#endif // !_MATCHSET_CYREY_HEADER
