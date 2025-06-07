#include "Board.hpp"
#include <cmath>
#include <map>
#include "raygui.h"
#include "raymath.h"

void Cyrey::Board::Update()
{
	this->UpdateUI();

	if (this->mIsPaused)
		return;

	if (this->mNewGameAnimProgress >= Board::cNewGameAnimDuration && this->mSecondsRemaining > 0.0f)
		this->mSecondsSinceLastCommand += this->GetStepInterval();

	if (!this->mIsInReplay)
	{
		this->UpdateDragging();
		this->UpdateInput();
	}

	if (this->mFallDelay > 0.0f)
		this->mFallDelay -= this->GetStepInterval();
	if (this->mMissDelay > 0.0f)
		this->mMissDelay -= this->GetStepInterval();

	bool didReplayMove = false;
	do
	{
		while (this->mFallDelay < 0.0f)
		{
			this->UpdateFalling();
			if (!this->UpdateMatchSets())
				this->mFallDelay = 0.0f;
			this->HandleQueuedSwaps();
		}

		if (this->mMissDelay < 0.0f)
		{
			this->mMissDelay = 0.0f;
			this->HandleQueuedSwaps();
		}

		if (this->mIsInReplay && this->mNewGameAnimProgress >= Board::cNewGameAnimDuration)
			didReplayMove = this->UpdateReplay();
		if (didReplayMove)
		{
			// SSLC would then be the remainder after doing the command
			if (this->mFallDelay != 0.0f)
				this->mFallDelay -= this->mSecondsSinceLastCommand;
			if (this->mMissDelay != 0.0f)
				this->mMissDelay -= this->mSecondsSinceLastCommand;
		}
	} while (didReplayMove);

	this->UpdateSwapAnim();
	this->UpdateMatchedPieceAnims();
	this->UpdateDroppedPieceAnims();
	this->UpdateBoardSwerve();

	if (!this->UpdateNewGameAnim() && ((this->mSecondsRemaining -= this->GetStepInterval()) < 0.0f))
		this->mSecondsRemaining = 0.0f;

	this->UpdateGameOverAnim();

	this->UpdateDroppedFiles();
	if (this->mHasSeekedReplay)
	{
		this->mIsPaused = true;
		this->mHasSeekedReplay = false;
	}
}

void Cyrey::Board::Draw()
{
	this->DrawCheckerboard();
	this->DrawBorder();
	this->DrawPieces();
	this->DrawSwapAnim();
	this->DrawPieceMatchAnims();
	this->DrawPieceDropAnims();
	this->DrawHoverSquare();
	this->DrawSideUI();
	if (this->mIsInReplay)
		this->DrawReplayControls();
	if (this->mIsGameOver)
		this->DrawResultsScreen();
}

void Cyrey::Board::UpdateInput()
{
	auto key = static_cast<KeyboardKey>(::GetKeyPressed());

	if (this->GetHoveredTile())
	{
		auto [x, y] = *this->GetHoveredTile();
		if (this->mSelectedTile.has_value())
		{
			x = this->mSelectedTile->x;
			y = this->mSelectedTile->y;
		}
		switch (key)
		{
		case ::KeyboardKey::KEY_W:
			this->TrySwap(x, y, SwapDirection::Up);
			return;
		case ::KeyboardKey::KEY_A:
			this->TrySwap(x, y, SwapDirection::Left);
			return;
		case ::KeyboardKey::KEY_S:
			this->TrySwap(x, y, SwapDirection::Down);
			return;
		case ::KeyboardKey::KEY_D:
			this->TrySwap(x, y, SwapDirection::Right);
			return;
#ifndef NDEBUG
		case ::KeyboardKey::KEY_O: // (almost) instantly finish game
			this->mSecondsRemaining = 0.5f;
			return;
		case ::KeyboardKey::KEY_F:
			this->mBoard[y][x].Bombify();
			return;
		case ::KeyboardKey::KEY_Z:
			this->mBoard[y][x].Lightningify();
			return;
		case ::KeyboardKey::KEY_C:
			this->mBoard[y][x].Hypercubify();
			return;
		case ::KeyboardKey::KEY_KP_1:
		case ::KeyboardKey::KEY_KP_2:
		case ::KeyboardKey::KEY_KP_3:
		case ::KeyboardKey::KEY_KP_4:
		case ::KeyboardKey::KEY_KP_5:
		case ::KeyboardKey::KEY_KP_6:
		case ::KeyboardKey::KEY_KP_7:
			this->mBoard[y][x].mColor = static_cast<PieceColor>(key - 320);
			break;
		case ::KeyboardKey::KEY_ONE:
		case ::KeyboardKey::KEY_TWO:
		case ::KeyboardKey::KEY_THREE:
		case ::KeyboardKey::KEY_FOUR:
		case ::KeyboardKey::KEY_FIVE:
		case ::KeyboardKey::KEY_SIX:
		case ::KeyboardKey::KEY_SEVEN:
			this->mBoard[y][x].mColor = static_cast<PieceColor>(key - 48);
			break;
#endif
		default:
			break;
		}
	}

	switch (key)
	{
	case ::KeyboardKey::KEY_R:
		this->NewGame();
		break;
	case ::KeyboardKey::KEY_L:
		this->mApp->mDarkMode ^= 1;
		break;
	case ::KeyboardKey::KEY_M:
		this->mApp->mSettings->mWantBoardSwerve ^= 1;
		break;
	case ::KeyboardKey::KEY_SPACE:
		if (this->mNewGameAnimProgress < Board::cNewGameAnimDuration)
		{
			this->mNewGameAnimProgress = Board::cNewGameAnimDuration;
			this->mBoardSwerve = ::Vector2Zero();
		}
		else if (this->mGameOverAnimProgress < Board::cGameOverAnimDuration)
		{
			this->mGameOverAnimProgress = Board::cGameOverAnimDuration;
		}
		break;
	default:
		break;
	}
}

std::vector<std::vector<Cyrey::Piece>> Cyrey::Board::ParseBoardString(const char* data)
{
	std::map<char, Cyrey::PieceColor> charPieceMap =
	{
		{ 'r', PieceColor::Red },
		{ 'g', PieceColor::Green },
		{ 'b', PieceColor::Blue },
		{ 'y', PieceColor::Yellow },
		{ 'o', PieceColor::Orange },
		{ 'w', PieceColor::White },
		{ 'p', PieceColor::Purple }
	};
	std::vector<std::vector<Piece>> board {};
	std::vector<Piece> row {};
	int i = 0;
	while (char c = data[i])
	{
		if (c == '\n')
		{
			board.push_back(row);
			row.clear();
		}
		else
			row.emplace_back(charPieceMap[c]);

		i++;
	}
	return board;
}

std::vector<std::vector<Cyrey::Piece>> Cyrey::Board::GenerateStartingBoard() const
{
	std::vector<std::vector<Piece>> board {};
	std::vector<Piece> row {};
	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			row.emplace_back(static_cast<PieceColor>(this->mApp->GetRandomNumber(1, this->mGameConfig.mPieceColorAmount)));
			row[j].mBoardX = j;
			row[j].mBoardY = i;
		}
		board.push_back(row);
		row.clear();
	}
	return board;
}

void Cyrey::Board::NewGame()
{
	this->mIsInReplay = false;
	this->mHasSavedReplay = false;
	this->mGameSpeed = 1.0f;
	this->mIsPaused = false;
	this->ResetBoard();
}

void Cyrey::Board::ResetBoard()
{
	this->mGameConfig = this->mApp->mGameConfig;
	this->mWidth = this->mGameConfig.mBoardWidth;
	this->mHeight = this->mGameConfig.mBoardHeight;
	unsigned int seed;
	if (this->mIsInReplay)
	{
		seed = this->mReplayCopy->mSeed;
		this->mApp->SeedRNG(seed);
	}
	else
	{
		seed = this->mApp->SeedRNG();
		this->mReplayData = std::make_unique<Replay>();
		this->mReplayData->mSeed = seed;
		this->mReplayData->mConfigVersion = this->mGameConfig.mVersion;
	}
	::TraceLog(TraceLogLevel::LOG_INFO, ::TextFormat("Seed: %u", seed));
	this->PlaySound(ResSoundID::BoardAppear);
	do
	{
		this->mMatchSets.clear();
		this->mBoard = this->GenerateStartingBoard();
	}
	while (this->FindSets()); // ugly until I make a better algorithm for creating boards with no sets
	this->mSecondsRemaining = 0.0f;
	// to fix NewGameAnim when called from other menus
	this->mTileSize = (static_cast<float>(this->mApp->mHeight) * this->mZoomPct / 100.0f) / static_cast<float>(this->mHeight);
	this->mBoardSwerve = ::Vector2 { 0.0f, -this->mTileSize * 8 };
	this->mNewGameAnimProgress = 0.0f;
	this->mDroppedNewGamePieces = false;
	this->mGameOverAnimProgress = 0.0f;
	this->mIsGameOver = false;
	this->mSelectedTile.reset();
	this->mQueuedSwapDirection = SwapDirection::None;
	this->mStats = {};

	this->mApp->PlayMusic(ResMusicID::Blitz1min);
}

