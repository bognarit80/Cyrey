#ifndef _CYREY_BOARD_HEADER
#define _CYREY_BOARD_HEADER

#include "Piece.hpp"
#include "CyreyApp.hpp"
#include <optional>
#include "MatchSet.h"
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
		std::vector<MatchSet> mMatchSets;
		std::unique_ptr<MatchSet> mCurrentMatchSet;

		Board() = default;
		Board(int width, int height) :
			mWidth(width), mHeight(height) {};
		Board(int size) : Board(size, size) {};

		void Init();
		void Update();
		void Draw() const;
		static std::vector<std::vector<Piece>> ParseBoardString(const char*);
		std::vector<std::vector<Piece>> GenerateStartingBoard() const;
		std::optional<raylib::Vector2> GetHoveredTile() const;
		bool IsMouseInBoard() const;
		bool FindSets();
		bool FindSets(int pieceRow, int pieceCol, PieceColor color, bool first = true);
		bool IsPieceBeingMatched(unsigned int pieceID) const;
		bool TrySwap(int row, int col, int toRow, int toCol);
		bool IsSwapLegal(int row, int col, int toRow, int toCol) const;
		constexpr bool IsPositionLegal(int row, int col) const;

	private:
		void UpdateDragging();
		void UpdateMatchSets();
		void UpdateFalling();
		void FillInBlanks();
		void DrawCheckerboard() const;
		void DrawPieces() const;
		void DrawHoverSquare() const;
	};
}
	
#endif // #_CYREY_BOARD_HEADER