#ifndef _CYREY_BOARD_HEADER
#define _CYREY_BOARD_HEADER

#include "raylib-cpp.hpp"
#include "Piece.hpp"
#include "CyreyApp.hpp"
#include <memory>
#include <optional>

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
		int mTileInset;
		float mBoardAlpha;
		std::vector<std::vector<Piece>> mBoard;
		CyreyApp* mApp;
		int mZoomPct;
		bool mDragging;
		raylib::Vector2 mDragMouseBegin;
		raylib::Vector2 mDragTileBegin;
		//used for disabling swaps until the mouse is released
		bool mTriedSwap;

		Board() = default;
		Board(int width, int height) :
			mWidth(width), mHeight(height) {};
		Board(int size) : Board(size, size) {};

		void Init();
		void Update();
		void Draw() const;
		static std::vector<std::vector<Piece>> ParseBoardString(const char*);
		std::optional<raylib::Vector2> GetHoveredTile() const;
		bool IsMouseInBoard() const;
		bool TrySwap(int x, int y, int toX, int toY);
		bool IsSwapLegal(int x, int y, int toX, int toY) const;

	private:
		void UpdateDragging();
		void DrawCheckerboard() const;
		void DrawPieces() const;
		void DrawHoverSquare() const;
	};
}
	
#endif // #_CYREY_BOARD_HEADER