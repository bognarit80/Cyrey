#ifndef _CYREY_PIECE_HEADER
#define _CYREY_PIECE_HEADER

namespace Cyrey
{
	enum class PieceColor
	{
		Uncolored = 0,
		Red,
		Green,
		Blue,
		Yellow,
		Orange,
		White,
		Purple
	};

	class Piece
	{
	public:
		PieceColor mColor;
		unsigned int mFlags;
		bool mCanSwap;

		Piece() = default;

		Piece(PieceColor mColor)
			: mColor(mColor), mFlags(0), mCanSwap(true)
		{};
	};
}

#endif // !_CYREY_PIECE_HEADER
