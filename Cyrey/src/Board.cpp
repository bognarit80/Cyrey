#include "Board.hpp"
#include <map>
#include "raygui.h"

void Cyrey::Board::Init()
{
	this->mXOffset = 100;
	this->mYOffset = 100;
	this->mTileSize = 30;
	this->mTileInset = 3;
	this->mBoardAlpha = 0.25f;
	this->mZoomPct = Board::cDefaultZoomPct;
	this->mDragging = false;
	this->mTriedSwap = false;
	this->mScore = 0;
	this->mPiecesCleared = 0;
	this->mCascadeNumber = 0;
	this->mPiecesClearedInMove = 0;
	this->mBoardSwerve = raylib::Vector2{0, -(float)this->mTileSize * 8};
	this->mFallDelay = 0.0f;
	this->mMissDelay = 0.0f;
	this->mColorCount = static_cast<int>(PieceColor::Count) - 1;
	this->mBaseScore = 50;
	this->mScoreMultiplier = 1;
	this->mSecondsRemaining = 0.0f;
	this->mMatchedPieceAnims = {};
	this->mDroppedPieceAnims = {};
	this->mNewGameAnimProgress = 0.0f;
	this->mDroppedNewGamePieces = false;

	/*auto boardMock = ParseBoardString(
R"(brygyrgr
ygbpwpwg
gygwygyw
gwygwywg
gwywgygy
rwprpwrw
ryygbgbb
pprpprpr
)");*/
	this->mCurrentMatchSet = std::make_unique<MatchSet>();
	this->ResetBoard();

	this->mWidth = mBoard[0].size();
	this->mHeight = mBoard.size();
}

void Cyrey::Board::Update()
{
	this->mZoomPct += raylib::Mouse::GetWheelMove(); //perhaps change this to the Camera functionality in raylib? seems a lot more versatile
	if (raylib::Mouse::IsButtonPressed(MouseButton::MOUSE_BUTTON_MIDDLE))
		this->mZoomPct = Board::cDefaultZoomPct;

	this->UpdateDragging();
	this->UpdateInput();

	this->UpdateMatchedPieceAnims();
	this->UpdateDroppedPieceAnims();

	int screenWidth = this->mApp->mWindow->GetWidth();
	int screenHeight = this->mApp->mWindow->GetHeight();

	if (screenWidth > 0 && screenHeight > 0)
	{
		this->mTileSize = (screenHeight * this->mZoomPct / 100) / this->mHeight;
		this->mTileInset = this->mTileSize / 10;
		this->mXOffset = (screenWidth / 2) - (this->mWidth * mTileSize / 2) + this->mBoardSwerve.x;
		this->mYOffset = (screenHeight / 2) - (this->mHeight * mTileSize / 2) + this->mBoardSwerve.y;
	}

	this->UpdateBoardSwerve();

	if (this->mFallDelay > 0.0f)
	{
		this->mFallDelay -= this->mApp->GetDeltaTime();
		if (this->mFallDelay <= 0.0f)
		{
			this->mFallDelay = 0.0f;
			this->UpdateFalling();
			if (!this->UpdateMatchSets() && this->mQueuedSwapDirection != SwapDirection::None)
			{
				this->TrySwap(this->mQueuedSwapPos.x, this->mQueuedSwapPos.y, this->mQueuedSwapDirection);
				this->mQueuedSwapDirection = SwapDirection::None;
			}
			if (this->mApp->mSettings->mQueueSwapTolerance < 1.0f)
				this->mQueuedSwapDirection = SwapDirection::None;
		}
	}
	if (this->mMissDelay > 0.0f)
	{
		this->mMissDelay -= this->mApp->GetDeltaTime();
		if (this->mMissDelay <= 0.0f)
		{
			this->mMissDelay = 0.0f;
			if (this->mQueuedSwapDirection != SwapDirection::None)
			{
				this->TrySwap(this->mQueuedSwapPos.x, this->mQueuedSwapPos.y, this->mQueuedSwapDirection);
				this->mQueuedSwapDirection = SwapDirection::None;
			}
			if (this->mApp->mSettings->mQueueSwapTolerance < 1.0f)
				this->mQueuedSwapDirection = SwapDirection::None;
		}
	}

	if (!this->UpdateNewGameAnim() && ((this->mSecondsRemaining -= this->mApp->GetDeltaTime()) < 0.0f))
		this->mSecondsRemaining = 0.0f;
}

void Cyrey::Board::Draw() const
{
	this->DrawCheckerboard();
	this->DrawBorder();
	this->DrawPieces();
	this->DrawPieceMatchAnims();
	this->DrawPieceDropAnims();
	this->DrawHoverSquare();
	this->DrawScore();
}

