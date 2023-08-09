#ifndef _CYREY_BOARD_HEADER
#define _CYREY_BOARD_HEADER

#include "raylib-cpp.hpp"
#include "Piece.hpp"
#include "CyreyApp.hpp"
#include <memory>

namespace Cyrey
{
	class CyreyApp;

	class Board
	{
	public:
		int mWidth;
		int mHeight;
		int mXOffset;
		int mYOffset;
		int mTileSize;
		float mBoardAlpha;
		std::vector<std::vector<Piece>> mBoard;
		CyreyApp* mApp;
		int mZoomPct;

		Board() = default;
		Board(int width, int height) :
			mWidth(width), mHeight(height) {};
		Board(int size) : Board(size, size) {};

		void Init();
		void Update();
		void Draw();
		static std::vector<std::vector<Piece>> ParseBoardString(const char*);

	private:
		void DrawCheckerboard();
		void DrawPieces();
		void DrawHoverSquare();
	};
}
	
#endif // #_CYREY_BOARD_HEADER