void Cyrey::Board::AddSwerve(::Vector2 swerve)
{
	if (!this->mApp->mSettings->mWantBoardSwerve)
		return;

	this->mBoardSwerve = ::Vector2Add(this->mBoardSwerve, swerve);
}

std::optional<::Vector2> Cyrey::Board::GetHoveredTile() const
{
	const int mouseX = ::GetTouchX() - static_cast<int>(this->mXOffset);
	const int mouseY = ::GetTouchY() - static_cast<int>(this->mYOffset);

	if (mouseX <= 0 || mouseY <= 0 || this->mTileSize <= 0)
		return std::nullopt;

	const int xTile = mouseX / this->mTileSize;
	const int yTile = mouseY / this->mTileSize;
	if (xTile >= this->mWidth || yTile >= this->mHeight)
		return std::nullopt;

	return ::Vector2 { static_cast<float>(xTile), static_cast<float>(yTile) };
}

bool Cyrey::Board::IsMouseInBoard() const
{
	return this->GetHoveredTile().has_value();
}

bool Cyrey::Board::FindSets()
{
	bool foundSet = false;

	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			this->FindSets(j, i, this->mBoard[i][j].mColor);
			this->mBoard[i][j].mImmunity = false; //temp until better measures are implemented
			if (!this->mMatchSets.empty())
				foundSet = true;
		}
	}

	return foundSet;
}

bool Cyrey::Board::FindSets(int pieceCol, int pieceRow, PieceColor color, bool first)
{
	if (this->mBoard[pieceRow][pieceCol].mColor == PieceColor::Uncolored)
		return false; // don't check uncolored pieces at all

	static MatchSet matchSet {};
	if (matchSet.mPieces.empty())
	{
		if (!this->IsPieceBeingMatched(this->mBoard[pieceRow][pieceCol].mID))
			matchSet.mPieces.push_back(&this->mBoard[pieceRow][pieceCol]);
		else
			return false; // already checked
	}
	else
	{
		for (auto piece : matchSet.mPieces)
		{
			if (piece->mID == this->mBoard[pieceRow][pieceCol].mID)
				return false; // already checked
		}
		matchSet.mPieces.push_back(&this->mBoard[pieceRow][pieceCol]);
	}

	if ((pieceRow - 1) >= 0 &&
		(pieceRow - 1) < this->mHeight &&
		this->mBoard[pieceRow - 1][pieceCol].mColor == color)
	{
		this->FindSets(pieceCol, pieceRow - 1, color, false);
	}

	if ((pieceRow + 1) >= 0 &&
		(pieceRow + 1) < this->mHeight &&
		this->mBoard[pieceRow + 1][pieceCol].mColor == color)
	{
		this->FindSets(pieceCol, pieceRow + 1, color, false);
	}

	if ((pieceCol - 1) >= 0 &&
		(pieceCol - 1) < this->mWidth &&
		this->mBoard[pieceRow][pieceCol - 1].mColor == color)
	{
		this->FindSets(pieceCol - 1, pieceRow, color, false);
	}

	if ((pieceCol + 1) >= 0 &&
		(pieceCol + 1) < this->mWidth &&
		this->mBoard[pieceRow][pieceCol + 1].mColor == color)
	{
		this->FindSets(pieceCol + 1, pieceRow, color, false);
	}

	bool foundSet = false;
	if (matchSet.mPieces.size() >= 3)
	{
		foundSet = true;
		if (first)
		{
			matchSet.mAddedPieces.push_back(&this->mBoard[pieceRow][pieceCol]);
			this->mMatchSets.push_back(matchSet);
		}
	}

	if (first)
	{
		matchSet.mPieces.clear();
		matchSet.mAddedPieces.clear();
	}

	return foundSet;
}

bool Cyrey::Board::IsPieceBeingMatched(unsigned int pieceID) const
{
	for (const auto& matchSet : this->mMatchSets)
	{
		for (const auto piece : matchSet.mPieces)
		{
			if (piece->mID == pieceID)
				return true;
		}
	}
	return false;
}

bool Cyrey::Board::SelectPiece(int col, int row)
{
	if (this->IsPositionLegal(col, row))
	{
		if (this->mSelectedTile.has_value())
		{
			int selCol = static_cast<int>(this->mSelectedTile->x);
			int selRow = static_cast<int>(this->mSelectedTile->y);
			if (selCol == col && selRow == row)
			{
				// deselect piece if the same one is pressed
				this->mSelectedTile.reset();
				return false;
			}
			if (this->IsSwapLegal(selCol, selRow, col, row))
			{
				int xDiff = selCol - col;
				int yDiff = selRow - row;

				if (abs(xDiff))
					this->TrySwap(selCol, selRow, (xDiff > 0 ? SwapDirection::Left : SwapDirection::Right));
				else if (abs(yDiff))
					this->TrySwap(selCol, selRow, (yDiff > 0 ? SwapDirection::Up : SwapDirection::Down));

				return false;
			}
		}
		this->mSelectedTile = { static_cast<float>(col), static_cast<float>(row) };
		return true;
	}
	return false;
}

bool Cyrey::Board::TrySwap(int col, int row, SwapDirection direction)
{
	this->mSelectedTile.reset();
	if (!this->CanSwap())
	{
		float queueSwapTolerance = this->mApp->mSettings->mQueueSwapTolerance;
		// Always queue if in replay, as thanks to rounding/timing errors the outcome can differ.
		// (except if game already ended)
		if ((this->mIsInReplay || queueSwapTolerance >= 1.0f ||
				(this->mFallDelay < queueSwapTolerance &&
					this->mMissDelay < queueSwapTolerance &&
					Board::cNewGameAnimDuration - this->mNewGameAnimProgress < queueSwapTolerance)) &&
			this->mSecondsRemaining > 0)
		{
			this->mQueuedSwapPos = ::Vector2 { static_cast<float>(col), static_cast<float>(row) };
			this->mQueuedSwapDirection = direction;
		}
		else if (this->mNewGameAnimProgress < Board::cNewGameAnimDuration) // skip anim if user tries to swap during it
		{
			this->mNewGameAnimProgress = Board::cNewGameAnimDuration;
			this->mBoardSwerve = ::Vector2Zero();
		}
		return false;
	}

	int toRow = col;
	int toCol = row;
	::Vector2 swerve = ::Vector2Zero();
	float swerveAmount = Cyrey::Board::cSwerveCoeff * this->mTileSize;
	switch (direction)
	{
	case Cyrey::SwapDirection::Up:
		toCol--;
		swerve.y -= swerveAmount;
		break;
	case Cyrey::SwapDirection::Down:
		toCol++;
		swerve.y += swerveAmount;
		break;
	case Cyrey::SwapDirection::Left:
		toRow--;
		swerve.x -= swerveAmount;
		break;
	case Cyrey::SwapDirection::Right:
		toRow++;
		swerve.x += swerveAmount;
		break;
	default:
		return false;
	}
	this->AddSwerve(swerve);

	if (!this->mIsInReplay)
	{
		this->mReplayData->mCommands.emplace_back(this->mReplayData->mCommands.size(),
		                                          col, row, direction, this->mSecondsSinceLastCommand);
		this->mSecondsSinceLastCommand = 0.0f;
	}

	this->mSwapAnim = { col, row, direction };
	return this->TrySwap(col, row, toRow, toCol);
}

bool Cyrey::Board::TrySwap(int col, int row, int toCol, int toRow)
{
	if (!this->IsSwapLegal(col, row, toCol, toRow))
		return false;

	Piece temp = this->mBoard[row][col];
	if (temp.IsFlagSet(PieceFlag::Hypercube))
	{
		this->mCascadeNumber++;
		int piecesCleared = this->DoHypercube(this->mBoard[row][col], this->mBoard[toRow][toCol]);
		this->mStats.mPiecesCleared += piecesCleared;
		this->mStats.mScore += (piecesCleared - 2) * this->mGameConfig.mBaseScore * this->mScoreMultiplier * this->mCascadeNumber;
		this->mPiecesClearedInMove += piecesCleared;
		this->mFallDelay += this->mGameConfig.mFallDelay * 2;
		this->mCascadeNumber++;
		this->mBoard[row][col] = Cyrey::gNullPiece; // temp until I make a proper sequence
		return true;
	}
	this->mBoard[row][col] = this->mBoard[toRow][toCol];
	this->mBoard[toRow][toCol] = temp;
	this->mBoard[row][col].mBoardX = col;
	this->mBoard[row][col].mBoardY = row;
	this->mBoard[toRow][toCol].mBoardX = toCol;
	this->mBoard[toRow][toCol].mBoardY = toRow;

	// check only the pieces swapped for matches, everything else should be untouched if we can only swap from a non-moving state
	bool foundSet1 = this->FindSets(toCol, toRow, this->mBoard[toRow][toCol].mColor);
	bool foundSet2 = this->FindSets(col, row, this->mBoard[row][col].mColor);
	if (foundSet1 || foundSet2)
		this->mStats.mMovesMade++;
	else
	{
		this->mMissDelay += this->mGameConfig.mFallDelay * this->mGameConfig.mMissDelayMultiplier;
		this->PlaySound(ResSoundID::BadMove);
	}

	this->UpdateMatchSets();

	return true;
}