void Cyrey::Board::UpdateInput()
{
	KeyboardKey key = (KeyboardKey)::GetKeyPressed();

	if (this->GetHoveredTile())
	{
		Vector2 hoveredTile = *this->GetHoveredTile();
		switch (key)
		{
		case KeyboardKey::KEY_W:
			this->TrySwap(hoveredTile.x, hoveredTile.y, SwapDirection::Up);
			return;
		case KeyboardKey::KEY_A:
			this->TrySwap(hoveredTile.x, hoveredTile.y, SwapDirection::Left);
			return;
		case KeyboardKey::KEY_S:
			this->TrySwap(hoveredTile.x, hoveredTile.y, SwapDirection::Down);
			return;
		case KeyboardKey::KEY_D:
			this->TrySwap(hoveredTile.x, hoveredTile.y, SwapDirection::Right);
			return;
#ifdef _DEBUG
		case KeyboardKey::KEY_F:
			this->mBoard[hoveredTile.y][hoveredTile.x].Bombify();
			return;
		case KeyboardKey::KEY_Z:
			this->mBoard[hoveredTile.y][hoveredTile.x].Lightningify();
			return;
		case KeyboardKey::KEY_C:
			this->mBoard[hoveredTile.y][hoveredTile.x].Hypercubify();
			return;
		case KeyboardKey::KEY_KP_1:
		case KeyboardKey::KEY_KP_2:
		case KeyboardKey::KEY_KP_3:
		case KeyboardKey::KEY_KP_4:
		case KeyboardKey::KEY_KP_5:
		case KeyboardKey::KEY_KP_6:
		case KeyboardKey::KEY_KP_7:
			this->mBoard[hoveredTile.y][hoveredTile.x].mColor = static_cast<PieceColor>(key - 320);
			break;
		case KeyboardKey::KEY_ONE:
		case KeyboardKey::KEY_TWO:
		case KeyboardKey::KEY_THREE:
		case KeyboardKey::KEY_FOUR:
		case KeyboardKey::KEY_FIVE:
		case KeyboardKey::KEY_SIX:
		case KeyboardKey::KEY_SEVEN:
			this->mBoard[hoveredTile.y][hoveredTile.x].mColor = static_cast<PieceColor>(key - 48);
			break;
#endif
		default:
			break;
		}
	}
	
	switch(key)
	{
	case KeyboardKey::KEY_R:
		this->ResetBoard();
		break;
	case KeyboardKey::KEY_L:
		this->mApp->mDarkMode ^= 1;
		break;
	case KeyboardKey::KEY_M:
		this->mApp->mSettings->mWantBoardSwerve ^= 1;
		break;
	case KeyboardKey::KEY_SPACE:
		if (this->mNewGameAnimProgress < Board::cNewGameAnimDuration)
		{
			this->mNewGameAnimProgress = Board::cNewGameAnimDuration;
			this->mBoardSwerve = raylib::Vector2::Zero();
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
		{'r', PieceColor::Red},
		{'g', PieceColor::Green},
		{'b', PieceColor::Blue},
		{'y', PieceColor::Yellow},
		{'o', PieceColor::Orange},
		{'w', PieceColor::White},
		{'p', PieceColor::Purple}
	};
	std::vector<std::vector<Cyrey::Piece>> board {};
	std::vector<Cyrey::Piece> row {};
	int i = 0;
	while (char c = data[i])
	{
		if (c == '\n')
		{
			board.push_back(row);
			row.clear();
		}
		else
			row.push_back(Piece(charPieceMap[c]));

		i++;
	}
	return board;
}

std::vector<std::vector<Cyrey::Piece>> Cyrey::Board::GenerateStartingBoard() const
{
	std::vector<std::vector<Cyrey::Piece>> board{};
	std::vector<Cyrey::Piece> row{};
	//TODO: SetRandomSeed
	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			row.push_back(Piece(static_cast<PieceColor>(GetRandomValue(1, this->mColorCount))));
			row[j].mBoardX = j;
			row[j].mBoardY = i;
		}
		board.push_back(row);
		row.clear();
	}
	return board;
}

void Cyrey::Board::ResetBoard()
{
	do
	{
		this->mMatchSets.clear();
		this->mBoard = this->GenerateStartingBoard();
	} while (this->FindSets()); //ugly until I make a better algorithm for creating boards with no sets
	this->mScore = 0;
	this->mPiecesCleared = 0;
	this->mSecondsRemaining = 0.0f;
	this->mBoardSwerve = raylib::Vector2{ 0, static_cast<float>(-this->mTileSize * 8) };
	this->mNewGameAnimProgress = 0.0f;
	this->mDroppedNewGamePieces = false;
}

void Cyrey::Board::AddSwerve(raylib::Vector2 swerve)
{
	if (!this->mApp->mSettings->mWantBoardSwerve)
		return;

	this->mBoardSwerve += swerve;
}

