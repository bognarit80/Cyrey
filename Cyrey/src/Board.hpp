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

	enum class SwapDirection
	{
		None,
		Up,
		Down,
		Left,
		Right
	};

	class Board
	{
	public:
		int mWidth;
		int mHeight;
		float mXOffset;
		float mYOffset;
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
		float mFallDelay;
		float mMissDelay;
		int64_t mScore;
		int mPiecesCleared;
		int mCascadeNumber;
		int mPiecesClearedInMove;
		raylib::Vector2 mBoardSwerve;
		constexpr static float cSwerveCoeff = 0.1f;
		constexpr static int cMaxCascadesSwerve = 8;
		constexpr static float cFallDelay = 0.2f;
		constexpr static float cMissPenalty = 3 * cFallDelay;
		bool mWantBoardSwerve;
		int mUpdateCnt;
		int mColorCount;
		int mBaseScore; //score for one match
		int mScoreMultiplier;
		float mSwapDeadZone; //percentage of tile width before the drag turns into a swap
		float mSecondsRemaining;
		constexpr static float cStartingTime = 60.0f;

		Board() = default;
		Board(int width, int height) :
			mWidth(width), mHeight(height) {};
		Board(int size) : Board(size, size) {};

		void Init();
		void Update();
		void Draw() const;
		void UpdateInput();
		static std::vector<std::vector<Piece>> ParseBoardString(const char*);
		std::vector<std::vector<Piece>> GenerateStartingBoard() const;
		void ResetBoard();
		std::optional<raylib::Vector2> GetHoveredTile() const;
		bool IsMouseInBoard() const;
		bool FindSets();
		bool FindSets(int pieceRow, int pieceCol, PieceColor color, bool first = true);
		bool IsPieceBeingMatched(unsigned int pieceID) const;
		bool TrySwap(int row, int col, SwapDirection direction);
		bool TrySwap(int row, int col, int toRow, int toCol);
		bool IsSwapLegal(int row, int col, int toRow, int toCol) const;
		bool CanSwap() const;
		constexpr bool IsPositionLegal(int row, int col) const;

	private:
		void UpdateDragging();
		void UpdateMatchSets();
		void UpdateFalling();
		void FillInBlanks();
		void DrawCheckerboard() const;
		void DrawPieces() const;
		void DrawHoverSquare() const;
		void DrawScore() const;
	};
}
	
#endif // #_CYREY_BOARD_HEADER