bool Cyrey::Board::IsSwapLegal(int col, int row, int toCol, int toRow) const
{
	if (!this->IsPositionLegal(toCol, toRow))
		return false; //out of bounds

	int xDiff = std::abs(col - toCol);
	int yDiff = std::abs(row - toRow);

	//swap with only adjacent squares. no diagonal swaps. swap with itself is also considered illegal
	if (xDiff > 1 || yDiff > 1 || !(xDiff ^ yDiff))
		return false;

	return true;
}

bool Cyrey::Board::CanSwap() const
{
	return this->mFallDelay <= 0.0f &&
		this->mMissDelay <= 0.0f &&
		this->mSecondsRemaining > 0.0f &&
		this->mNewGameAnimProgress >= Board::cNewGameAnimDuration;
}

constexpr bool Cyrey::Board::IsPositionLegal(int col, int row) const
{
	return !(col < 0 || row < 0 || col >= this->mWidth || row >= this->mHeight);
}

int Cyrey::Board::MatchPiece(Piece& piece, const Piece& byPiece, bool destroy)
{
	//do nothing if the parameter is the nullPiece
	if (piece.mID == 0)
		return 0;

	Piece pieceCopy = piece;
	if (piece.mImmunity && destroy)
		return 0;

	//no infinite recursions of special pieces destroying each other
	if (piece.mImmunity)
		return 1;

	// FIXME: The copy constructor of std::vector<AnimSparkle> is throwing on this call, while trying to alloc
	auto& anim = this->mMatchedPieceAnims.emplace_back(piece.mBoardX,
	                                                   piece.mBoardY,
	                                                   piece.mColor,
	                                                   destroy);
	if (destroy)
	{
		anim.mSparklesAmount = ::GetRandomValue(AnimSparkle::cMinSparkles, AnimSparkle::cMaxSparkles);
		for (int i = 0; i < anim.mSparklesAmount; i++)
		{
			anim.mSparkles[i] = AnimSparkle {
				static_cast<float>(::GetRandomValue(0, 360)),
				static_cast<float>(::GetRandomValue(0, 360)),
				static_cast<float>(::GetRandomValue(0, this->mTileSize / 2))
			};
		}
	}
	piece = Cyrey::gNullPiece;

	int piecesCleared = 1;
	if (pieceCopy.IsFlagSet(PieceFlag::Bomb))
	{
		this->mStats.mBombsDetonated++;
		this->PlaySound(ResSoundID::BombExplode);
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0)
					continue; //skip the bomb itself

				if (this->IsPositionLegal(pieceCopy.mBoardX + j, pieceCopy.mBoardY + i))
					piecesCleared += this->MatchPiece(this->mBoard[pieceCopy.mBoardY + i][pieceCopy.mBoardX + j],
					                                  pieceCopy, true);
			}
		}
		this->mFallDelay += this->mGameConfig.mFallDelay * 0.25f;
	}
	else if (pieceCopy.IsFlagSet(PieceFlag::Lightning))
	{
		this->mStats.mLightningsDetonated++;
		this->PlaySound(ResSoundID::LightningExplode);
		for (int i = 0; i < this->mGameConfig.mLightningPiecesAmount; i++)
		{
			uint32_t row, col, count = 0;
			do
			{
				count++;
				row = this->mApp->GetRandomNumber(0, this->mHeight - 1);
				col = this->mApp->GetRandomNumber(0, this->mWidth - 1);

				if (count > (this->mWidth * this->mHeight * 10))
				[[unlikely]]
				{
					::TraceLog(TraceLogLevel::LOG_DEBUG, "Lightning piece activation couldn't find a vacant piece");
					break; //no need to set x and y to anything as calling MatchPiece on a nullPiece is fine
				}
			}
			while (this->mBoard[row][col].mID == 0);

			piecesCleared += this->MatchPiece(this->mBoard[row][col], pieceCopy, true);
		}
		this->mFallDelay += this->mGameConfig.mFallDelay;
	}
	else if (pieceCopy.IsFlagSet(PieceFlag::Hypercube))
	{
		piecesCleared += this->DoHypercube(pieceCopy, byPiece);
		this->mFallDelay += this->mGameConfig.mFallDelay;
	}

	return piecesCleared;
}

int Cyrey::Board::DoHypercube(const Piece& cubePiece, const Piece& byPiece)
{
	//if called on a piece that isn't a hypercube for some reason, don't resolve
	if (!cubePiece.IsFlagSet(PieceFlag::Hypercube))
		return 0;

	this->PlaySound(ResSoundID::HypercubeExplode);
	this->mStats.mHypercubesDetonated++;
	int piecesCleared = 1;
	PieceColor targetColor = byPiece.mColor != PieceColor::Uncolored ? byPiece.mColor : cubePiece.mOldColor;
	bool wantDHR = byPiece.IsFlagSet(PieceFlag::Hypercube);

	for (auto& row : this->mBoard)
	{
		for (auto& piece : row)
		{
			if (piece.mColor == targetColor || wantDHR)
				piecesCleared += this->MatchPiece(piece, cubePiece, true);
		}
	}

	return piecesCleared;
}

void Cyrey::Board::PlayReplay()
{
	if (this->mReplayData->mConfigVersion != this->mGameConfig.mVersion)
		return;

	this->mIsInReplay = true;
	this->mReplayCopy = std::make_unique<Replay>(*this->mReplayData);
	this->ResetBoard();
}

void Cyrey::Board::PlayReplay(const Replay& replay)
{
	this->mReplayData = std::make_unique<Replay>(replay);
	this->PlayReplay();
}

void Cyrey::Board::SetReplayTo(float secs)
{
	// edge values don't reset endgame animation
	if (secs < 0.0f)
		secs = 0.0f;
	else if (secs >= this->mGameConfig.mStartingTime)
		secs = std::nextafter(this->mGameConfig.mStartingTime, 0.0f);

	this->mHasSeekedReplay = true;
	this->PlayReplay();
	this->mNewGameAnimProgress = Board::cNewGameAnimDuration + 1;
	this->mDroppedNewGamePieces = true;
	this->mSecondsRemaining = this->mGameConfig.mStartingTime - secs;
	this->mSecondsSinceLastCommand = secs;
	this->mBoardSwerve = {0.0f, 0.0f};
	this->mFallDelay -= secs;
	this->mMissDelay -= secs;
	this->mApp->SeekMusic(ResMusicID::Blitz1min, secs);
}

void Cyrey::Board::UpdateUI()
{
	this->mZoomPct += ::GetMouseWheelMove();
	// perhaps change this to the Camera functionality in raylib? seems a lot more versatile

	static ::Vector2 lastVector { 0.0f, 0.0f };
	::Vector2 pinch { std::abs(::GetGesturePinchVector().x), std::abs(::GetGesturePinchVector().y) };
	if (pinch.x != 0.0f && pinch.y != 0.0f && (lastVector.x != 0.0f && lastVector.y != 0.0f))
		this->mZoomPct += (pinch.x - lastVector.x + (pinch.y - lastVector.y) * 2) * 5;
	lastVector = pinch;

	if (::IsMouseButtonPressed(::MouseButton::MOUSE_BUTTON_MIDDLE) || ::GetTouchPointCount() >= 4)
		this->mZoomPct = Board::cDefaultZoomPct;

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
}

bool Cyrey::Board::UpdateReplay()
{
	if (this->mReplayCopy->mCommands.empty())
		return false;

	auto& nextCmd = this->mReplayCopy->mCommands.front();
	if (this->mSecondsSinceLastCommand < nextCmd.mSecondsSinceLastCmd)
		return false;

	if (this->mHasSeekedReplay)
	{
		this->mMatchedPieceAnims.clear();
		this->mDroppedPieceAnims.clear();
		this->mBoardSwerve = ::Vector2Zero();
	}
	bool swapSuccessful = this->TrySwap(nextCmd.mBoardCol, nextCmd.mBoardRow, nextCmd.mDirection);
	// TODO: Add an #ifdef _DEBUG here later
	bool swapQueued = this->mQueuedSwapDirection != SwapDirection::None;
	::TraceLog((swapSuccessful || swapQueued) ? ::TraceLogLevel::LOG_INFO : ::TraceLogLevel::LOG_WARNING,
	           ::TextFormat("%s Replay move %d: sslc:%f, repsslc:%f, fallDelay:%f, secondsRemaining:%f. %d left.",
	                        swapSuccessful ? "(S)" : swapQueued ? "(Q)" : "(F)",
	                        nextCmd.mCommandNumber + 1,
	                        this->mSecondsSinceLastCommand,
	                        nextCmd.mSecondsSinceLastCmd,
	                        this->mFallDelay,
	                        this->mSecondsRemaining,
	                        this->mReplayCopy->mCommands.size() - 1));
	this->mSecondsSinceLastCommand -= nextCmd.mSecondsSinceLastCmd;
	this->mReplayCopy->mCommands.pop_front();
	return true;
}

