#include "TutorialBoard.hpp"

#include <raygui.h>

bool operator==(const ::Vector2 first, const ::Vector2 second)
{
	return first.x == second.x && first.y == second.y;
}

static bool IsSwapInCoords(int col, int row, Cyrey::SwapDirection direction, std::initializer_list<::Vector2> pieces)
{
	if (pieces.size() != 2)
		return false;

	constexpr int offsets[][5] = {
		{ 0, 0 },
		{ 0, -1 },
		{ 0, 1, },
		{ -1, 0 },
		{ 1, 0 }
	};

	::Vector2 first = { static_cast<float>(col), static_cast<float>(row), };
	::Vector2 second = {
		static_cast<float>(col + offsets[static_cast<int>(direction)][0]),
		static_cast<float>(row + offsets[static_cast<int>(direction)][1])
	};

	auto data = std::data(pieces);
	return (first == data[0] && second == data[1]) || (first == data[1] && second == data[0]);
}

void Cyrey::TutorialBoard::Update()
{
	if (this->mStage != TutorialStage::FirstWindow
		&& this->mStage != TutorialStage::FirstRefill
		&& this->mStage != TutorialStage::Cascade
		&& !this->mShowSkipDialog)
	{
		this->UpdateDragging();
		this->UpdateInput();
	}

	this->UpdateSwapAnim();
	this->UpdateMatchedPieceAnims();
	this->UpdateDroppedPieceAnims();

	auto screenWidth = static_cast<float>(this->mApp->mWidth);
	auto screenHeight = static_cast<float>(this->mApp->mHeight);

	if (screenWidth > 0 && screenHeight > 0)
	{
		this->mTileSize = (screenHeight * this->mZoomPct / 100) / static_cast<float>(this->mHeight);
		this->mTileInset = this->mTileSize / 10;
		this->mXOffset = (screenWidth / 2) - (static_cast<float>(this->mWidth) * this->mTileSize / 2) + this->
			mBoardSwerve.x;
		this->mYOffset = (screenHeight / 2) - (static_cast<float>(this->mHeight) * this->mTileSize / 2) + this->
			mBoardSwerve.y;
	}

	this->UpdateBoardSwerve();

	if (this->mFallDelay > 0.0f)
	{
		this->mFallDelay -= this->GetStepInterval();
		if (this->mFallDelay <= 0.0f)
		{
			this->mFallDelay = 0.0f;
			this->UpdateFalling();
			this->UpdateMatchSets();

			switch (this->mStage)
			{
			case TutorialStage::FirstMatch:
			case TutorialStage::SecondMatch:
				if (this->mCascadeNumber == 2)
				{
					this->mFallDelay = 2.5f;
					this->NextStage();
				}
			case TutorialStage::BombMatch:
			case TutorialStage::BombExplode:
				if (this->mCascadeNumber == 0)
					this->NextStage();
				break;
			case TutorialStage::FinalMatches:
				if (this->mCascadeNumber == 0)
				{
					if (this->mMatchesRemaining-- == 0)
						this->NextStage();
					else
						this->mApp->mGameConfig.mFallDelay -= 0.05f;
				}
			default:
				break;
			}
		}
	}
	if (this->mStage == TutorialStage::FirstRefill
		|| this->mStage == TutorialStage::Cascade)
	{
		static float timer = 4.0f;
		timer -= this->GetStepInterval();
		if (timer <= 0.0f)
		{
			timer = 4.0f;
			this->NextStage();
		}
	}
	if (this->mMissDelay > 0.0f)
	{
		this->mMissDelay -= this->GetStepInterval();
		if (this->mMissDelay <= 0.0f)
			this->mMissDelay = 0.0f;
	}

	bool isInNewGameAnim = this->UpdateNewGameAnim();
	if (!isInNewGameAnim && this->mSecondsRemaining < 0.0f)
		this->mSecondsRemaining = 0.0f;

	if (!isInNewGameAnim && this->mStage == TutorialStage::Loading)
		this->NextStage();

	this->UpdateGameOverAnim();

	::UpdateMusicStream(this->mApp->mResMgr->mMusics["mainMenuTheme.ogg"]);

	if (this->mPopupHighlightAlpha > 0.0f)
	{
		this->mPopupHighlightAlpha -= this->GetStepInterval() * 255.0f;
		if (this->mPopupHighlightAlpha <= 0.0f)
			this->mPopupHighlightAlpha = 0.0f;
	}
}

