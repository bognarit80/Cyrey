#ifndef CYREY_PIECEMATCHANIM_HEADER
#define CYREY_PIECEMATCHANIM_HEADER

#include "Piece.hpp"

namespace Cyrey
{
	class PieceMatchAnim
	{
	public:
		int mBoardX;
		int mBoardY;
		PieceColor mColor;
		float mOpacity;
		bool mDestroyed;
		constexpr static float cStartingOpacity = 0.75f;

		PieceMatchAnim(int x, int y, PieceColor color, bool destroyed) :
			mBoardX(x), mBoardY(y), mColor(color), mOpacity(cStartingOpacity), mDestroyed(destroyed) {};
	};
}

#endif // !CYREY_PIECEMATCHANIM_HEADER