void Cyrey::Board::UpdateSwapAnim()
{
	this->mSwapAnim.mOpacity -= SwapAnim::cStartingOpacity *
		(this->GetStepInterval() / this->mGameConfig.mFallDelay);
	if (this->mSwapAnim.mOpacity < 0.0f)
	{
		this->mSwapAnim.mOpacity = 0.0f;
		this->mSwapAnim.mDirection = SwapDirection::None;
	}
}

void Cyrey::Board::UpdateMatchedPieceAnims()
{
	for (auto& anim : this->mMatchedPieceAnims)
	{
		anim.mOpacity -= PieceMatchAnim::cStartingOpacity * (this->GetStepInterval() / this->mGameConfig.mFallDelay);
		for (int i = 0; i < anim.mSparklesAmount; ++i)
		{
			anim.mSparkles[i].mRotationDeg += AnimSparkle::cRotationPerSec * this->GetStepInterval();
			anim.mSparkles[i].mDistance += AnimSparkle::cSpeed * this->GetStepInterval();
		}
	}
	std::erase_if(this->mMatchedPieceAnims, [](const PieceMatchAnim& anim) { return anim.mOpacity <= 0.0f; });
}

void Cyrey::Board::UpdateDroppedPieceAnims()
{
	for (auto& anim : this->mDroppedPieceAnims)
		anim.mOpacity -= PieceDropAnim::cStartingOpacity * (this->GetStepInterval() / this->mGameConfig.mFallDelay);

	std::erase_if(this->mDroppedPieceAnims, [](const PieceDropAnim& anim) { return anim.mOpacity <= 0.0f; });
}

void Cyrey::Board::UpdateGameOverAnim()
{
	if (this->mSecondsRemaining > 0.0f || this->mFallDelay > 0.0f || !this->mDroppedNewGamePieces || this->mIsGameOver)
		return;

	// Implicit casts are intentional, explicit ones would really bloat this function
	int rowsClearedBeforeUpdate = this->mHeight * (this->mGameOverAnimProgress / Board::cGameOverAnimDuration);
	if ((this->mGameOverAnimProgress += this->GetStepInterval()) > Board::cGameOverAnimDuration)
	{
		this->mGameOverAnimProgress = Board::cGameOverAnimDuration; // failsafe
		this->mIsGameOver = true;
		if (this->mIsInReplay && this->mReplayCopy->mStats.mScore != this->mStats.mScore)
			::TraceLog(::TraceLogLevel::LOG_WARNING, "Replay score mismatch!");
		else
			this->mReplayData->mStats = this->mStats;

		if (!this->mIsInReplay)
			this->UpdateCurrentUserStats();

		this->mApp->PlayMusic(ResMusicID::ResultsScreenBlitz1Min);
	}
	int rowsClearedAfterUpdate = this->mHeight * (this->mGameOverAnimProgress / Board::cGameOverAnimDuration);

	if (rowsClearedBeforeUpdate < rowsClearedAfterUpdate)
	{
		for (auto& piece : this->mBoard[rowsClearedBeforeUpdate])
		{
			this->mMatchedPieceAnims.emplace_back(piece.mBoardX, piece.mBoardY, piece.mColor, false);
			piece = Cyrey::gNullPiece;
		}
		this->PlaySound(ResSoundID::RowBlow);
	}
}

void Cyrey::Board::UpdateCurrentUserStats() const
{
	User& user = *this->mApp->mCurrentUser;
	auto& stats = this->mStats;
	user.mXP += stats.mScore;
	user.mPiecesCleared += stats.mPiecesCleared;
	user.mGamesPlayed++;
	user.mBombsDetonated += stats.mBombsDetonated;
	user.mLightningsDetonated += stats.mLightningsDetonated;
	user.mHypercubesDetonated += stats.mHypercubesDetonated;
	user.mBestMovePoints = std::max(user.mBestMovePoints, stats.mBestMovePoints);
	user.mBestMoveCascades = std::max(user.mBestMoveCascades, stats.mBestMoveCascades);

	this->mApp->SaveCurrentUserData();
}

void Cyrey::Board::UpdateBoardSwerve()
{
	float fallDelay = 1 - std::pow(this->mGameConfig.mFallDelay * 0.003f, this->GetStepInterval());
	this->mBoardSwerve.x = ::Lerp(this->mBoardSwerve.x, 0, fallDelay);
	this->mBoardSwerve.y = ::Lerp(this->mBoardSwerve.y, 0, fallDelay);
}

bool Cyrey::Board::UpdateNewGameAnim()
{
	if (this->mNewGameAnimProgress > Board::cNewGameAnimDuration)
		return false;

	this->mSecondsRemaining = (this->mNewGameAnimProgress / Board::cNewGameAnimDuration) * this->mGameConfig.mStartingTime;
	this->mNewGameAnimProgress += this->GetStepInterval();

	if (this->mNewGameAnimProgress >= (Board::cNewGameAnimDuration * 0.75f) && !this->mDroppedNewGamePieces)
	{
		for (int i = 0; i < this->mWidth; i++)
			this->mDroppedPieceAnims.emplace_back(i);

		this->mDroppedNewGamePieces = true;
		this->AddSwerve(::Vector2 { 0.0f, this->mTileSize * Board::cSwerveCoeff * 3 });
	}

	if (this->mNewGameAnimProgress >= Board::cNewGameAnimDuration)
	{
		this->mNewGameAnimProgress = Board::cNewGameAnimDuration + 1;
		this->mSecondsRemaining = this->mGameConfig.mStartingTime; // failsafe
		this->mSecondsSinceLastCommand = 0.0f;
		this->HandleQueuedSwaps();
	}

	if (this->mSecondsRemaining > this->mGameConfig.mStartingTime)
		this->mSecondsRemaining = this->mGameConfig.mStartingTime; // failsafe

	return true;
}

void Cyrey::Board::UpdateDragging()
{
	::Vector2 mousePos = ::GetMousePosition();
	static ::Vector2 dragMouseBegin { ::Vector2Zeros };
	static ::Vector2 dragTileBegin { ::Vector2Zeros };
	static bool triedSwap = false;
	if (::IsMouseButtonDown(::MouseButton::MOUSE_BUTTON_LEFT))
	{
		if (!this->mDragging
			&& this->IsMouseInBoard()
			&& !triedSwap)
		{
			this->mDragging = true;
			dragMouseBegin = mousePos;
			dragTileBegin = *this->GetHoveredTile(); // we're guaranteed to have a value here
		}
		else if (this->mDragging && this->mFallDelay <= this->mApp->mSettings->mQueueSwapTolerance)
		{
			float xDiff = dragMouseBegin.x - mousePos.x;
			float yDiff = dragMouseBegin.y - mousePos.y;

			auto xTileBegin = static_cast<int>(dragTileBegin.x);
			auto yTileBegin = static_cast<int>(dragTileBegin.y);

			if (abs(xDiff) > (this->mTileSize * this->mApp->mSettings->mSwapDeadZone))
			{
				this->TrySwap(xTileBegin, yTileBegin, (xDiff > 0 ? SwapDirection::Left : SwapDirection::Right));
				this->mDragging = false;
				triedSwap = true;
			}
			else if (abs(yDiff) > (this->mTileSize * this->mApp->mSettings->mSwapDeadZone))
			{
				this->TrySwap(xTileBegin, yTileBegin, (yDiff > 0 ? SwapDirection::Up : SwapDirection::Down));
				this->mDragging = false;
				triedSwap = true;
			}
		}
	}
	else if (::IsMouseButtonReleased(::MouseButton::MOUSE_BUTTON_LEFT))
	{
		if (this->mDragging)
			this->SelectPiece(dragTileBegin.x, dragTileBegin.y);
		this->mDragging = false;
		dragMouseBegin = ::Vector2Zero();
		dragTileBegin = ::Vector2Zero();
		triedSwap = false;
	}
}

