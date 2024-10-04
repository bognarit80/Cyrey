#ifndef CYREY_SWAPANIM_HEADER
#define CYREY_SWAPANIM_HEADER
#include "SwapDirection.hpp"

namespace Cyrey
{
	struct SwapAnim
	{
		int mRow;
		int mCol;
		SwapDirection mDirection { SwapDirection::None };
		float mOpacity { cStartingOpacity };

		static constexpr float cStartingOpacity = 0.80f;
	};
} // namespace Cyrey

#endif // CYREY_SWAPANIM_HEADER
