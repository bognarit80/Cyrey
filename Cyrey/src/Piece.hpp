#ifndef CYREY_PIECE_HEADER
#define CYREY_PIECE_HEADER

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

	enum PieceFlag
	{
		Bomb = 0x1,
		Lightning = 0x2,
		Hypercube = 0x4
	};

	class Piece
	{
	public:
		PieceColor mColor;
		PieceColor mOldColor; // for hypercubes
		unsigned int mID;
		unsigned int mFlags;
		bool mCanSwap;
		bool mDragging;
		float mXDiff;
		float mYDiff;
		bool mMatched;
		bool mImmunity;
		int mBoardX;
		int mBoardY;

		Piece() = default;

		explicit Piece(PieceColor color)
			: mColor(color), mOldColor(color), mID(sNextPieceID++), mFlags(0), mCanSwap(true), mDragging(false),
			mXDiff(0.0f), mYDiff(0.0f), mMatched(false), mImmunity(false), mBoardX(0), mBoardY(0)
		{};

		[[nodiscard]] bool IsFlagSet(unsigned int flag) const;
		void SetFlag(unsigned int flag);
		void Bombify();
		void Lightningify();
		void Hypercubify();

	private:
		static inline unsigned int sNextPieceID = 1;
	};
	static constexpr auto gNullPiece = Piece();
} // namespace Cyrey

#endif // !CYREY_PIECE_HEADER