std::optional<raylib::Vector2> Cyrey::Board::GetHoveredTile() const
{
	int mouseX = raylib::Touch::GetX() - (int)this->mXOffset;
	int mouseY = raylib::Touch::GetY() - (int)this->mYOffset;

	if (mouseX <= 0 || mouseY <= 0 || this->mTileSize <= 0)
		return std::nullopt;

	int xTile = mouseX / mTileSize;
	int yTile = mouseY / mTileSize;
	if (xTile >= this->mWidth || yTile >= this->mHeight)
		return std::nullopt;

	return raylib::Vector2(xTile, yTile);
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
			if (this->mMatchSets.size() > 0)
				foundSet = true;
		}
	}

	return foundSet;
}

bool Cyrey::Board::FindSets(int pieceCol, int pieceRow, PieceColor color, bool first)
{
	if (this->mBoard[pieceRow][pieceCol].mColor == PieceColor::Uncolored)
		return false; //don't check uncolored pieces at all

	if (this->mCurrentMatchSet->mPieces.size() == 0)
	{
		if (!this->IsPieceBeingMatched(this->mBoard[pieceRow][pieceCol].mID))
			this->mCurrentMatchSet->mPieces.push_back(&this->mBoard[pieceRow][pieceCol]);
		else return false; //already checked
	}
	else
	{
		for (auto piece : this->mCurrentMatchSet->mPieces)
		{
			if (piece->mID == this->mBoard[pieceRow][pieceCol].mID)
				return false; // already checked
		}
		this->mCurrentMatchSet->mPieces.push_back(&this->mBoard[pieceRow][pieceCol]);
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
	if (this->mCurrentMatchSet->mPieces.size() >= 3)
	{
		foundSet = true;
		if (first)
		{
			this->mCurrentMatchSet->mAddedPieces.push_back(&this->mBoard[pieceRow][pieceCol]);
			this->mMatchSets.push_back(*this->mCurrentMatchSet);
		}
	}

	if (first)
	{
		this->mCurrentMatchSet->mPieces.clear();
		this->mCurrentMatchSet->mAddedPieces.clear();
	}

	return foundSet;
}

bool Cyrey::Board::IsPieceBeingMatched(unsigned int pieceID) const
{
	for (auto& matchSet : this->mMatchSets)
	{
		for (auto piece : matchSet.mPieces)
		{
			if (piece->mID == pieceID)
				return true;
		}
	}
	return false;
}

bool Cyrey::Board::TrySwap(int col, int row, SwapDirection direction)
{
	if (!this->CanSwap())
	{
		float queueSwapTolerance = this->mApp->mSettings->mQueueSwapTolerance;
		if ((queueSwapTolerance >= 1.0f ||
			(this->mFallDelay < queueSwapTolerance && 
			this->mMissDelay < queueSwapTolerance &&
			Board::cNewGameAnimDuration - this->mNewGameAnimProgress < queueSwapTolerance)) &&
			this->mSecondsRemaining > 0)
		{
			this->mQueuedSwapPos = raylib::Vector2{ (float)col, (float)row };
			this->mQueuedSwapDirection = direction;
		}
		else if (this->mNewGameAnimProgress < Board::cNewGameAnimDuration) //skip anim if user tries to swap during it
		{
			this->mNewGameAnimProgress = Board::cNewGameAnimDuration;
			this->mBoardSwerve = raylib::Vector2::Zero();
		}
		return false;
	}

	int toRow = col;
	int toCol = row;
	raylib::Vector2 swerve = raylib::Vector2::Zero();
	float swerveAmount = this->cSwerveCoeff * this->mTileSize;
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

	return this->TrySwap(col, row, toRow, toCol);
}

bool Cyrey::Board::TrySwap(int col, int row, int toCol, int toRow)
{
	if (!this->IsSwapLegal(col, row, toCol, toRow))
		return false;

	Cyrey::Piece temp = this->mBoard[row][col];
	if (temp.IsFlagSet(PieceFlag::Hypercube))
	{
		this->mCascadeNumber++;
		int piecesCleared = this->DoHypercube(this->mBoard[row][col], this->mBoard[toRow][toCol]);
		this->mPiecesCleared += piecesCleared;
		this->mScore += (piecesCleared - 2) * this->mBaseScore * this->mScoreMultiplier * this->mCascadeNumber;
		this->mPiecesClearedInMove += piecesCleared;
		this->mFallDelay = Board::cFallDelay * 2;
		this->mCascadeNumber++;
		this->mBoard[row][col] = Cyrey::gNullPiece; //temp until I make a proper sequence
		return true;
	}
	this->mBoard[row][col] = this->mBoard[toRow][toCol];
	this->mBoard[toRow][toCol] = temp;
	this->mBoard[row][col].mBoardX = col;
	this->mBoard[row][col].mBoardY = row;
	this->mBoard[toRow][toCol].mBoardX = toCol;
	this->mBoard[toRow][toCol].mBoardY = toRow;

	//check only the pieces swapped for matches, everything else should be untouched if we can only swap from a non-moving state
	bool foundSet1 = this->FindSets(toCol, toRow, this->mBoard[toRow][toCol].mColor);
	bool foundSet2 = this->FindSets(col, row, this->mBoard[row][col].mColor);
	if (!foundSet1 && !foundSet2)
		this->mMissDelay = this->cMissPenalty;

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
	{
		return 0;
	}
	else if (piece.mImmunity)
	{
		//no infinite recursions of special pieces destroying each other
		return 1;
	}
	this->mMatchedPieceAnims.push_back(PieceMatchAnim(piece.mBoardX, piece.mBoardY, piece.mColor, destroy));
	piece = Cyrey::gNullPiece;

	int piecesCleared = 1;
	if (pieceCopy.IsFlagSet(PieceFlag::Bomb))
	{
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0)
					continue; //skip the bomb itself

				if (this->IsPositionLegal(pieceCopy.mBoardX + j, pieceCopy.mBoardY + i))
				{
					piecesCleared += this->MatchPiece(this->mBoard[pieceCopy.mBoardY + i][pieceCopy.mBoardX + j], pieceCopy, true);
				}
			}
		}
		this->mFallDelay += Board::cFallDelay * 0.25;
	}
	else if (pieceCopy.IsFlagSet(PieceFlag::Lightning))
	{
		for (int i = 0; i < Board::cLightningPiecesAmount; i++)
		{
			int row, col, count = 0;
			do
			{
				count++;
				row = ::GetRandomValue(0, this->mHeight - 1);
				col = ::GetRandomValue(0, this->mWidth - 1);
				
				if (count > (this->mWidth * this->mHeight * 10)) [[unlikely]]
				{
						printf("Lightning piece activation couldn't find a vacant piece\n");
						break; //no need to set x and y to anything as calling MatchPiece on a nullPiece is fine
				}
			} while (this->mBoard[row][col].mID == 0);

			piecesCleared += this->MatchPiece(this->mBoard[row][col], pieceCopy, true);
		}
		this->mFallDelay += Board::cFallDelay;
	}
	else if (pieceCopy.IsFlagSet(PieceFlag::Hypercube))
	{
		piecesCleared += this->DoHypercube(pieceCopy, byPiece);
		this->mFallDelay += Board::cFallDelay;
	}

	return piecesCleared;
}