size_t Cyrey::Board::UpdateMatchSets()
{
	size_t matchSets = this->mMatchSets.size();
	if (matchSets <= 0)
		return matchSets;

	this->mFallDelay += this->mGameConfig.mFallDelay;
	this->AddSwerve(::Vector2 {
		0.0f,
		Board::cSwerveCoeff *
		static_cast<float>(std::min(this->mCascadeNumber, Board::cMaxCascadesSwerve)) *
		this->mTileSize * 0.75f
	});
	this->mCascadeNumber++;
	if (this->mCascadeNumber <= 1)
		this->PlaySound(ResSoundID::Match);
	else
	{
		this->mApp->SetSoundPitch(ResSoundID::Cascade,
			0.7f + (static_cast<float>(std::min(this->mCascadeNumber, Board::cMaxCascadesSwerve)) * 0.15f));
		this->PlaySound(ResSoundID::Cascade);
	}

	for (auto& [mPieces, mAddedPieces] : this->mMatchSets)
	{
		size_t piecesPerSet = mPieces.size();
		size_t addedPiecesPerSet = mAddedPieces.size();
		// make the special at the added piece, or in the middle of the set if no pieces were added
		Piece* addedPiece = addedPiecesPerSet > 0 ?
			                    mAddedPieces[addedPiecesPerSet / 2] :
			                    mPieces[piecesPerSet / 2];
		int piecesCleared = 0;

		for (auto piece : mPieces)
		{
			if (piece == addedPiece && piecesPerSet == 4)
			{
				addedPiece->Bombify();
				this->PlaySound(ResSoundID::BombCreate);
			}
			else if (piece == addedPiece && piecesPerSet == 5)
			{
				addedPiece->Lightningify();
				this->PlaySound(ResSoundID::LightningCreate);
			}
			else if (piece == addedPiece && piecesPerSet >= 6)
			{
				addedPiece->Hypercubify();
				this->PlaySound(ResSoundID::HypercubeCreate);
			}

			piecesCleared += this->MatchPiece(*piece);
		}
		this->mStats.mPiecesCleared += piecesCleared;
		int scoreForCascade = (piecesCleared - 2) * this->mGameConfig.mBaseScore * this->mScoreMultiplier * this->mCascadeNumber;
		this->mStats.mScore += scoreForCascade;
		this->mScoreInMove += scoreForCascade;
		this->mPiecesClearedInMove += piecesCleared;
	}

	this->mMatchSets.clear();
	return matchSets;
}

void Cyrey::Board::UpdateFalling()
{
	for (int i = this->mHeight - 1; i >= 0; i--)
	{
		for (int j = this->mWidth - 1; j >= 0; j--)
		{
			int k = 1;
			while (this->IsPositionLegal(j, i + k) && this->mBoard[i + k][j].mID == 0)
			{
				this->mBoard[i + k][j] = this->mBoard[i + k - 1][j];
				this->mBoard[i + k][j].mBoardY = i + k;
				this->mBoard[i + k - 1][j] = Cyrey::gNullPiece;
				k++;
			}
		}
	}
	this->PlaySound(ResSoundID::PieceFall);
	this->FillInBlanks();
}

void Cyrey::Board::UpdateDroppedFiles()
{
	static std::optional<Replay> replay = std::nullopt;
	static bool hasDroppedFile = false;
	if (::IsFileDropped())
	{
		::FilePathList fileList = ::LoadDroppedFiles();
		if (fileList.paths != nullptr)
			replay = Replay::OpenReplayFile(fileList.paths[0]);

		hasDroppedFile = true;
		::UnloadDroppedFiles(fileList);
	}

	if (!hasDroppedFile)
		return;

	// TODO: handle dropped files in the CyreyApp class instead

	if (replay.has_value())
	{
		this->PlayReplay(*replay);
		replay = std::nullopt;
		hasDroppedFile = false;
	}
	else
	{
		if (this->mApp->DrawDialog("Error", "Failed to open file.", "OK") != -1)
			hasDroppedFile = false;
	}
}

float Cyrey::Board::GetStepInterval() const
{
	return this->mApp->GetDeltaTime() * this->mGameSpeed;
}

void Cyrey::Board::FillInBlanks()
{
	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			Piece& piece = this->mBoard[i][j];
			if (piece.mID == 0)
			{
				piece = Piece(static_cast<PieceColor>(this->mApp->GetRandomNumber(1, this->mGameConfig.mPieceColorAmount)));
				piece.mBoardX = j;
				piece.mBoardY = i;
				this->mDroppedPieceAnims.emplace_back(j);
			}
		}
	}
	if (!this->FindSets())
	{
		if (this->mCascadeNumber > this->mStats.mBestMoveCascades)
			this->mStats.mBestMoveCascades = this->mCascadeNumber;
		if (this->mScoreInMove > this->mStats.mBestMovePoints)
			this->mStats.mBestMovePoints = this->mScoreInMove;

		this->mCascadeNumber = 0;
		this->mScoreInMove = 0;
		this->mPiecesClearedInMove = 0;
	}
}

void Cyrey::Board::HandleQueuedSwaps()
{
	if (this->mQueuedSwapDirection != SwapDirection::None && this->CanSwap()) // queued swap shouldn't queue another
	{
		this->TrySwap(this->mQueuedSwapPos.x, this->mQueuedSwapPos.y, this->mQueuedSwapDirection);
		this->mQueuedSwapDirection = SwapDirection::None;
	}
	// preserve the queued swap through cascades only if tolerance set to inf
	if (this->mApp->mSettings->mQueueSwapTolerance < 1.0f)
		this->mQueuedSwapDirection = SwapDirection::None;
}

void Cyrey::Board::PlaySound(ResSoundID sound) const
{
	if (!this->mHasSeekedReplay)
		this->mApp->PlaySound(sound);
}

void Cyrey::Board::DrawCheckerboard() const
{
	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			::DrawRectangleRec(
				::Rectangle {
					j * this->mTileSize + this->mXOffset,
					i * this->mTileSize + this->mYOffset,
					this->mTileSize,
					this->mTileSize
				},
				(i % 2) ^ (j % 2) ?
					//alternate between light and dark every row
					::ColorAlpha(::LIGHTGRAY, Board::cBoardAlpha) :
					::ColorAlpha(::GRAY, Board::cBoardAlpha));
		}
	}
}

void Cyrey::Board::DrawBorder() const
{
	// draw the outline
	::Color outlineColor = this->mApp->mDarkMode ? ::GRAY : ::DARKGRAY;
	if (this->mMissDelay > 0.0f || this->mSecondsRemaining <= 0.0f)
		outlineColor = ::RED;

	::DrawRectangleRoundedLinesEx(
		::Rectangle {
			this->mXOffset - 1,
			this->mYOffset - 1,
			(this->mTileSize * static_cast<float>(this->mWidth)) + 2,
			(this->mTileSize * static_cast<float>(this->mHeight)) + 2
		},
		0.0f, 1, this->mTileInset, outlineColor);

	// draw the timer - way more complicated than I thought it would be. definitely review and simplify later.
	float thick = this->mTileInset * 2;
	float offset = thick / 2;
	//Rectangle draws thickness outwards, Line draws it in the middle, shift it to fit somewhat
	::Color timerColor = this->mSecondsRemaining < 10 ? ::RED : ::GREEN;

	float halfPerimeter = ((static_cast<float>(this->mWidth) * this->mTileSize) +
		(static_cast<float>(this->mHeight) * this->mTileSize) + thick * 2 + offset);
	float fillPct = this->mSecondsRemaining / this->mGameConfig.mStartingTime;
	if (this->mMissDelay > 0.0f)
	{
		fillPct = 1;
		timerColor = ::RED;
	}

	float fillLength = halfPerimeter * fillPct;
	float firstCurveLen = ((static_cast<float>(this->mWidth) * this->mTileSize) / 2) + thick;
	float secondCurveLen = firstCurveLen + (static_cast<float>(this->mHeight) * this->mTileSize) + thick;

	// intentional implicit casts
	::Vector2 startPos {
		this->mXOffset + (static_cast<float>(this->mWidth) / 2 * this->mTileSize),
		this->mYOffset + (this->mHeight * this->mTileSize) + offset
	};
	::Vector2 firstCurvePointLeft {
		this->mXOffset - offset,
		this->mYOffset + (this->mHeight * this->mTileSize) + offset
	};
	::Vector2 firstCurvePointRight {
		this->mXOffset + (this->mWidth * this->mTileSize) + offset,
		this->mYOffset + (this->mHeight * this->mTileSize) + offset
	};
	::Vector2 secondCurvePointLeft {
		this->mXOffset - offset,
		this->mYOffset - offset
	};
	::Vector2 secondCurvePointRight {
		this->mXOffset + (this->mWidth * this->mTileSize) + offset,
		this->mYOffset - offset
	};

	bool wantFirstCurve = fillLength >= firstCurveLen;
	bool wantSecondCurve = fillLength >= secondCurveLen;

	float leftX = wantSecondCurve ?
		              secondCurvePointLeft.x + fillLength - secondCurveLen :
		              wantFirstCurve ?
		              firstCurvePointLeft.x :
		              startPos.x - fillLength;
	float leftY = wantSecondCurve ?
		              secondCurvePointLeft.y :
		              wantFirstCurve ?
		              firstCurvePointLeft.y - (fillLength - firstCurveLen) :
		              firstCurvePointLeft.y;
	float rightX = wantSecondCurve ?
		               secondCurvePointRight.x - (fillLength - secondCurveLen) :
		               wantFirstCurve ?
		               firstCurvePointRight.x :
		               startPos.x + fillLength;
	float rightY = wantSecondCurve ?
		               secondCurvePointRight.y :
		               wantFirstCurve ?
		               firstCurvePointRight.y - (fillLength - firstCurveLen) :
		               firstCurvePointRight.y;
	::Vector2 lastLeft { leftX, leftY };
	::Vector2 lastRight { rightX, rightY };

	int edgeCircleSegments = 6;
	if (wantSecondCurve)
	{
		//left
		::DrawLineEx(startPos, firstCurvePointLeft, thick, timerColor);
		::DrawCircleSector(firstCurvePointLeft, offset, 90, 180, edgeCircleSegments, timerColor);
		::DrawLineEx(firstCurvePointLeft, secondCurvePointLeft, thick, timerColor);
		::DrawCircleSector(secondCurvePointLeft, offset, 180, 270, edgeCircleSegments, timerColor);
		::DrawLineEx(secondCurvePointLeft, lastLeft, thick, timerColor);

		//right
		::DrawLineEx(startPos, firstCurvePointRight, thick, timerColor);
		::DrawCircleSector(firstCurvePointRight, offset, 90, 0, edgeCircleSegments, timerColor);
		::DrawLineEx(firstCurvePointRight, secondCurvePointRight, thick, timerColor);
		::DrawCircleSector(secondCurvePointRight, offset, 270, 360, edgeCircleSegments, timerColor);
		::DrawLineEx(secondCurvePointRight, lastRight, thick, timerColor);
	}
	else if (wantFirstCurve)
	{
		//left
		::DrawLineEx(startPos, firstCurvePointLeft, thick, timerColor);
		::DrawCircleSector(firstCurvePointLeft, offset, 90, 180, edgeCircleSegments, timerColor);
		::DrawLineEx(firstCurvePointLeft, lastLeft, thick, timerColor);

		//right
		::DrawLineEx(startPos, firstCurvePointRight, thick, timerColor);
		::DrawCircleSector(firstCurvePointRight, offset, 90, 0, edgeCircleSegments, timerColor);
		::DrawLineEx(firstCurvePointRight, lastRight, thick, timerColor);
	}
	else
	{
		//left
		::DrawLineEx(startPos, lastLeft, thick, timerColor);

		//right
		::DrawLineEx(startPos, lastRight, thick, timerColor);
	}
}