void Cyrey::TutorialBoard::Draw()
{
	Board::Draw();

	if (this->mShowSkipDialog)
		return;

	switch (this->mStage)
	{
	case TutorialStage::FirstWindow:
		if (this->DrawWindow(TutorialBoard::cWelcomeTitle, TutorialBoard::cWelcomeTxt, std::size(cWelcomeTxt)))
			this->NextStage();
		break;
	case TutorialStage::FirstSwap:
		this->DrawPieceHighlights(TutorialBoard::cFirstSwapCoords);
		break;
	case TutorialStage::SecondMatch:
		this->DrawPieceHighlights(TutorialBoard::cSecondSwapCoords);
		break;
	case TutorialStage::Cascade:
		this->DrawPieceHighlights(TutorialBoard::cCascadeCoords);
		break;
	case TutorialStage::BombMatch:
		this->DrawPieceHighlights(TutorialBoard::cThirdSwapCoords);
		break;
	case TutorialStage::BombExplode:
		this->DrawPieceHighlights(TutorialBoard::cFourthSwapCoords);
		break;
	case TutorialStage::FinalWindow:
		if (this->DrawWindow(TutorialBoard::cFinalWindowTitle,
		                     TutorialBoard::cFinalWindowTxt,
		                     std::size(cFinalWindowTxt)))
		{
			this->FinishTutorial();
		}
		break;
	default:
		break;
	}
}

void Cyrey::TutorialBoard::NewGame()
{
	this->mIsInReplay = false;
	this->mHasSavedReplay = false;
	this->mApp->SeedRNG(TutorialBoard::cTutorialSeed);
	this->mReplayData->mSeed = TutorialBoard::cTutorialSeed;
	this->mStage = TutorialStage::Loading;
	this->mMatchesRemaining = TutorialBoard::cFinalMatchesAmount;
	this->mApp->mGameConfig = Cyrey::cTutorialGameConfig;
	this->ResetBoard();
}

void Cyrey::TutorialBoard::ResetBoard()
{
	::PlaySound(this->mApp->mResMgr->mSounds["boardAppear.ogg"]);
	this->mBoard = this->GenerateStartingBoard(); // current seed generates a matchless board in 1 try
	this->mSecondsRemaining = 0.0f;
	this->mBoardSwerve = ::Vector2 { 0.0f, -this->mTileSize * 8 };
	this->mNewGameAnimProgress = 0.0f;
	this->mDroppedNewGamePieces = false;
	this->mGameOverAnimProgress = 0.0f;
	this->mIsGameOver = false;
	this->mSelectedTile.reset();
	this->mScore = 0;
	this->mPiecesCleared = 0;
	::StopMusicStream(this->mApp->mResMgr->mMusics["mainMenuTheme.ogg"]);
	::PlayMusicStream(this->mApp->mResMgr->mMusics["mainMenuTheme.ogg"]);
}

bool Cyrey::TutorialBoard::TrySwap(int col, int row, SwapDirection direction)
{
	bool ret;
	switch (this->mStage)
	{
	case TutorialStage::FirstSwap:
		if (!IsSwapInCoords(col, row, direction, TutorialBoard::cFirstSwapCoords))
			return false;

		ret = Board::TrySwap(col, row, direction);
		this->mFallDelay = 4.0f;
		this->NextStage();
		break;
	case TutorialStage::SecondMatch:
		if (!IsSwapInCoords(col, row, direction, TutorialBoard::cSecondSwapCoords))
			return false;

		ret = Board::TrySwap(col, row, direction);
		this->mFallDelay = 3.0f;
		break;
	case TutorialStage::BombMatch:
		if (!IsSwapInCoords(col, row, direction, TutorialBoard::cThirdSwapCoords))
			return false;

		ret = Board::TrySwap(col, row, direction);
		this->mFallDelay = 2.0f;
		break;
	case TutorialStage::BombExplode:
		if (!IsSwapInCoords(col, row, direction, TutorialBoard::cFourthSwapCoords))
			return false;

		ret = Board::TrySwap(col, row, direction);
		this->mFallDelay = 2.0f;
		break;
	default:
		return Board::TrySwap(col, row, direction);
	}
	return ret;
}

