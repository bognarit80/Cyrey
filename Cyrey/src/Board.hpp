#ifndef CYREY_BOARD_HEADER
#define CYREY_BOARD_HEADER

#include "Piece.hpp"
#include "CyreyApp.hpp"
#include <optional>
#include "MatchSet.hpp"
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
		float mTileSize;
		float mTileInset;
		float mBoardAlpha;
		std::vector<std::vector<Piece>> mBoard;
		CyreyApp* mApp;
		float mZoomPct;
		bool mDragging;
		::Vector2 mDragMouseBegin;
		::Vector2 mDragTileBegin;
		bool mTriedSwap; // used for disabling swaps until the mouse is released
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
		int mColorCount;
		int mBaseScore; // score for one match
		int mScoreMultiplier;
		float mSecondsRemaining;
		std::vector<PieceMatchAnim> mMatchedPieceAnims;
		std::vector<PieceDropAnim> mDroppedPieceAnims;
		::Vector2 mQueuedSwapPos;
		SwapDirection mQueuedSwapDirection;
		float mNewGameAnimProgress;
		bool mDroppedNewGamePieces;
		float mGameOverAnimProgress;
		bool mIsGameOver;
		bool mIsInReplay;
		std::unique_ptr<Replay> mReplayData;
		std::unique_ptr<Replay> mReplayCopy; // temp for testing purposes
		float mSecondsSinceLastCommand;
        bool mHasSavedReplay;
        bool mHasDroppedFile;
        std::optional<Replay> mDroppedReplay;

		static constexpr float cDefaultZoomPct = 70.0f;
		static constexpr float cSwerveCoeff = 0.1f;
		static constexpr int cMaxCascadesSwerve = 8;
		static constexpr float cFallDelay = 0.2f;
		static constexpr float cMissPenalty = 3 * cFallDelay;
		static constexpr float cStartingTime = 60.0f;
		static constexpr int cLightningPiecesAmount = 10;
		static constexpr float cNewGameAnimDuration = 1.0f;
		static constexpr float cGameOverAnimDuration = 1.0f;

		// game stats
		int mMovesMade;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int64_t mBestMovePoints;
		int mBestMoveCascades;

		Board() = default;
		Board(int width, int height) :
			mWidth(width), mHeight(height) {};
		explicit Board(int size) : Board(size, size) {};

		void Init();
		void Update();
		void Draw(); // not const because we want buttons on side UI and results screen
		void UpdateInput();
		static std::vector<std::vector<Piece>> ParseBoardString(const char* data);
		[[nodiscard]] std::vector<std::vector<Piece>> GenerateStartingBoard() const;
		void NewGame();
		void ResetBoard();
		void AddSwerve(::Vector2 swerve); /// Checks for mWantBoardSwerve. Modify swerve value directly to skip the check.
		[[nodiscard]] std::optional<::Vector2> GetHoveredTile() const;
		[[nodiscard]] bool IsMouseInBoard() const;
		bool FindSets();
		bool FindSets(int pieceCol, int pieceRow, PieceColor color, bool first = true);
		[[nodiscard]] bool IsPieceBeingMatched(unsigned int pieceID) const;
		bool TrySwap(int col, int row, SwapDirection direction);
		bool TrySwap(int col, int row, int toCol, int toRow);
		[[nodiscard]] bool IsSwapLegal(int col, int row, int toCol, int toRow) const;
		[[nodiscard]] bool CanSwap() const;
		[[nodiscard]] constexpr bool IsPositionLegal(int col, int row) const;
		int MatchPiece(Piece& piece, const Piece& byPiece = Cyrey::gNullPiece, bool destroy = false); // returns the amount of pieces cleared, if the piece was special
		int DoHypercube(const Piece& cubePiece, const Piece& byPiece = Cyrey::gNullPiece);
        void PlayReplay();
        void PlayReplay(const Replay& replay);

	private:
		void UpdateReplay();
		void UpdateMatchedPieceAnims();
		void UpdateDroppedPieceAnims();
		void UpdateGameOverAnim();
		void UpdateBoardSwerve();
		bool UpdateNewGameAnim(); // increases mSecondsRemaining linearly over the duration of the anim, returns true if in anim
		void UpdateDragging();
		size_t UpdateMatchSets(); // returns the amount of match sets processed
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
} // namespace Cyrey
	
#endif // #CYREY_BOARD_HEADER