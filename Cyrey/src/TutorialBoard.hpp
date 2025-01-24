#ifndef CYREY_TUTORIALBOARD_HEADER
#define CYREY_TUTORIALBOARD_HEADER

#include "Board.hpp"

namespace Cyrey
{
	enum class TutorialStage
	{
		Loading = 0,
		FirstWindow,
		FirstSwap,
		FirstMatch,
		FirstRefill,
		SecondMatch,
		Cascade,
		BombMatch,
		BombExplode,
		FinalMatches,
		FinalWindow
	};

	class TutorialBoard : public Board
	{
	public:
		TutorialBoard(int width, int height) :
			Board(width, height) {}

		void Update() override;
		void Draw() override;
		void NewGame() override;
		void ResetBoard() override;
		bool TrySwap(int col, int row, SwapDirection direction) override;

	protected:
		void DrawSideUI() override;

	private:
		TutorialStage mStage { TutorialStage::Loading };
		std::string mSideText;
		float mPopupHighlightAlpha { 0.0f };
		int mMatchesRemaining { TutorialBoard::cFinalMatchesAmount };
		bool mShowSkipDialog { false };

		void NextStage();
		void FinishTutorial();
		bool DrawWindow(const char* title, const char* text, int textLength) const;
		void DrawSidePopup(const char* text, int textLength) const;
		bool DrawSkipDialog();
		void DrawPieceHighlights(std::initializer_list<::Vector2> pieces) const;

		static constexpr int cTutorialSeed = 1732213098;

		static constexpr char cSkipTutorialTxt[] = "Skip Tutorial";
		static constexpr char cSkipTutorialConfirmation[] = "Are you sure you want to skip the tutorial?";

		static constexpr char cWelcomeTitle[] = "Welcome!";
		static constexpr char cWelcomeTxt[] =
			"Welcome to Cyrey! Cyrey is a fast-paced Match-3 game focused on matching speed."
			" Animations are minimal and distractions are even less.\nLet's go"
			" through some basics first. The main goal is making matches of 3 or more same-coloured pieces."
			" To do that, you swap two adjacent pieces. Try it now!";

		static constexpr char cSwapThese[] =
			"Swap these two pieces!"
			"\n\n(Swap two adjacent pieces by dragging one to another, or selecting one piece "
			"and then the other.\nYou can also use WASD controls on a hovered or selected piece.)";
		static constexpr std::initializer_list<::Vector2> cFirstSwapCoords = { { 3, 2 }, { 3, 3 } };

		static constexpr char cMatchOfThreeWill[] = "A match of 3 or more same-coloured pieces will disappear...";
		static constexpr char cAndNewPieces[] = "... and new pieces will drop to replace them!";

		static constexpr char cSecondSwap[] =
			"A match doesn't have to be fully horizontal or vertical."
			"\nTry swapping these two pieces!";
		static constexpr std::initializer_list<::Vector2> cSecondSwapCoords = { { 5, 3 }, { 6, 3 } };
		static constexpr char cCascades[] =
			"On board refill, any new matches will also disappear automatically."
			"\nThis is called a Cascade. They award extra points!";
		static constexpr std::initializer_list<::Vector2> cCascadeCoords = { { 6, 3 }, { 7, 3 }, { 7, 4 } };

		static constexpr char cBombMatch[] =
			"Matches of 4 or more pieces will create special pieces."
			"\nTry creating a Bomb by swapping these two pieces!";
		static constexpr std::initializer_list<::Vector2> cThirdSwapCoords = { { 3, 6 }, { 4, 6 } };

		static constexpr char cBombExplode[] = "Great move! Now, let's detonate the bombs we created!";
		static constexpr std::initializer_list<::Vector2> cFourthSwapCoords = { { 4, 6 }, { 5, 6 } };

		static constexpr char cSwapTxt[] =
			"Wow, that was an amazing combo! You're getting the hang of this quickly!"
			"\nTry making a few more matches on your own now!"
			"\n\nSwap two adjacent pieces by dragging one to another, or selecting one piece "
			"and then the other.\nYou can also use WASD controls on a hovered or selected piece.";
		static constexpr int cFinalMatchesAmount = 10;

		static constexpr char cFinalWindowTitle[] = "Ready!";
		static constexpr char cFinalWindowTxt[] =
			"Okay, you are ready for the real thing now. Use what you've learned so far"
			" to score as high as you can in one minute! Maximize your combos, make big matches, or make a lot of small"
			" matches quickly - develop your style of playing! Just don't miss... or maybe you should? :)";
	};
} // namespace Cyrey

#endif // CYREY_TUTORIALBOARD_HEADER