void Cyrey::TutorialBoard::DrawSideUI()
{
	switch (this->mStage)
	{
	case TutorialStage::FirstSwap:
		this->DrawSidePopup(TutorialBoard::cSwapThese, std::size(TutorialBoard::cSwapThese));
		break;
	case TutorialStage::FirstMatch:
		this->DrawSidePopup(TutorialBoard::cMatchOfThreeWill, std::size(TutorialBoard::cMatchOfThreeWill));
		break;
	case TutorialStage::FirstRefill:
		this->DrawSidePopup(TutorialBoard::cAndNewPieces, std::size(TutorialBoard::cAndNewPieces));
		break;
	case TutorialStage::SecondMatch:
		this->DrawSidePopup(TutorialBoard::cSecondSwap, std::size(TutorialBoard::cSecondSwap));
		break;
	case TutorialStage::Cascade:
		this->DrawSidePopup(TutorialBoard::cCascades, std::size(TutorialBoard::cCascades));
		break;
	case TutorialStage::BombMatch:
		this->DrawSidePopup(TutorialBoard::cBombMatch, std::size(TutorialBoard::cBombMatch));
		break;
	case TutorialStage::BombExplode:
		this->DrawSidePopup(TutorialBoard::cBombExplode, std::size(TutorialBoard::cBombExplode));
		break;
	case TutorialStage::FinalMatches:
		this->DrawSidePopup(TutorialBoard::cSwapTxt, std::size(TutorialBoard::cSwapTxt));
		break;
	default:
		break;
	}

	int fontSize = this->mApp->mHeight / 25;
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
	int iconSize = fontSize * 2;
	int iconScale = iconSize / 16;
	::GuiSetIconScale(iconScale);

	if (::GuiButton(::Rectangle {
						this->mXOffset - iconSize - (this->mTileSize / 2),
						(static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 7,
						static_cast<float>(iconSize),
						static_cast<float>(iconSize)
					},
					::GuiIconText(::GuiIconName::ICON_PLAYER_PAUSE, "")))
	{
		this->mApp->ChangeToState(CyreyAppState::SettingsMenu);
	}

	if (::GuiButton(::Rectangle {
							this->mXOffset - (iconSize * 2.25f) - (this->mTileSize / 2),
							(static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 7,
							static_cast<float>(iconSize),
							static_cast<float>(iconSize)
						},
						::GuiIconText(::GuiIconName::ICON_REDO_FILL, "")))
	{
		this->mShowSkipDialog = true;
	}

	if (this->mShowSkipDialog)
		this->mShowSkipDialog = this->DrawSkipDialog();
}

void Cyrey::TutorialBoard::NextStage()
{
	this->mStage = static_cast<TutorialStage>(static_cast<int>(this->mStage) + 1);
	this->mPopupHighlightAlpha = 255.0f;
}

void Cyrey::TutorialBoard::FinishTutorial()
{
	this->mApp->mCurrentUser->mFinishedTutorial = true;
	this->mApp->ChangeToState(CyreyAppState::MainMenu);
	if (this->mApp->mCurrentUser->mName == User::cDefaultName)
	{
		this->mApp->mUserMenu->mIsOpen = true;
		this->mApp->mUserMenu->mWantNameDialog = true;
	}
	else
		this->mApp->SaveCurrentUserData();
}

bool Cyrey::TutorialBoard::DrawWindow(const char* title, const char* text, int textLength) const
{
	auto appWidth = static_cast<float>(this->mApp->mWidth);
	auto appHeight = static_cast<float>(this->mApp->mHeight);

	float windowY = appHeight * 0.1f;
	float windowHeight = appHeight - (windowY * 2);
	float windowX = appWidth > appHeight ? (appWidth - windowHeight) / 2 : appWidth * 0.1f;
	float windowWidth = windowY < windowX ? windowHeight : appWidth - (windowX * 2); // square if window is wide
	Vector2 windowAnchor = { windowX, windowY };
	Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };
	float windowPaddingX = windowWidth * 0.025f;
	float windowPaddingY = windowHeight * 0.065f;

	float fontSize = windowHeight > windowWidth ? windowWidth / 20 : windowHeight / 20;
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
	::GuiSetIconScale(fontSize / 16);

	Rectangle txtRect = {
		windowX + windowPaddingX,
		windowY + windowPaddingY,
		windowWidth - (windowPaddingX * 2),
		windowHeight - (windowPaddingY * 2.2f)
	};
	Rectangle okBtnRect = {
		windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (windowHeight - windowPaddingY * 1.9f),
		(windowWidth - (windowPaddingX * 2)),
		fontSize
	};

	bool ret = ::GuiWindowBox(windowRect, title);

	auto wrap = ::GuiGetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_WRAP_MODE);
	auto alignment = ::GuiGetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_ALIGNMENT_VERTICAL);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_ALIGNMENT_VERTICAL,
	              ::GuiTextAlignmentVertical::TEXT_ALIGN_TOP);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_WRAP_MODE, ::GuiTextWrapMode::TEXT_WRAP_WORD);
	::GuiTextBox(txtRect, const_cast<char*>(text), textLength, false);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_WRAP_MODE, wrap);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_ALIGNMENT_VERTICAL, alignment);

	bool btn = ::GuiButton(okBtnRect, "Ok");

	return ret || btn;
}

