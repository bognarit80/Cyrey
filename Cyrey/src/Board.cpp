#include "Board.hpp"
#include <map>

void Cyrey::Board::Init()
{
	this->mXOffset = 100;
	this->mYOffset = 100;
	this->mTileSize = 30;
	this->mTileInset = 3;
	this->mBoardAlpha = 0.25f;
	this->mZoomPct = 70;
	this->mDragging = false;
	this->mTriedSwap = false;
	this->mScore = 0;
	this->mPiecesCleared = 0;
	this->mCascadeNumber = 0;
	this->mPiecesClearedInMove = 0;
	this->mBoardSwerve = 0;
	this->mUpdateCnt = 0;
	this->mFallDelay = 0.0f;
	this->mMissDelay = 0.0f;
	this->mWantBoardSwerve = true;
	this->mColorCount = static_cast<int>(PieceColor::Count) - 1;
	this->mBaseScore = 50;
	this->mScoreMultiplier = 1;
	this->mSwapDeadZone = 0.33f;
	this->mSecondsRemaining = Board::cStartingTime;
	this->mMatchedPieceAnims = {};
	this->mDroppedPieceAnims = {};

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
	this->UpdateDragging();
	this->UpdateInput();

	int screenWidth = this->mApp->mWindow->GetWidth();
	int screenHeight = this->mApp->mWindow->GetHeight();

	if (screenWidth > 0 && screenHeight > 0)
	{
		this->mTileSize = (screenHeight * this->mZoomPct / 100) / this->mHeight;
		this->mTileInset = this->mTileSize / 10;
		this->mXOffset = (screenWidth / 2) - (this->mWidth * mTileSize / 2) + this->mBoardSwerve.x;
		this->mYOffset = (screenHeight / 2) - (this->mWidth * mTileSize / 2) + this->mBoardSwerve.y;
	}

	float fallDelay = 1 - std::pow(this->cFallDelay * 0.003f, this->mApp->GetDeltaTime());
	this->mBoardSwerve.x = ::Lerp(this->mBoardSwerve.x, 0, fallDelay);
	this->mBoardSwerve.y = ::Lerp(this->mBoardSwerve.y, 0, fallDelay);

	if (this->mFallDelay > 0.0f)
	{
		this->mFallDelay -= this->mApp->GetDeltaTime();
		if (this->mFallDelay <= 0.0f)
		{
			this->mFallDelay = 0.0f;
			this->UpdateFalling();
			this->UpdateMatchSets();
		}
	}
	if (this->mMissDelay > 0.0f)
	{
		this->mMissDelay -= this->mApp->GetDeltaTime();
		if (this->mMissDelay <= 0.0f)
		{
			this->mMissDelay = 0.0f;
		}
	}

	if ((this->mSecondsRemaining -= this->mApp->GetDeltaTime()) < 0.0f)
		this->mSecondsRemaining = 0.0f;

	for (auto &anim : this->mMatchedPieceAnims)
	{
		anim.mOpacity -= PieceMatchAnim::cStartingOpacity * (this->mApp->GetDeltaTime() / Board::cFallDelay);
		if (anim.mOpacity <= 0.0f)
		{
			this->mMatchedPieceAnims.clear();
		}
	}

	for (auto &anim : this->mDroppedPieceAnims)
	{
		anim.mOpacity -= PieceDropAnim::cStartingOpacity * (this->mApp->GetDeltaTime() / Board::cFallDelay);
		if (anim.mOpacity <= 0.0f)
		{
			this->mDroppedPieceAnims.clear();
		}
	}

	this->mUpdateCnt++;
}

