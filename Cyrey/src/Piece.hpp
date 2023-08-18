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
		Purple,
		Count
	};

	class Piece
	{
	public:
		PieceColor mColor;
		unsigned int mID;
		unsigned int mFlags;
		bool mCanSwap;
		bool mDragging;
		float mXDiff;
		float mYDiff;
		bool mMatched;

		Piece() = default;

		Piece(PieceColor mColor)
			: mColor(mColor), mID(sNextPieceID++), mFlags(0), mCanSwap(true), mDragging(false), mXDiff(0.0f), mYDiff(0.0f), mMatched(false)
		{};

	private:
		static inline unsigned int sNextPieceID = 1;
	};
	static constexpr const Piece gNullPiece = Piece();
}

#endif // !_CYREY_PIECE_HEADER
