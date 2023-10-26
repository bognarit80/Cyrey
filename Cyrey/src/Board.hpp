#ifndef _CYREY_BOARD_HEADER
#define _CYREY_BOARD_HEADER

#include "Piece.hpp"
#include "CyreyApp.hpp"
#include <optional>
#include "MatchSet.h"
#include <memory>
#include <vector>
#include "PieceMatchAnim.hpp"
#include "PieceDropAnim.hpp"
#include "SwapDirection.hpp"
#include "Replay.hpp"

namespace Cyrey
{
	class CyreyApp;

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
		static constexpr int cDefaultZoomPct = 70;
		bool mDragging;
		::Vector2 mDragMouseBegin;
		::Vector2 mDragTileBegin;
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
		int64_t mScoreInMove;
		::Vector2 mBoardSwerve;
		constexpr static float cSwerveCoeff = 0.1f;
		constexpr static int cMaxCascadesSwerve = 8;
		constexpr static float cFallDelay = 0.2f;
		constexpr static float cMissPenalty = 3 * cFallDelay;
		int mColorCount;
		int mBaseScore; //score for one match
		int mScoreMultiplier;
		float mSecondsRemaining;
		constexpr static float cStartingTime = 60.0f;
		constexpr static int cLightningPiecesAmount = 10;
		std::vector<PieceMatchAnim> mMatchedPieceAnims;
		std::vector<PieceDropAnim> mDroppedPieceAnims;
		::Vector2 mQueuedSwapPos;
		SwapDirection mQueuedSwapDirection;
		float mNewGameAnimProgress;
		bool mDroppedNewGamePieces;
		static constexpr float cNewGameAnimDuration = 1.0f;
		float mGameOverAnimProgress;
		bool mIsGameOver;
		static constexpr float cGameOverAnimDuration = 1.0f;
		bool mIsInReplay;
		std::unique_ptr<Replay> mReplayData;
		std::unique_ptr<Replay> mReplayCopy; // temp for testing purposes
		float mSecondsSinceLastCommand;
        bool mHasSavedReplay;
        bool mHasDroppedFile;
        std::optional<Replay> mDroppedReplay;

		// game stats
		int mMovesMade;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int mBestMovePoints;
		int mBestMoveCascades;

		Board() = default;
		Board(int width, int height) :
			mWidth(width), mHeight(height) {};
		Board(int size) : Board(size, size) {};

		void Init();
		void Update();
		void Draw(); // not const because we want buttons on side UI and results screen
		void UpdateInput();
		static std::vector<std::vector<Piece>> ParseBoardString(const char*);
		std::vector<std::vector<Piece>> GenerateStartingBoard() const;
		void ResetBoard();
		void AddSwerve(::Vector2 swerve); //Checks for mWantBoardSwerve. Modify swerve value directly to skip the check.
		std::optional<::Vector2> GetHoveredTile() const;
		bool IsMouseInBoard() const;
		bool FindSets();
		bool FindSets(int pieceRow, int pieceCol, PieceColor color, bool first = true);
		bool IsPieceBeingMatched(unsigned int pieceID) const;
		bool TrySwap(int col, int row, SwapDirection direction);
		bool TrySwap(int col, int row, int toCol, int toRow);
		bool IsSwapLegal(int col, int row, int toCol, int toRow) const;
		bool CanSwap() const;
		constexpr bool IsPositionLegal(int col, int row) const;
		int MatchPiece(Piece& piece, const Piece& byPiece = Cyrey::gNullPiece, bool destroy = false); //returns the amount of pieces cleared, if the piece was special
		int DoHypercube(Piece& piece, const Piece& byPiece = Cyrey::gNullPiece);

	private:
		void UpdateReplay();
		void UpdateMatchedPieceAnims();
		void UpdateDroppedPieceAnims();
		void UpdateGameOverAnim();
		void UpdateBoardSwerve();
		bool UpdateNewGameAnim(); //increases mSecondsRemaining linearly over the duration of the anim, returns true if in anim
		void UpdateDragging();
		int UpdateMatchSets(); //returns the amount of match sets processed
		void UpdateFalling();
        void UpdateDroppedFiles();
		void FillInBlanks();
		void DrawCheckerboard() const;
		void DrawBorder() const;
		void DrawPieces() const;
		void DrawPieceMatchAnims() const;
		void DrawPieceDropAnims() const;
		void DrawHoverSquare() const;
		void DrawSideUI();
		void DrawResultsScreen(); // not const because we want buttons
	};
}
	
#endif // #_CYREY_BOARD_HEADER