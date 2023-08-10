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
		bool mDragging;
		float mXDiff;
		float mYDiff;

		Piece() = default;

		Piece(PieceColor mColor)
			: mColor(mColor), mFlags(0), mCanSwap(true), mDragging(false), mXDiff(0.0f), mYDiff(0.0f)
		{};
	};
}

#endif // !_CYREY_PIECE_HEADER