int Cyrey::Board::DoHypercube(Piece& cubePiece, const Piece& byPiece)
{
	//if called on a piece that isn't a hypercube for some reason, don't resolve
	if (!cubePiece.IsFlagSet(PieceFlag::Hypercube))
		return 0;

	int piecesCleared = 1;
	PieceColor targetColor = byPiece.mColor != PieceColor::Uncolored ? byPiece.mColor : cubePiece.mOldColor;
	bool wantDHR = byPiece.IsFlagSet(PieceFlag::Hypercube);

	for ( auto &row : this->mBoard )
	{
		for (auto &piece : row )
		{
			if (piece.mColor == targetColor || wantDHR)
			{
				piecesCleared += this->MatchPiece(piece, cubePiece, true);
			}
		}
	}

	return piecesCleared;
}

void Cyrey::Board::UpdateMatchedPieceAnims()
{
	for (auto& anim : this->mMatchedPieceAnims)
	{
		anim.mOpacity -= PieceMatchAnim::cStartingOpacity * (this->mApp->GetDeltaTime() / Board::cFallDelay);
		if (anim.mOpacity <= 0.0f)
		{
			this->mMatchedPieceAnims.clear();
		}
	}
}

void Cyrey::Board::UpdateDroppedPieceAnims()
{
	for (auto& anim : this->mDroppedPieceAnims)
	{
		anim.mOpacity -= PieceDropAnim::cStartingOpacity * (this->mApp->GetDeltaTime() / Board::cFallDelay);
		if (anim.mOpacity <= 0.0f)
		{
			this->mDroppedPieceAnims.clear();
		}
	}
}

void Cyrey::Board::UpdateBoardSwerve()
{
	float fallDelay = 1 - std::pow(this->cFallDelay * 0.003f, this->mApp->GetDeltaTime());
	this->mBoardSwerve.x = ::Lerp(this->mBoardSwerve.x, 0, fallDelay);
	this->mBoardSwerve.y = ::Lerp(this->mBoardSwerve.y, 0, fallDelay);
}

bool Cyrey::Board::UpdateNewGameAnim()
{
	if (this->mNewGameAnimProgress > Board::cNewGameAnimDuration)
		return false;

	this->mSecondsRemaining = (this->mNewGameAnimProgress / Board::cNewGameAnimDuration) * Board::cStartingTime;
	this->mNewGameAnimProgress += this->mApp->GetDeltaTime();

	if (this->mNewGameAnimProgress >= (Board::cNewGameAnimDuration * 0.75f) && !this->mDroppedNewGamePieces)
	{
		for (int i = 0; i < this->mWidth; i++)
			this->mDroppedPieceAnims.emplace_back(i);

		this->mDroppedNewGamePieces = true;
		this->AddSwerve(raylib::Vector2{ 0, this->mTileSize * Board::cSwerveCoeff * 3 });
	}

	if (this->mNewGameAnimProgress >= Board::cNewGameAnimDuration)
	{
		this->mNewGameAnimProgress = Board::cNewGameAnimDuration + 1;
		this->mSecondsRemaining = Board::cStartingTime; //failsafe
	}

	if (this->mSecondsRemaining > Board::cStartingTime)
		this->mSecondsRemaining = Board::cStartingTime; //failsafe

	return true;
}

