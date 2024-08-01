#ifndef CYREY_PIECEMATCHANIM_HEADER
#define CYREY_PIECEMATCHANIM_HEADER

#include <array>
#include "Piece.hpp"

namespace Cyrey
{
    struct AnimSparkle
    {
        float mRotationDeg;
        float mDirectionAngleDeg;
        float mDistance; /// Distance from the tile center, as percentage of tile size

        static constexpr int cMinSparkles = 4;
        static constexpr int cMaxSparkles = 16;
        static constexpr float cRotationPerSec = 540.0f;
        static constexpr float cSpeed = 25.0f; // mDistance added per second
    };

	class PieceMatchAnim
	{
	public:
		int mBoardX;
		int mBoardY;
		PieceColor mColor;
		float mOpacity;
		bool mDestroyed;
        std::array<AnimSparkle, AnimSparkle::cMaxSparkles> mSparkles;
        int mSparklesAmount;

		static constexpr float cStartingOpacity = 0.75f;

		PieceMatchAnim(int x, int y, PieceColor color, bool destroyed) :
			mBoardX(x), mBoardY(y), mColor(color), mOpacity(cStartingOpacity), mDestroyed(destroyed), mSparkles({}), mSparklesAmount(0) {};
	};
} // namespace Cyrey

#endif // !CYREY_PIECEMATCHANIM_HEADER
