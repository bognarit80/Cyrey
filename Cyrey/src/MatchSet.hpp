#ifndef MATCHSET_CYREY_HEADER
#define MATCHSET_CYREY_HEADER

#include <vector>
#include "Piece.hpp"

namespace Cyrey
{
	class MatchSet
	{
	public:
		std::vector<Piece*> mPieces;
		/// The swapped in piece(s), or the ones that falled in, important for deciding where specials are created
		std::vector<Piece*> mAddedPieces;
	};
} // namespace Cyrey

#endif // !MATCHSET_CYREY_HEADER