void Cyrey::Board::DrawReplayControls()
{
	float controlHeight = static_cast<float>(this->mApp->mHeight) / 25.0f;

	::Rectangle seekingBar {
		this->mTileSize / 2,
		static_cast<float>(this->mApp->mHeight) / 3,
		this->mXOffset - this->mTileSize,
		controlHeight
	};

	float oldSeconds = this->mGameConfig.mStartingTime - this->mSecondsRemaining;
	float oldSeconds2 = oldSeconds;
	::GuiSlider(
		seekingBar,
		"",
		"",
		&oldSeconds,
		0.0f,
		this->mGameConfig.mStartingTime);

	static bool isDragging = false;
	if (isDragging)
	{
		if (::IsMouseButtonUp(::MouseButton::MOUSE_BUTTON_LEFT))
		{
			isDragging = false;
			this->mIsPaused = false;
		}
		else if (::GetMouseDelta().x != 0.0f || ::GetMouseDelta().y != 0.0f) // also updates if the bar is alr min or max
		{
			this->mIsPaused = false;
			this->SetReplayTo(oldSeconds);
		}
	}
	else if (::CheckCollisionPointRec(::GetMousePosition(), seekingBar))
	{
		if (::IsMouseButtonPressed(::MouseButton::MOUSE_BUTTON_LEFT))
		{
			isDragging = true;
			this->SetReplayTo(oldSeconds);
		}
	}

	controlHeight *= 2;
	::GuiSetIconScale(static_cast<int>(controlHeight / 16.0f));
	::Rectangle speedLabel {
		this->mXOffset / 2 - controlHeight / 2,
		static_cast<float>(this->mApp->mHeight) / 3 - controlHeight * 2,
		controlHeight,
		controlHeight
	};
	::Rectangle middleButton {
		this->mXOffset / 2 - controlHeight / 2,
		static_cast<float>(this->mApp->mHeight) / 3 - controlHeight,
		controlHeight,
		controlHeight
	};
	::Rectangle slowDownButton {
		this->mXOffset / 2 - controlHeight * 1.5f,
		static_cast<float>(this->mApp->mHeight) / 3 - controlHeight,
		controlHeight,
		controlHeight
	};
	::Rectangle backFrameButton {
		this->mXOffset / 2 - controlHeight * 2.5f,
		static_cast<float>(this->mApp->mHeight) / 3 - controlHeight,
		controlHeight,
		controlHeight
	};
	::Rectangle speedUpButton {
		this->mXOffset / 2 + controlHeight / 2,
		static_cast<float>(this->mApp->mHeight) / 3 - controlHeight,
		controlHeight,
		controlHeight
	};
	::Rectangle forwardFrameButton {
		this->mXOffset / 2 + controlHeight * 1.5f,
		static_cast<float>(this->mApp->mHeight) / 3 - controlHeight,
		controlHeight,
		controlHeight
	};
	if (::GuiButton(middleButton,
		::GuiIconText(this->mIsPaused ? ::GuiIconName::ICON_PLAYER_PLAY : ::GuiIconName::ICON_PLAYER_PAUSE, "")))
	{
		this->mIsPaused ^= 1;
	}
	if (::GuiButton(backFrameButton,
		::GuiIconText(::GuiIconName::ICON_PLAYER_PREVIOUS, "")))
	{
		this->mIsPaused = false;
		this->SetReplayTo((this->mGameConfig.mStartingTime - this->mSecondsRemaining) - (this->GetStepInterval() * 2));
	}
	if (::GuiButton(forwardFrameButton,
		::GuiIconText(::GuiIconName::ICON_PLAYER_NEXT, "")))
	{
		this->mIsPaused = false;
		this->mHasSeekedReplay = true;
	}
	if (::GuiButton(slowDownButton,
		::GuiIconText(::GuiIconName::ICON_ARROW_LEFT_FILL, "")))
	{
		this->mGameSpeed = std::max(0.25f, this->mGameSpeed - 0.25f);
	}
	if (::GuiButton(speedUpButton,
		::GuiIconText(::GuiIconName::ICON_ARROW_RIGHT_FILL, "")))
	{
		this->mGameSpeed = std::min(3.0f, this->mGameSpeed + 0.25f);
	}
	if (::GuiLabelButton(speedLabel, ::TextFormat("x%.2f", this->mGameSpeed)))
		this->mGameSpeed = 1.0f;
}

void Cyrey::Board::DrawPieces() const
{
	//don't draw the pieces on gameover
	if (this->mIsGameOver || !this->mDroppedNewGamePieces)
		return;

	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			auto color = ::BLANK;
			int sides = 3;
			float rotation = 0;
			float radius = (static_cast<float>(this->mTileSize) / 2) - this->mTileInset;
			::Vector2 center {
				((this->mTileSize * static_cast<float>(j)) + this->mXOffset + this->mTileSize / 2),
				((this->mTileSize * static_cast<float>(i)) + this->mYOffset + this->mTileSize / 2)
			};

			switch (this->mBoard[i][j].mColor)
			{
			case PieceColor::Red:
				color = ::RED;
				sides = 4;
				rotation = 45.0f;
				radius += this->mTileInset;
				break;
			case PieceColor::Green:
				color = ::GREEN;
				sides = 8;
				break;
			case PieceColor::Blue:
				color = ::BLUE;
				sides = 3;
				center.y -= this->mTileSize / 10.0f;
				rotation = 210.0f;
				radius += this->mTileInset;
				break;
			case PieceColor::Yellow:
				color = ::YELLOW;
				sides = 4;
				break;
			case PieceColor::Orange:
				color = ::ORANGE;
				sides = 6;
			// rotation = 90.0f;
				break;
			case PieceColor::White:
				color = this->mApp->mDarkMode ? ::WHITE : ::DARKGRAY;
				sides = 12;
				break;
			case PieceColor::Purple:
				color = ::PURPLE;
				sides = 3;
				rotation = 150.0f;
				center.y += this->mTileSize / 10.0f;
				radius += this->mTileInset;
				break;
			default:
				break;
			}

			::DrawPoly(center, sides, radius, rotation, color);
			if (this->mBoard[i][j].IsFlagSet(PieceFlag::Bomb))
				::DrawPolyLinesEx(center, sides, radius + this->mTileInset / 2, rotation, this->mTileSize / 10,
				                  ::ORANGE);
			if (this->mBoard[i][j].IsFlagSet(PieceFlag::Lightning))
				::DrawPolyLinesEx(center, sides, radius + this->mTileInset / 2, rotation, this->mTileSize / 10,
				                  ::SKYBLUE);
			if (this->mBoard[i][j].IsFlagSet(PieceFlag::Hypercube))
			{
				::DrawCircleV(center, radius, ::PINK);
				::DrawPolyLinesEx(center, 4, radius, rotation, this->mTileSize / 5, ::BLUE);
			}
		}
	}
}