void Cyrey::TutorialBoard::DrawSidePopup(const char* text, int textLength) const
{
	int fontSize = this->mApp->mHeight / 25;
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, GuiTextAlignment::TEXT_ALIGN_RIGHT);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);

	auto wrap = ::GuiGetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_WRAP_MODE);
	auto alignment = ::GuiGetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_ALIGNMENT_VERTICAL);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_ALIGNMENT_VERTICAL,
	              ::GuiTextAlignmentVertical::TEXT_ALIGN_TOP);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_WRAP_MODE, ::GuiTextWrapMode::TEXT_WRAP_WORD);
	Rectangle popupRect = {
		this->mXOffset - this->mTileSize * 5.5f,
		static_cast<float>(this->mApp->mHeight) * 0.25f,
		this->mTileSize * 5,
		static_cast<float>(this->mApp->mHeight) / 2
	};
	if (this->mStage == TutorialStage::SecondMatch ||
		this->mStage == TutorialStage::Cascade)
	{
		popupRect.x = this->mXOffset + (this->mTileSize * this->mWidth) + this->mTileSize / 2;
	}
	::GuiTextBox(popupRect, const_cast<char*>(text), textLength, false);
	::DrawRectangleLinesEx(popupRect, 4.0f, { 255, 255, 255, static_cast<uint8_t>(this->mPopupHighlightAlpha) });
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_WRAP_MODE, wrap);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_ALIGNMENT_VERTICAL, alignment);
}

bool Cyrey::TutorialBoard::DrawSkipDialog()
{
	int result = this->mApp->DrawDialog(
		TutorialBoard::cSkipTutorialTxt,
		TutorialBoard::cSkipTutorialConfirmation,
		"Yes;No");
	if (result == 1)
		this->FinishTutorial();
	return result == -1;
}

void Cyrey::TutorialBoard::DrawPieceHighlights(std::initializer_list<Vector2> pieces) const
{
	static Color rectColor = { 75, 228, 255, 255 };
	static float mult = 1.0f;
	static float accum = rectColor.g;
	float increment = (this->GetStepInterval() * 255.0f) * 2.0f; // for consistent speed on all framerates

	if (static_cast<uint32_t>(rectColor.g) + static_cast<uint32_t>(increment * mult) >= 255)
		mult = -mult;

	accum += increment * mult;
	rectColor.g = static_cast<char>(accum);
	for (auto [x, y] : pieces)
	{
		Rectangle rect = {
			x * this->mTileSize + this->mXOffset + 1,
			y * this->mTileSize + this->mYOffset + 1,
			this->mTileSize - 2,
			this->mTileSize - 2
		};
		::DrawRectangleRoundedLinesEx(rect, 0.0f, 1, this->mTileInset / 2.0f, rectColor);
	}
}