void Cyrey::Board::UpdateDragging()
{
	raylib::Vector2 mousePos = raylib::Mouse::GetPosition();
	if (raylib::Mouse::IsButtonDown(MouseButton::MOUSE_BUTTON_LEFT))
	{
		if (!this->mDragging 
			//&& raylib::Mouse::GetDelta() != raylib::Vector2::Zero()
			&& this->IsMouseInBoard()
			&& !this->mTriedSwap)
		{
			this->mDragging = true;
			this->mDragMouseBegin = mousePos;
			this->mDragTileBegin = *this->GetHoveredTile(); //we're guaranteed to have a value here
		}
		else if (this->mDragging && this->mFallDelay <= 0.0f)
		{
			//this->mBoard[this->mDragTileBegin.row][this->mDragTileBegin.col].mDragging = true;

			float xDiff = this->mDragMouseBegin.x - mousePos.x;
			float yDiff = this->mDragMouseBegin.y - mousePos.y;

			float xTileBegin = this->mDragTileBegin.x;
			float yTileBegin = this->mDragTileBegin.y;

			if (abs(xDiff) > (this->mTileSize * this->mApp->mSettings->mSwapDeadZone))
			{
				this->TrySwap(xTileBegin, yTileBegin, (xDiff > 0 ? SwapDirection::Left : SwapDirection::Right));
				this->mDragging = false;
				this->mTriedSwap = true;
			}
			else if (abs(yDiff) > (this->mTileSize * this->mApp->mSettings->mSwapDeadZone))
			{
				this->TrySwap(xTileBegin, yTileBegin, (yDiff > 0 ? SwapDirection::Up : SwapDirection::Down));
				this->mDragging = false;
				this->mTriedSwap = true;
			}
		}
	}
	else if (raylib::Mouse::IsButtonReleased(MouseButton::MOUSE_BUTTON_LEFT))
	{
		//this->mBoard[this->mDragTileBegin.row][this->mDragTileBegin.col].mDragging = false;
		this->mDragging = false;
		this->mDragMouseBegin = raylib::Vector2::Zero();
		this->mDragTileBegin = raylib::Vector2::Zero();
		this->mTriedSwap = false;
	}
}

int Cyrey::Board::UpdateMatchSets()
{
	int matchSets = this->mMatchSets.size();
	if (matchSets > 0)
	{
		this->mFallDelay = this->cFallDelay;
		this->AddSwerve({ 0, this->cSwerveCoeff * std::min(this->mCascadeNumber, cMaxCascadesSwerve) * this->mTileSize * 0.75f });
		this->mCascadeNumber++;
	}
	for ( auto& matchSet : this->mMatchSets )
	{
		int piecesPerSet = matchSet.mPieces.size();
		int addedPiecesPerSet = matchSet.mAddedPieces.size();
		//make the special at the added piece, or in the middle of the set if no pieces were added
		Piece* addedPiece = addedPiecesPerSet > 0 ? 
			matchSet.mAddedPieces[addedPiecesPerSet / 2] : 
			matchSet.mPieces[piecesPerSet / 2];
		int piecesCleared = 0;

		for ( auto piece : matchSet.mPieces )
		{
			if (piece == addedPiece && piecesPerSet == 4)
			{
				addedPiece->Bombify();
			}
			else if (piece == addedPiece && piecesPerSet == 5)
			{
				addedPiece->Lightningify();
			}
			else if (piece == addedPiece && piecesPerSet >= 6)
			{
				addedPiece->Hypercubify();
			}
			
			piecesCleared += this->MatchPiece(*piece);
		}
		this->mPiecesCleared += piecesCleared;
		this->mScore += (piecesCleared - 2) * this->mBaseScore * this->mScoreMultiplier * this->mCascadeNumber;
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
	this->FillInBlanks();
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
				piece = Piece((static_cast<PieceColor>(GetRandomValue(1, this->mColorCount))));
				piece.mBoardX = j;
				piece.mBoardY = i;
				this->mDroppedPieceAnims.emplace_back(j);
			}
		}
	}
	if (!this->FindSets())
	{
		this->mCascadeNumber = 0;
		this->mPiecesClearedInMove = 0;
	}
}

void Cyrey::Board::DrawCheckerboard() const
{
	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			raylib::Rectangle(
				(float)((j * this->mTileSize) + this->mXOffset),
				(float)((i * this->mTileSize) + this->mYOffset),
				(float)this->mTileSize,
				(float)this->mTileSize
			).Draw((i % 2) ^ (j % 2) ?						//alternate between light and dark every row
				raylib::Color::LightGray().Alpha(this->mBoardAlpha) : 
				raylib::Color::Gray().Alpha(this->mBoardAlpha));
		}
	}
}