void Cyrey::Board::DrawSwapAnim() const
{
	if (this->mSwapAnim.mDirection == SwapDirection::None)
		return;

	float x = this->mXOffset + (this->mSwapAnim.mRow * this->mTileSize) + this->mTileInset;
	float y = this->mYOffset + (this->mSwapAnim.mCol * this->mTileSize) + this->mTileInset;
	if (this->mSwapAnim.mDirection == SwapDirection::Up)
		y -= this->mTileSize;
	if (this->mSwapAnim.mDirection == SwapDirection::Left)
		x -= this->mTileSize;

	float width = this->mTileSize - (this->mTileInset * 2);
	float height = this->mTileSize - (this->mTileInset * 2);
	if (this->mSwapAnim.mDirection == SwapDirection::Left || this->mSwapAnim.mDirection == SwapDirection::Right)
		width += width + this->mTileInset * 2;
	if (this->mSwapAnim.mDirection == SwapDirection::Up || this->mSwapAnim.mDirection == SwapDirection::Down)
		height += height + this->mTileInset * 2;

	::Color from = this->mApp->mDarkMode ?
		               ::ColorAlpha(::RAYWHITE, this->mSwapAnim.mOpacity) :
		               ::ColorAlpha(::GRAY, this->mSwapAnim.mOpacity);
	::Color to = this->mApp->mDarkMode ?
		             ::ColorAlpha(::GRAY, this->mSwapAnim.mOpacity) :
		             ::ColorAlpha(::RAYWHITE, this->mSwapAnim.mOpacity);

	::DrawRectangleGradientEx({ x, y, width, height },
	                          (this->mSwapAnim.mDirection == SwapDirection::Down || this->mSwapAnim.mDirection ==
		                          SwapDirection::Right) ?
		                          to :
		                          from,
	                          (this->mSwapAnim.mDirection == SwapDirection::Up || this->mSwapAnim.mDirection ==
		                          SwapDirection::Right) ?
		                          to :
		                          from,
	                          (this->mSwapAnim.mDirection == SwapDirection::Up || this->mSwapAnim.mDirection ==
		                          SwapDirection::Left) ?
		                          to :
		                          from,
	                          (this->mSwapAnim.mDirection == SwapDirection::Down || this->mSwapAnim.mDirection ==
		                          SwapDirection::Left) ?
		                          to :
		                          from);
}

void Cyrey::Board::DrawPieceMatchAnims() const
{
	for (auto& anim : this->mMatchedPieceAnims)
	{
		auto color = ::BLANK;
		int sides = 3;
		float rotation = 0;
		float radius = (this->mTileSize / 2) - this->mTileInset;
		::Vector2 center {
			((anim.mBoardX * this->mTileSize) + this->mXOffset + this->mTileSize / 2),
			((anim.mBoardY * this->mTileSize) + this->mYOffset + this->mTileSize / 2)
		};

		switch (anim.mColor)
		{
		case PieceColor::Red:
			color = ::RED;
			sides = 4;
			rotation = 45.0f;
			radius += this->mTileInset;
			break;
		case PieceColor::Green:
			color = ::GREEN;
			sides = 8;
			break;
		case PieceColor::Blue:
			color = ::BLUE;
			sides = 3;
			rotation = 210.0f;
			center.y -= this->mTileSize / 10.0f;
			radius += this->mTileInset;
			break;
		case PieceColor::Yellow:
			color = ::YELLOW;
			sides = 4;
			break;
		case PieceColor::Orange:
			color = ::ORANGE;
			sides = 6;
		// rotation = 90.0f;
			break;
		case PieceColor::White:
			color = this->mApp->mDarkMode ? ::WHITE : ::DARKGRAY;
			sides = 12;
			break;
		case PieceColor::Purple:
			color = ::PURPLE;
			sides = 3;
			rotation = 150.0f;
			center.y += this->mTileSize / 10.0f;
			radius += this->mTileInset;
			break;
		default:
			break;
		}
		color = ::ColorAlpha(color, anim.mOpacity);

		if (anim.mDestroyed)
		{
			for (int i = 0; i < anim.mSparklesAmount; ++i)
			{
				const auto& [mRotationDeg, mDirectionAngleDeg, mDistance] = anim.mSparkles[i];
				::Vector2 rotationVec = ::Vector2Rotate(::Vector2 { 1, 0 }, mDirectionAngleDeg);
				::Vector2 offsetVec = ::Vector2Add(center, ::Vector2Scale(rotationVec, mDistance));

				::DrawPoly(offsetVec, 4, radius * 0.2f, mRotationDeg, color);
			}
		}
		else
			::DrawPoly(center, sides, radius, rotation, color);
	}
}

void Cyrey::Board::DrawPieceDropAnims() const
{
	for (auto& anim : this->mDroppedPieceAnims)
	{
		float x = this->mXOffset + (anim.mBoardCol * this->mTileSize) + this->mTileInset;
		float y = this->mYOffset - (this->mTileSize * 3);
		::Color from = this->mApp->mDarkMode ?
			               ::ColorAlpha(::RAYWHITE, anim.mOpacity) :
			               ::ColorAlpha(::BLACK, anim.mOpacity);
		::Color to = this->mApp->mDarkMode ?
			             ::ColorAlpha(::BLACK, anim.mOpacity) :
			             ::ColorAlpha(::RAYWHITE, anim.mOpacity);

		::DrawRectangleGradientV(x, y, this->mTileSize - (this->mTileInset * 2),
		                         this->mTileSize * 2.75f, from, to);
	}
}

void Cyrey::Board::DrawHoverSquare() const
{
	if (this->mIsInReplay)
		return;

	auto rectColor = ::ORANGE;
	if (this->mDragging)
		rectColor = ::GREEN;
	if (!this->CanSwap())
		rectColor = ::RED;

	if (std::optional<::Vector2> hoveredTile = this->GetHoveredTile(); hoveredTile.has_value())
	{
		::DrawRectangleRoundedLinesEx(::Rectangle {
			                            hoveredTile->x * this->mTileSize + this->mXOffset + 1,
			                            hoveredTile->y * this->mTileSize + this->mYOffset + 1,
			                            this->mTileSize - 2,
			                            this->mTileSize - 2
		                            },
		                            0.0f, 1, this->mTileInset / 2.0f, rectColor);
	}
	if (this->mSelectedTile.has_value())
	{
		::DrawRectangleRoundedLinesEx(::Rectangle {
										this->mSelectedTile->x * this->mTileSize + this->mXOffset + 1,
										this->mSelectedTile->y * this->mTileSize + this->mYOffset + 1,
										this->mTileSize - 2,
										this->mTileSize - 2
									},
									0.0f, 1, this->mTileInset / 2.0f, ::GREEN);
	}
	if (this->mQueuedSwapDirection != SwapDirection::None)
	{
		::DrawRectangleRoundedLinesEx(::Rectangle {
										this->mQueuedSwapPos.x * this->mTileSize + this->mXOffset + 1,
										this->mQueuedSwapPos.y * this->mTileSize + this->mYOffset + 1,
										this->mTileSize - 2,
										this->mTileSize - 2
									},
									0.0f, 1, this->mTileInset / 2.0f, ::VIOLET);
		// lookup table kekw
		float xDelta = 0.0f;
		float yDelta = 0.0f;
		if (this->mQueuedSwapDirection == SwapDirection::Left)
			xDelta = -1.0f;
		else if (this->mQueuedSwapDirection == SwapDirection::Right)
			xDelta = 1.0f;
		if (this->mQueuedSwapDirection == SwapDirection::Up)
			yDelta = -1.0f;
		else if (this->mQueuedSwapDirection == SwapDirection::Down)
			yDelta = 1.0f;
		::DrawRectangleRoundedLinesEx(::Rectangle {
										(this->mQueuedSwapPos.x + xDelta) * this->mTileSize + this->mXOffset + 1,
										(this->mQueuedSwapPos.y + yDelta) * this->mTileSize + this->mYOffset + 1,
										this->mTileSize - 2,
										this->mTileSize - 2
									},
									0.0f, 1, this->mTileInset / 2.0f, ::VIOLET);
	}
}

