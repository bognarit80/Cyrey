#ifndef CYREY_PIECEDROPANIM_HEADER
#define CYREY_PIECEDROPANIM_HEADER

namespace Cyrey
{
	class PieceDropAnim
	{
	public:
		int mBoardCol;
		float mOpacity;
		static constexpr float cStartingOpacity = 0.80f;

		explicit PieceDropAnim(int col) : mBoardCol(col), mOpacity(cStartingOpacity) {};
	};
} // namespace Cyrey

#endif // !CYREY_PIECEDROPANIM_HEADER