void Cyrey::Board::DrawBorder() const
{
	//draw the outline
	raylib::Color outlineColor = this->mApp->mDarkMode ? raylib::Color::Gray() : raylib::Color::DarkGray();
	if (this->mMissDelay > 0.0f || this->mSecondsRemaining <= 0.0f)
		outlineColor = raylib::Color::Red();

	raylib::Rectangle(
		(float)(this->mXOffset - 1),
		(float)(this->mYOffset - 1),
		(float)((this->mTileSize * this->mWidth) + 2),
		(float)((this->mTileSize * this->mHeight) + 2)
	).DrawRoundedLines(0.0f, 1, this->mTileInset, outlineColor);

	//draw the timer - way more complicated than I thought it would be. definitely review and simplify later.
	float thick = this->mTileInset * 2;
	float offset = thick / 2; //Rectangle draws thickness outwards, Line draws it in the middle, shift it to fit somewhat
	raylib::Color timerColor = this->mSecondsRemaining < 10 ? raylib::Color::Red() : raylib::Color::Green();

	int halfCircumference = ((this->mWidth * this->mTileSize) + (this->mHeight * this->mTileSize) + thick * 2 + offset);
	float fillPct = this->mSecondsRemaining / Board::cStartingTime;
	if (this->mMissDelay > 0.0f)
	{
		fillPct = 1;
		timerColor = raylib::Color::Red();
	}

	float fillLength = static_cast<float>(halfCircumference) * fillPct;
	float firstCurveLen = (static_cast<float>(this->mWidth * this->mTileSize) / 2) + thick;
	float secondCurveLen = firstCurveLen + static_cast<float>(this->mHeight * this->mTileSize) + thick;

	raylib::Vector2 startPos{ this->mXOffset + (static_cast<float>(this->mWidth) / 2 * this->mTileSize) , this->mYOffset + (this->mHeight * this->mTileSize) + offset};
	raylib::Vector2 firstCurvePointLeft{ this->mXOffset - offset, this->mYOffset + (this->mHeight * this->mTileSize) + offset };
	raylib::Vector2 firstCurvePointRight{ this->mXOffset + (this->mWidth * this->mTileSize) + offset, this->mYOffset + (this->mHeight * this->mTileSize) + offset };
	raylib::Vector2 secondCurvePointLeft{ this->mXOffset - offset, this->mYOffset - offset };
	raylib::Vector2 secondCurvePointRight{ this->mXOffset + (this->mWidth * this->mTileSize) + offset, this->mYOffset - offset };

	bool wantFirstCurve = fillLength >= firstCurveLen;
	bool wantSecondCurve = fillLength >= secondCurveLen;

	float leftX = wantSecondCurve ?
		secondCurvePointLeft.x + fillLength - secondCurveLen :
		wantFirstCurve ? firstCurvePointLeft.x :
		startPos.x - fillLength;
	float leftY = wantSecondCurve ?
		secondCurvePointLeft.y :
		wantFirstCurve ? firstCurvePointLeft.y - (fillLength - firstCurveLen) :
		firstCurvePointLeft.y;
	float rightX = wantSecondCurve ?
		secondCurvePointRight.x - (fillLength - secondCurveLen) :
		wantFirstCurve ? firstCurvePointRight.x :
		startPos.x + fillLength;
	float rightY = wantSecondCurve ?
		secondCurvePointRight.y :
		wantFirstCurve ? firstCurvePointRight.y - (fillLength - firstCurveLen) :
		firstCurvePointRight.y;
	raylib::Vector2 lastLeft{ leftX, leftY };
	raylib::Vector2 lastRight{ rightX, rightY };

	int edgeCircleSegments = 6;
	if (wantSecondCurve)
	{
		//left
		::DrawLineEx(startPos, firstCurvePointLeft, thick, timerColor);
		::DrawCircleSector(firstCurvePointLeft, offset, 270, 360, edgeCircleSegments, timerColor);
		::DrawLineEx(firstCurvePointLeft, secondCurvePointLeft, thick, timerColor);
		::DrawCircleSector(secondCurvePointLeft, offset, 180, 270, edgeCircleSegments, timerColor);
		::DrawLineEx(secondCurvePointLeft, lastLeft, thick, timerColor);

		//right
		::DrawLineEx(startPos, firstCurvePointRight, thick, timerColor);
		::DrawCircleSector(firstCurvePointRight, offset, 90, 0, edgeCircleSegments, timerColor);
		::DrawLineEx(firstCurvePointRight, secondCurvePointRight, thick, timerColor);
		::DrawCircleSector(secondCurvePointRight, offset, 180, 90, edgeCircleSegments, timerColor);
		::DrawLineEx(secondCurvePointRight, lastRight, thick, timerColor);
	}
	else if (wantFirstCurve)
	{
		//left
		::DrawLineEx(startPos, firstCurvePointLeft, thick, timerColor);
		::DrawCircleSector(firstCurvePointLeft, offset, 270, 360, edgeCircleSegments, timerColor);
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

void Cyrey::Board::DrawPieces() const
{
	//don't draw the pieces on gameover
	if (this->mSecondsRemaining <= 0.0f && this->mFallDelay <= 0.0f || !this->mDroppedNewGamePieces)
		return;

	for (int i = 0; i < this->mHeight; i++)
	{
		for (int j = 0; j < this->mWidth; j++)
		{
			Color color = raylib::Color::Blank();
			int sides = 3;
			float rotation = 0;
			float radius = (this->mTileSize / 2) - this->mTileInset;
			raylib::Vector2 center { (float)((j * this->mTileSize) + this->mXOffset + (float)this->mTileSize / 2),
									(float)((i * this->mTileSize) + this->mYOffset + (float)this->mTileSize / 2)
			};

			switch (this->mBoard[i][j].mColor)
			{
			case PieceColor::Red:
				color = raylib::Color::Red(); 
				sides = 4; 
				rotation = 45.0f; 
				radius += this->mTileInset; break;
			case PieceColor::Green:
				color = raylib::Color::Green();
				sides = 8; break;
			case PieceColor::Blue:
				color = raylib::Color::Blue(); 
				sides = 3; 
				center.y -= this->mTileSize / 10.0f; 
				radius += this->mTileInset; break;
			case PieceColor::Yellow:
				color = raylib::Color::Yellow();
				sides = 4;
				break;
			case PieceColor::Orange:
				color = raylib::Color::Orange();
				sides = 6; 
				rotation = 90.0f; break;
			case PieceColor::White:
				color = this->mApp->mDarkMode ? raylib::Color::White() : raylib::Color::DarkGray();
				sides = 12; break;
			case PieceColor::Purple:
				color = raylib::Color::Purple(); 
				sides = 3; 
				rotation = 180.0f; 
				center.y += this->mTileSize / 10.0f; 
				radius += this->mTileInset; break;
			}

			::DrawPoly(center, sides, radius, rotation, color);
			if (this->mBoard[i][j].IsFlagSet(PieceFlag::Bomb))
				::DrawPolyLinesEx(center, sides, radius + (float)this->mTileInset / 2, rotation, (float)this->mTileSize / 10, raylib::Color::Orange());
			if (this->mBoard[i][j].IsFlagSet(PieceFlag::Lightning))
				::DrawPolyLinesEx(center, sides, radius + (float)this->mTileInset / 2, rotation, (float)this->mTileSize / 10, raylib::Color::SkyBlue());
			if (this->mBoard[i][j].IsFlagSet(PieceFlag::Hypercube))
			{
				::DrawCircleV(center, radius, raylib::Color::Pink());
				::DrawPolyLinesEx(center, 4, radius, rotation, this->mTileSize / 5, raylib::Color::Blue());
			}

			/*raylib::Rectangle(
				(float)((i * this->mTileSize) + this->mXOffset + this->mTileInset) + (this->mBoard[i][j].mDragging ? this->mBoard[i][j].mXDiff : 0),
				(float)((j * this->mTileSize) + this->mYOffset + this->mTileInset) + (this->mBoard[i][j].mDragging ? this->mBoard[i][j].mYDiff : 0),
				(float)this->mTileSize - (this->mTileInset * 2),
				(float)this->mTileSize - (this->mTileInset * 2)
			).Draw(color);*/
		}
	}
}

void Cyrey::Board::DrawPieceMatchAnims() const
{
	for (auto &anim : this->mMatchedPieceAnims)
	{
		raylib::Color color = raylib::Color::Blank();
		int sides = 3;
		float rotation = 0;
		float radius = (this->mTileSize / 2) - this->mTileInset;
		raylib::Vector2 center{ (float)((anim.mBoardX * this->mTileSize) + this->mXOffset + (float)this->mTileSize / 2),
								(float)((anim.mBoardY * this->mTileSize) + this->mYOffset + (float)this->mTileSize / 2)
		};

		switch (anim.mColor)
		{
		case PieceColor::Red:
			color = raylib::Color::Red();
			sides = 4;
			rotation = 45.0f;
			radius += this->mTileInset; break;
		case PieceColor::Green:
			color = raylib::Color::Green();
			sides = 8; break;
		case PieceColor::Blue:
			color = raylib::Color::Blue();
			sides = 3;
			center.y -= this->mTileSize / 10.0f;
			radius += this->mTileInset; break;
		case PieceColor::Yellow:
			color = raylib::Color::Yellow();
			sides = 4;
			break;
		case PieceColor::Orange:
			color = raylib::Color::Orange();
			sides = 6;
			rotation = 90.0f; break;
		case PieceColor::White:
			color = this->mApp->mDarkMode ? raylib::Color::White() : raylib::Color::DarkGray();
			sides = 12; break;
		case PieceColor::Purple:
			color = raylib::Color::Purple();
			sides = 3;
			rotation = 180.0f;
			center.y += this->mTileSize / 10.0f;
			radius += this->mTileInset; break;
		}
		color = color.Alpha(anim.mOpacity);

		::DrawPoly(center, sides, radius, rotation, color);
	}
}

void Cyrey::Board::DrawPieceDropAnims() const
{
	for (auto &anim : this->mDroppedPieceAnims)
	{
		float x = this->mXOffset + (anim.mBoardCol * this->mTileSize) + this->mTileInset;
		float y = this->mYOffset - (this->mTileSize * 3);
		raylib::Color from = this->mApp->mDarkMode ? raylib::Color::RayWhite().Alpha(anim.mOpacity) : raylib::Color::Black().Alpha(anim.mOpacity);
		raylib::Color to = this->mApp->mDarkMode ? raylib::Color::Black().Alpha(anim.mOpacity) : raylib::Color::RayWhite().Alpha(anim.mOpacity);

		raylib::Rectangle(x, y, this->mTileSize - (this->mTileInset * 2), (float)this->mTileSize * 2.75f).DrawGradientV(from, to);
	}
}

void Cyrey::Board::DrawHoverSquare() const
{
	std::optional<raylib::Vector2> hoveredTile = this->GetHoveredTile();
	if (!hoveredTile)
		return;

	raylib::Color rectColor = raylib::Color::Orange();
	if (this->mDragging)
		rectColor = raylib::Color::Green();
	if (!this->CanSwap())
		rectColor = raylib::Color::Red();

	raylib::Rectangle(
		(float)(((*hoveredTile).x * mTileSize) + this->mXOffset + 1),
		(float)(((*hoveredTile).y * mTileSize) + this->mYOffset + 1),
		(float)(this->mTileSize - 2),
		(float)(this->mTileSize - 2)
	).DrawRoundedLines(0.0f, 1, this->mTileInset / 2.0f, rectColor);
}

void Cyrey::Board::DrawScore() const
{
	int fontSize = this->mApp->mHeight / 25;
	int defaultColor = ::GuiGetStyle(::GuiControl::DEFAULT, GuiControlProperty::TEXT_COLOR_NORMAL);
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, GuiTextAlignment::TEXT_ALIGN_RIGHT);
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);

	::GuiLabel(
		raylib::Rectangle{ 0, static_cast<float>(this->mApp->mHeight) / 2, 
			this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize) },
		::TextFormat("Score: %lld", this->mScore)
	);
	::GuiLabel(
		raylib::Rectangle{ 0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize,
			this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize) },
		::TextFormat("Pieces cleared: %d", this->mPiecesCleared)
	);

	if (this->mCascadeNumber >= 3)
	{
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, raylib::Color::Green());
		::GuiLabel(
			raylib::Rectangle{ 0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 3,
				this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize) },
			::TextFormat("Cascades: %d", this->mCascadeNumber)
		);
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, defaultColor);
	}
	if (this->mPiecesClearedInMove >= 10)
	{
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, raylib::Color::Pink());
		::GuiLabel(
			raylib::Rectangle{ 0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 4,
				this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize) },
			::TextFormat("Pieces: %d", this->mPiecesClearedInMove)
		);
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, defaultColor);
	}

	::GuiSetStyle(::GuiControl::LABEL, 
		::GuiControlProperty::TEXT_COLOR_NORMAL, 
		this->mSecondsRemaining < 10 ? raylib::Color::Red() : raylib::Color::White());
	::GuiSetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE, fontSize * 2);
	::GuiLabel(
		raylib::Rectangle{ 0, (static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 5,
			this->mXOffset - (this->mTileSize / 2), static_cast<float>(fontSize) },
		::TextFormat("%.1f", this->mSecondsRemaining)
	);
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_COLOR_NORMAL, defaultColor);

	if (::GuiButton(Rectangle{ this->mXOffset - (::MeasureText("Menu", fontSize * 2) + (this->mTileSize / 2)),
		(static_cast<float>(this->mApp->mHeight) / 2) + fontSize * 7,
		static_cast<float>(::MeasureText("Menu", fontSize * 2)), 
		static_cast<float>(fontSize * 2)}, 
		"Menu"))
	{
		this->mApp->ChangeToState(CyreyAppState::SettingsMenu);
	}

	if (this->mSecondsRemaining <= 0.0f && this->mFallDelay <= 0.0f && this->mDroppedNewGamePieces)
	{
		::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, this->mTileSize - (this->mTileSize / 2));
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, GuiTextAlignment::TEXT_ALIGN_CENTER);
		::GuiLabel(
			raylib::Rectangle{ this->mXOffset, 
				this->mYOffset + this->mTileSize / 2,
				static_cast<float>(this->mWidth * this->mTileSize), 
				static_cast<float>(this->mHeight * this->mTileSize) },
			"Press R to restart the game!"
		);
	}
}