void Cyrey::Board::Draw() const
{
	this->DrawCheckerboard();
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
			this->mBoard[hoveredTile.x][hoveredTile.y].Bombify();
			return;
		case KeyboardKey::KEY_Z:
			this->mBoard[hoveredTile.x][hoveredTile.y].Lightningify();
			return;
		case KeyboardKey::KEY_C:
			this->mBoard[hoveredTile.x][hoveredTile.y].Hypercubify();
			return;
		case KeyboardKey::KEY_KP_1:
		case KeyboardKey::KEY_KP_2:
		case KeyboardKey::KEY_KP_3:
		case KeyboardKey::KEY_KP_4:
		case KeyboardKey::KEY_KP_5:
		case KeyboardKey::KEY_KP_6:
		case KeyboardKey::KEY_KP_7:
			this->mBoard[hoveredTile.x][hoveredTile.y].mColor = static_cast<PieceColor>(key - 320);
			break;
		case KeyboardKey::KEY_ONE:
		case KeyboardKey::KEY_TWO:
		case KeyboardKey::KEY_THREE:
		case KeyboardKey::KEY_FOUR:
		case KeyboardKey::KEY_FIVE:
		case KeyboardKey::KEY_SIX:
		case KeyboardKey::KEY_SEVEN:
			this->mBoard[hoveredTile.x][hoveredTile.y].mColor = static_cast<PieceColor>(key - 48);
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
		this->mWantBoardSwerve ^= 1;
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
			row[j].mBoardX = i;
			row[j].mBoardY = j;
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
	this->mSecondsRemaining = Board::cStartingTime;
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
			this->FindSets(i, j, this->mBoard[i][j].mColor);
			this->mBoard[i][j].mImmunity = false; //temp until better measures are implemented
			if (this->mMatchSets.size() > 0)
				foundSet = true;
		}
	}

	return foundSet;
}

