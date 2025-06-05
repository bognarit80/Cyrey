#ifndef CYREY_BOARD_HEADER
#define CYREY_BOARD_HEADER

#include <memory>
#include <optional>
#include <vector>
#include "CyreyApp.hpp"
#include "GameConfig.hpp"
#include "GameStats.hpp"
#include "MatchSet.hpp"
#include "Piece.hpp"
#include "PieceDropAnim.hpp"
#include "PieceMatchAnim.hpp"
#include "Replay.hpp"
#include "Resources.hpp"
#include "SwapAnim.hpp"
#include "SwapDirection.hpp"

namespace Cyrey
{
	class CyreyApp;

	class Board
	{
	public:
		int mWidth { 1 };
		int mHeight { 1 };
		float mXOffset { 0.0f };
		float mYOffset { 0.0f };
		float mTileSize { 0.0f };
		float mTileInset { 0.0f };
		std::vector<std::vector<Piece>> mBoard;
		CyreyApp* mApp {};
		float mZoomPct { Board::cDefaultZoomPct };
		bool mDragging { false };
		std::vector<MatchSet> mMatchSets;
		float mFallDelay { 0.0f };
		float mMissDelay { 0.0f };
		GameStats mStats {};
		int mCascadeNumber { 0 };
		int mPiecesClearedInMove { 0 };
		int64_t mScoreInMove { 0 };
		::Vector2 mBoardSwerve {};
		GameConfig mGameConfig {};
		int mScoreMultiplier { 1 };
		float mGameSpeed { 1.0f };
		float mSecondsRemaining { 0.0f };
		std::vector<PieceMatchAnim> mMatchedPieceAnims;
		std::vector<PieceDropAnim> mDroppedPieceAnims;
		::Vector2 mQueuedSwapPos {};
		SwapDirection mQueuedSwapDirection { SwapDirection::None };
		float mNewGameAnimProgress { 0.0f };
		bool mDroppedNewGamePieces { false };
		float mGameOverAnimProgress { 0.0f };
		bool mIsGameOver { false };
		bool mIsInReplay { false };
		std::unique_ptr<Replay> mReplayData;
		std::unique_ptr<Replay> mReplayCopy; // temp for testing purposes
		float mSecondsSinceLastCommand { 0.0f };
		bool mHasSavedReplay { false };
		SwapAnim mSwapAnim;
		std::optional<::Vector2> mSelectedTile;
		bool mIsPaused { false };
		bool mHasSeekedReplay { false };

#ifdef PLATFORM_ANDROID
		static constexpr float cDefaultZoomPct = 85.0f;
#else
		static constexpr float cDefaultZoomPct = 70.0f;
#endif
		static constexpr float cSwerveCoeff = 0.1f;
		static constexpr int cMaxCascadesSwerve = 8;
		static constexpr float cNewGameAnimDuration = 1.0f;
		static constexpr float cGameOverAnimDuration = 1.0f;
		static constexpr float cBoardAlpha = 0.25f;

		Board() = default;

		Board(int width, int height) :
			mWidth(width), mHeight(height) {}
		explicit Board(int size) : Board(size, size) {}
		Board(const Board& other) = delete;
		Board(Board&& other) noexcept = default;
		Board& operator=(const Board& other) = delete;
		Board& operator=(Board&& other) noexcept = default;
		virtual ~Board() = default;

		virtual void Update();
		virtual void Draw(); // not const because we want buttons on side UI and results screen
		void UpdateInput();
		static std::vector<std::vector<Piece>> ParseBoardString(const char* data);
		[[nodiscard]] std::vector<std::vector<Piece>> GenerateStartingBoard() const;
		virtual void NewGame();
		virtual void ResetBoard();
		/// Checks for mWantBoardSwerve. Modify swerve value directly to skip the check.
		void AddSwerve(::Vector2 swerve);
		[[nodiscard]] std::optional<::Vector2> GetHoveredTile() const;
		[[nodiscard]] bool IsMouseInBoard() const;
		bool FindSets();
		bool FindSets(int pieceCol, int pieceRow, PieceColor color, bool first = true);
		[[nodiscard]] bool IsPieceBeingMatched(unsigned int pieceID) const;
		bool SelectPiece(int col, int row);
		virtual bool TrySwap(int col, int row, SwapDirection direction);
		bool TrySwap(int col, int row, int toCol, int toRow);
		[[nodiscard]] bool IsSwapLegal(int col, int row, int toCol, int toRow) const;
		[[nodiscard]] bool CanSwap() const;
		[[nodiscard]] constexpr bool IsPositionLegal(int col, int row) const;
		// returns the amount of pieces cleared, if the piece was special
		int MatchPiece(Piece& piece, const Piece& byPiece = Cyrey::gNullPiece, bool destroy = false);
		int DoHypercube(const Piece& cubePiece, const Piece& byPiece = Cyrey::gNullPiece);
		void PlayReplay();
		void PlayReplay(const Replay& replay);
		void SetReplayTo(float secs);

	protected:
		/// Updates sizes and offsets related to the window size.
		void UpdateUI();
		bool UpdateReplay();
		void UpdateSwapAnim();
		void UpdateMatchedPieceAnims();
		void UpdateDroppedPieceAnims();
		void UpdateGameOverAnim();
		void UpdateCurrentUserStats() const; // call after game ends
		void UpdateBoardSwerve();
		// increases mSecondsRemaining linearly over the duration of the anim, returns true if in anim
		bool UpdateNewGameAnim();
		void UpdateDragging();
		size_t UpdateMatchSets(); // returns the amount of match sets processed
		void UpdateFalling();
		void UpdateDroppedFiles();
		[[nodiscard]] float GetStepInterval() const;
		void FillInBlanks();
		void HandleQueuedSwaps();
		virtual void PlaySound(ResSoundID sound) const; /// Wrapper, for skipping sounds while seeking replays.
		void DrawCheckerboard() const;
		void DrawBorder() const;
		void DrawPieces() const;
		void DrawSwapAnim() const;
		void DrawPieceMatchAnims() const;
		void DrawPieceDropAnims() const;
		void DrawHoverSquare() const;
		virtual void DrawSideUI();
		void DrawReplayControls(); // not const because the buttons alter state
		void DrawResultsScreen(); // not const because we want buttons
	};
} // namespace Cyrey

#endif // #CYREY_BOARD_HEADER