void Cyrey::Board::DrawSideUI()
{
	int fontSize = this->mApp->mHeight / 25;
	int defaultColor = ::GuiGetStyle(::GuiControl::DEFAULT, GuiControlProperty::TEXT_COLOR_NORMAL);
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, GuiTextAlignment::TEXT_ALIGN_RIGHT);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);

	::GuiLabel(
		::Rectangle {
			0, static_cast<float>(this->mApp->mHeight) / 2,
			this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize)
		},
		::TextFormat("Score: %lld", this->mStats.mScore)
	);
	::GuiLabel(
		::Rectangle {
			0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize,
			this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize)
		},
		::TextFormat("Pieces cleared: %d", this->mStats.mPiecesCleared)
	);

	if (this->mCascadeNumber >= 3)
	{
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, ColorToInt(::GREEN));
		::GuiLabel(
			::Rectangle {
				0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 3,
				this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize)
			},
			::TextFormat("Cascades: %d", this->mCascadeNumber)
		);
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, defaultColor);
	}
	if (this->mPiecesClearedInMove >= 10)
	{
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, ColorToInt(::PINK));
		::GuiLabel(
			::Rectangle {
				0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 4,
				this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize)
			},
			::TextFormat("Pieces: %d", this->mPiecesClearedInMove)
		);
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, defaultColor);
	}

	::GuiSetStyle(::GuiControl::LABEL,
	              ::GuiControlProperty::TEXT_COLOR_NORMAL,
	              this->mSecondsRemaining < 10 ? ColorToInt(::RED) : ColorToInt(::WHITE));
	::GuiSetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE, fontSize * 2);
	::GuiLabel(
		::Rectangle {
			0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 5,
			this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize)
		},
		::TextFormat("%.1f", this->mSecondsRemaining)
	);
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, defaultColor);

	int iconSize = fontSize * 2;
	int iconScale = iconSize / 16;
	::GuiSetIconScale(iconScale);

	if (this->mIsGameOver)
		::GuiDisable();
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
	::GuiEnable();

	if (::GuiButton(::Rectangle {
		                this->mXOffset - (iconSize * 2.25f) - (this->mTileSize / 2),
		                (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 7,
		                static_cast<float>(iconSize),
		                static_cast<float>(iconSize)
	                },
	                ::GuiIconText(::GuiIconName::ICON_RESTART, "")))
	{
		this->NewGame();
	}
	if (this->mIsInReplay)
	{
		::GuiSetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE, fontSize);
		::GuiSetIconScale(fontSize / 16);
		::GuiLabel(
			::Rectangle {
				0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 10,
				this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize)
			},
			::GuiIconText(::GuiIconName::ICON_PLAYER_RECORD, "Replay")
		);
	}
}

void Cyrey::Board::DrawResultsScreen()
{
	auto appWidth = static_cast<float>(this->mApp->mWidth);
	auto appHeight = static_cast<float>(this->mApp->mHeight);

	float windowY = appHeight * 0.1f;
	float windowHeight = appHeight - (windowY * 2);
	float windowX = appWidth > appHeight ? (appWidth - windowHeight) / 2 : appWidth * 0.1f;
	float windowWidth = windowY < windowX ? windowHeight : appWidth - (windowX * 2); // square if window is wide
	Vector2 windowAnchor = { windowX, windowY };
	Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };

	float fontSize = windowHeight > windowWidth ? windowWidth / 20 : windowHeight / 20;
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
	::GuiSetIconScale(std::max(fontSize / 16, 1.0f));

	float windowPaddingY = windowHeight * 0.1f;
	float controlOffsetY = fontSize * 1.3f;
	float controlPaddingX = windowWidth * 0.05f;

	Rectangle movesLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 2),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle movesValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 2),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle mpsLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 3),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle mpsValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 3),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle bombsLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 4),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle bombsValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 4),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle lightningsLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 5),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle lightningsValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 5),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle hypercubesLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 6),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle hypercubesValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 6),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle bestMoveLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 7),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle bestMoveValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 7),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle bestCascadeLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 8),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle bestCascadeValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 8),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle piecesClearedLabelPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 9),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle piecesClearedValuePos = {
		windowAnchor.x + (windowWidth / 2),
		windowAnchor.y + windowPaddingY + (controlOffsetY * 9),
		(windowWidth / 2) - controlPaddingX,
		fontSize
	};
	Rectangle viewReplayBtnPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 10.5f),
		(windowWidth / 2) - controlPaddingX * 1.5f,
		fontSize
	};
	Rectangle submitBtnPos = {
		windowAnchor.x + (windowWidth / 2) + controlPaddingX * 0.5f,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 10.5f),
		(windowWidth / 2) - controlPaddingX * 1.5f,
		fontSize
	};
	Rectangle mainMenuBtnPos = {
		windowAnchor.x + controlPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 12),
		(windowWidth / 2) - controlPaddingX * 1.5f,
		fontSize
	};
	Rectangle playAgainBtnPos = {
		windowAnchor.x + (windowWidth / 2) + controlPaddingX * 0.5f,
		windowAnchor.y + windowPaddingY + (controlOffsetY * 12),
		(windowWidth / 2) - controlPaddingX * 1.5f,
		fontSize
	};

	if (::GuiWindowBox(windowRect, "Results") ||
		::GuiButton(mainMenuBtnPos, ::GuiIconText(::GuiIconName::ICON_EXIT, "Main Menu")))
	{
		this->mApp->ChangeToState(CyreyAppState::MainMenu);
	}

	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, ::GuiTextAlignment::TEXT_ALIGN_RIGHT);
	::GuiLabel(movesLabelPos, "Moves: ");
	::GuiLabel(mpsLabelPos, "Moves per second: ");
	::GuiLabel(bombsLabelPos, "Bombs: ");
	::GuiLabel(lightningsLabelPos, "Lightnings: ");
	::GuiLabel(hypercubesLabelPos, "Hypercubes: ");
	::GuiLabel(bestMoveLabelPos, "Best move (points): ");
	::GuiLabel(bestCascadeLabelPos, "Highest cascade: ");
	::GuiLabel(piecesClearedLabelPos, "Pieces cleared: ");

	auto& stats = this->mStats;
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, ::GuiTextAlignment::TEXT_ALIGN_LEFT);
	::GuiLabel(movesValuePos, ::TextFormat(" %d", stats.mMovesMade));
	::GuiLabel(mpsValuePos,
	           ::TextFormat(" %.2f", static_cast<float>(stats.mMovesMade) / this->mGameConfig.mStartingTime));
	::GuiLabel(bombsValuePos, ::TextFormat(" %d", stats.mBombsDetonated));
	::GuiLabel(lightningsValuePos, ::TextFormat(" %d", stats.mLightningsDetonated));
	::GuiLabel(hypercubesValuePos, ::TextFormat(" %d", stats.mHypercubesDetonated));
	::GuiLabel(bestMoveValuePos, ::TextFormat(" %d", stats.mBestMovePoints));
	::GuiLabel(bestCascadeValuePos, ::TextFormat(" %d", stats.mBestMoveCascades));
	::GuiLabel(piecesClearedValuePos, ::TextFormat(" %d", stats.mPiecesCleared));

	if (this->mHasSavedReplay)
		::GuiDisable();
	if (::GuiButton(submitBtnPos, this->mHasSavedReplay ? "Replay saved" : "Save Replay") ||
		(this->mApp->mSettings->mWantReplayAutoSave && !this->mHasSavedReplay))
	{
		auto currentTime = time(nullptr);
		tm* timeDetails = localtime(&currentTime);
		if (Replay::SaveReplayToFile(*this->mReplayData,
		                             ::TextFormat("%s/cyrey_%d%02d%02d_%02d%02d%02d.cyrep",
		                                          Replay::cReplaysFolderName,
		                                          timeDetails->tm_year + 1900,
		                                          timeDetails->tm_mon + 1,
		                                          timeDetails->tm_mday,
		                                          timeDetails->tm_hour,
		                                          timeDetails->tm_min,
		                                          timeDetails->tm_sec)))
		{
			Replay::PublishReplay(*this->mReplayData, this->mApp->mCurrentUser->mName);
			this->mHasSavedReplay = true;
		}
	}
	::GuiEnable();

	if (::GuiButton(viewReplayBtnPos, "View Replay"))
		this->PlayReplay();

	if (::GuiButton(playAgainBtnPos, ::GuiIconText(::GuiIconName::ICON_RESTART, "Play Again (R)")))
		this->NewGame();

	float fontSizeTitle = windowHeight > windowWidth ? windowWidth / 12 : windowHeight / 12;
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, static_cast<int>(fontSizeTitle));
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, ::GuiTextAlignment::TEXT_ALIGN_CENTER);

	Rectangle finalScoreLabel = {
		windowAnchor.x,
		windowAnchor.y + windowPaddingY,
		windowWidth,
		fontSizeTitle
	};
	::GuiLabel(finalScoreLabel,
	           ::TextFormat("Blitz %ds: %lld pts", static_cast<int>(this->mGameConfig.mStartingTime),
	                        stats.mScore));
}