bool Cyrey::Board::FindSets(int pieceRow, int pieceCol, PieceColor color, bool first)
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
		(pieceRow - 1) < this->mWidth &&
		this->mBoard[pieceRow - 1][pieceCol].mColor == color)
	{
		this->FindSets(pieceRow - 1, pieceCol, color, false);
	}

	if ((pieceRow + 1) >= 0 &&
		(pieceRow + 1) < this->mWidth &&
		this->mBoard[pieceRow + 1][pieceCol].mColor == color)
	{
		this->FindSets(pieceRow + 1, pieceCol, color, false);
	}

	if ((pieceCol - 1) >= 0 &&
		(pieceCol - 1) < this->mHeight &&
		this->mBoard[pieceRow][pieceCol - 1].mColor == color)
	{
		this->FindSets(pieceRow, pieceCol - 1, color, false);
	}

	if ((pieceCol + 1) >= 0 &&
		(pieceCol + 1) < this->mHeight &&
		this->mBoard[pieceRow][pieceCol + 1].mColor == color)
	{
		this->FindSets(pieceRow, pieceCol + 1, color, false);
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

bool Cyrey::Board::TrySwap(int row, int col, SwapDirection direction)
{
	if (!this->CanSwap())
		return false;

	int toRow = row;
	int toCol = col;
	raylib::Vector2 swerve = { 0,0 };
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
	if (this->mWantBoardSwerve)
		this->mBoardSwerve += swerve;

	return this->TrySwap(row, col, toRow, toCol);
}

bool Cyrey::Board::TrySwap(int row, int col, int toRow, int toCol)
{
	if (!this->IsSwapLegal(row, col, toRow, toCol))
		return false;

	Cyrey::Piece temp = this->mBoard[row][col];
	if (temp.IsFlagSet(PieceFlag::Hypercube))
	{
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
	this->mBoard[row][col].mBoardX = row;
	this->mBoard[row][col].mBoardY = col;
	this->mBoard[toRow][toCol].mBoardX = toRow;
	this->mBoard[toRow][toCol].mBoardY = toCol;

	//check only the pieces swapped for matches, everything else should be untouched if we can only swap from a non-moving state
	bool foundSet1 = this->FindSets(toRow, toCol, this->mBoard[toRow][toCol].mColor);
	bool foundSet2 = this->FindSets(row, col, this->mBoard[row][col].mColor);
	if (!foundSet1 && !foundSet2)
		this->mMissDelay = this->cMissPenalty;

	this->UpdateMatchSets();

	return true;
}

bool Cyrey::Board::IsSwapLegal(int row, int col, int toRow, int toCol) const
{
	if (!this->IsPositionLegal(toRow, toCol))
		return false; //out of bounds
	
	int xDiff = std::abs(row - toRow);
	int yDiff = std::abs(col - toCol);

	//swap with only adjacent squares. no diagonal swaps. swap with itself is also considered illegal
	if (xDiff > 1 || yDiff > 1 || !(xDiff ^ yDiff))
		return false;

	return true;
}

bool Cyrey::Board::CanSwap() const
{
	return this->mFallDelay <= 0.0f && this->mMissDelay <= 0.0f && this->mSecondsRemaining > 0.0f;
}

constexpr bool Cyrey::Board::IsPositionLegal(int row, int col) const
{
	return !(row < 0 || col < 0 || row >= this->mWidth || col >= this->mHeight);
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

				if (this->IsPositionLegal(pieceCopy.mBoardX + i, pieceCopy.mBoardY + j))
				{
					piecesCleared += this->MatchPiece(this->mBoard[pieceCopy.mBoardX + i][pieceCopy.mBoardY + j], pieceCopy, true);
				}
			}
		}
		this->mFallDelay += Board::cFallDelay * 0.25;
	}
	else if (pieceCopy.IsFlagSet(PieceFlag::Lightning))
	{
		for (int i = 0; i < Board::cLightningPiecesAmount; i++)
		{
			int x, y, count = 0;
			do
			{
				count++;
				x = ::GetRandomValue(0, this->mWidth - 1);
				y = ::GetRandomValue(0, this->mHeight - 1);
				
				if (count > (this->mWidth * this->mHeight * 10)) [[unlikely]]
				{
						printf("Lightning piece activation couldn't find a vacant piece\n");
						break; //no need to set x and y to anything as calling MatchPiece on a nullPiece is fine
				}
			} while (this->mBoard[x][y].mID == 0);

			piecesCleared += this->MatchPiece(this->mBoard[x][y], pieceCopy, true);
		}
		this->mFallDelay += Board::cFallDelay;
	}
	else if (pieceCopy.IsFlagSet(PieceFlag::Hypercube))
	{
		this->DoHypercube(pieceCopy, byPiece);
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

			if (abs(xDiff) > (this->mTileSize * this->mSwapDeadZone))
			{
				this->TrySwap(xTileBegin, yTileBegin, (xDiff > 0 ? SwapDirection::Left : SwapDirection::Right));
				this->mDragging = false;
				this->mTriedSwap = true;
			}
			else if (abs(yDiff) > (this->mTileSize * this->mSwapDeadZone))
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

void Cyrey::Board::UpdateMatchSets()
{
	if (this->mMatchSets.size() > 0)
	{
		this->mFallDelay = this->cFallDelay;
		if (this->mWantBoardSwerve)
			this->mBoardSwerve.y += this->cSwerveCoeff * std::min(this->mCascadeNumber, cMaxCascadesSwerve) * this->mTileSize * 0.75f;
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
}

void Cyrey::Board::UpdateFalling()
{
	for (int i = this->mHeight - 1; i >= 0; i--)
	{
		for (int j = this->mWidth - 1; j >= 0; j--)
		{
			int k = 1;
			while (this->IsPositionLegal(i, j + k) && this->mBoard[i][j + k].mID == 0)
			{
				this->mBoard[i][j + k] = this->mBoard[i][j + k - 1];
				this->mBoard[i][j + k].mBoardY = j + k;
				this->mBoard[i][j + k - 1] = Cyrey::gNullPiece;
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
				piece.mBoardX = i;
				piece.mBoardY = j;
				this->mDroppedPieceAnims.emplace_back(i);
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

	//draw the outline
	raylib::Color outlineColor = this->mApp->mDarkMode ? raylib::Color::Gray() : raylib::Color::DarkGray();
	if (this->mMissDelay > 0.0 || this->mSecondsRemaining <= 0.0f)
		outlineColor = raylib::Color::Red();

	raylib::Rectangle(
		(float)(this->mXOffset - 1),
		(float)(this->mYOffset - 1),
		(float)((this->mTileSize * this->mWidth) + 2),
		(float)((this->mTileSize * this->mHeight) + 2)
	).DrawRoundedLines(0.0f, 1, this->mTileInset, outlineColor);
}

void Cyrey::Board::DrawPieces() const
{
	//don't draw the pieces on gameover
	if (this->mSecondsRemaining <= 0.0f && this->mFallDelay <= 0.0f)
		return;

	for (int i = 0; i < this->mBoard.size(); i++)
	{
		for (int j = 0; j < this->mBoard[0].size(); j++)
		{
			Color color = raylib::Color::Blank();
			int sides = 3;
			float rotation = 0;
			float radius = (this->mTileSize / 2) - this->mTileInset;
			raylib::Vector2 center { (float)((i * this->mTileSize) + this->mXOffset + (float)this->mTileSize / 2),
									(float)((j * this->mTileSize) + this->mYOffset + (float)this->mTileSize / 2)
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
	raylib::DrawText(std::to_string(this->mUpdateCnt), 0, 100, 16, raylib::Color::White());
	std::string score = "Score: " + std::to_string(this->mScore);
	std::string pieces = "Pieces cleared: " + std::to_string(this->mPiecesCleared);
	std::string cascades = "Cascades: " + std::to_string(this->mCascadeNumber);
	std::string piecesInMove = "Pieces: " + std::to_string(this->mPiecesClearedInMove);
	std::string timeRemaining = ::TextFormat("%.2f", this->mSecondsRemaining);
	int fontSize = this->mApp->mWindow->GetHeight() / 30;
	int fontWidthScore = raylib::MeasureText(score, fontSize);
	int fontWidthPieces = raylib::MeasureText(pieces, fontSize);
	int fontWidthCascades = raylib::MeasureText(cascades, fontSize);
	int fontWidthPiecesInMove = raylib::MeasureText(piecesInMove, fontSize);
	int fontWidthTimeRemaining = raylib::MeasureText(timeRemaining, fontSize * 2);
	raylib::Color color = this->mApp->mDarkMode ? raylib::Color::White() : raylib::Color::Black();

	raylib::DrawText(score, 
		this->mXOffset - (this->mTileSize / 2) - fontWidthScore,
		this->mApp->mWindow->GetHeight() / 2,
		fontSize, 
		color);
	raylib::DrawText(pieces,
		this->mXOffset - (this->mTileSize / 2) - fontWidthPieces,
		(this->mApp->mWindow->GetHeight() / 2) + fontSize,
		fontSize,
		color);
	if (this->mCascadeNumber >= 3)
	{
		raylib::DrawText(cascades,
			this->mXOffset - (this->mTileSize / 2) - fontWidthCascades,
			(this->mApp->mWindow->GetHeight() / 2) + fontSize * 3,
			fontSize,
			this->mApp->mDarkMode ? raylib::Color::Green() : raylib::Color::DarkGreen());
	}
	if (this->mPiecesClearedInMove >= 10)
	{
		raylib::DrawText(piecesInMove,
			this->mXOffset - (this->mTileSize / 2) - fontWidthPiecesInMove,
			(this->mApp->mWindow->GetHeight() / 2) + fontSize * 4,
			fontSize,
			this->mApp->mDarkMode ? raylib::Color::SkyBlue() : raylib::Color::DarkBlue());
	}
	raylib::DrawText(timeRemaining,
		this->mXOffset - (this->mTileSize / 2) - fontWidthTimeRemaining,
		(this->mApp->mWindow->GetHeight() / 2) + fontSize * 5,
		fontSize * 2,
		this->mSecondsRemaining < 10 ? raylib::Color::Red() : color);

	if (this->mSecondsRemaining <= 0.0f && this->mFallDelay <= 0.0f)
	{
		const char* restartText = "Press R to restart the game!";
		int rrTextSize = this->mTileSize / 2;
		int rrTextWidth = raylib::MeasureText(restartText, rrTextSize);
		raylib::DrawText(restartText,
			this->mXOffset + (this->mWidth / 2 * this->mTileSize) - rrTextWidth / 2,
			this->mYOffset + (this->mHeight / 2 * this->mTileSize),
			this->mTileSize / 2, 
			color);
	}
}
