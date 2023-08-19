#include "Board.hpp"
#include <map>

void Cyrey::Board::Init()
{
	this->mXOffset = 100;
	this->mYOffset = 100;
	this->mTileSize = 30;
	this->mTileInset = 3;
	this->mBoardAlpha = 0.25f;
	this->mZoomPct = 90;
	this->mDragging = false;
	this->mTriedSwap = false;
	this->mScore = 0;
	this->mPiecesCleared = 0;
	this->mCascadeNumber = 0;
	this->mPiecesClearedInMove = 0;

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
	do
	{
		this->mMatchSets.clear();
		this->mBoard = this->GenerateStartingBoard();
	} while (this->FindSets()); //ugly until I make a better algorithm for creating boards with no sets

	this->mWidth = mBoard[0].size();
	this->mHeight = mBoard.size();
}

void Cyrey::Board::Update()
{
	this->mZoomPct += raylib::Mouse::GetWheelMove();
	this->UpdateDragging();

	int screenWidth = this->mApp->mWindow->GetWidth();
	int screenHeight = this->mApp->mWindow->GetHeight();

	if (screenWidth > 0 && screenHeight > 0)
	{
		this->mTileSize = (screenHeight * this->mZoomPct / 100) / this->mHeight;
		this->mTileInset = this->mTileSize / 10;
		this->mXOffset = (screenWidth / 2) - (this->mWidth * mTileSize / 2);
		this->mYOffset = (screenHeight / 2) - (this->mWidth * mTileSize / 2);
	}

	if (this->mCascadeDelay > 0.0f)
	{
		this->mCascadeDelay -= this->mApp->GetDeltaTime();
		if (this->mCascadeDelay <= 0.0f)
		{
			this->mCascadeDelay = 0.0f;
			this->UpdateFalling();
			this->UpdateMatchSets();
		}
	}
}

void Cyrey::Board::Draw() const
{
	this->DrawCheckerboard();
	this->DrawPieces();
	this->DrawHoverSquare();
	this->DrawScore();
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
			row.push_back(Piece(static_cast<PieceColor>(GetRandomValue(1, static_cast<int>(PieceColor::Count) - 1))));
		}
		board.push_back(row);
		row.clear();
	}
	return board;
}

std::optional<raylib::Vector2> Cyrey::Board::GetHoveredTile() const
{
	int mouseX = raylib::Mouse::GetX() - this->mXOffset;
	int mouseY = raylib::Mouse::GetY() - this->mYOffset;
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
		if(first)
			this->mMatchSets.push_back(*this->mCurrentMatchSet);
	}

	if (first)
		this->mCurrentMatchSet->mPieces.clear();

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

bool Cyrey::Board::TrySwap(int row, int col, int toRow, int toCol)
{
	if (!this->IsSwapLegal(row, col, toRow, toCol))
		return false;

	Cyrey::Piece temp = this->mBoard[row][col];
	this->mBoard[row][col] = this->mBoard[toRow][toCol];
	this->mBoard[toRow][toCol] = temp;
	//check only the pieces swapped for matches, everything else should be untouched if we can only swap from a non-moving state
	this->FindSets(row, col, this->mBoard[row][col].mColor);
	this->FindSets(toRow, toCol, this->mBoard[toRow][toCol].mColor);
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

constexpr bool Cyrey::Board::IsPositionLegal(int row, int col) const
{
	return !(row < 0 || col < 0 || row >= this->mWidth || col >= this->mHeight);
}

void Cyrey::Board::UpdateDragging()
{
	raylib::Vector2 mousePos = raylib::Mouse::GetPosition();
	if (raylib::Mouse::IsButtonDown(MouseButton::MOUSE_BUTTON_LEFT))
	{
		if (!this->mDragging 
			&& raylib::Mouse::GetDelta() != raylib::Vector2::Zero()
			&& this->IsMouseInBoard()
			&& !this->mTriedSwap)
		{
			this->mDragging = true;
			this->mDragMouseBegin = mousePos;
			this->mDragTileBegin = *this->GetHoveredTile(); //we're guaranteed to have a value here
		}
		else if (this->mDragging && this->mCascadeDelay <= 0.0f)
		{
			//this->mBoard[this->mDragTileBegin.row][this->mDragTileBegin.col].mDragging = true;

			float xDiff = this->mDragMouseBegin.x - mousePos.x;
			float yDiff = this->mDragMouseBegin.y - mousePos.y;

			float xTileBegin = this->mDragTileBegin.x;
			float yTileBegin = this->mDragTileBegin.y;

			if (abs(xDiff) > (this->mTileSize * 0.33f))
			{
				this->TrySwap(xTileBegin, yTileBegin, xTileBegin + (xDiff > 0 ? -1 : 1), yTileBegin);
				this->mDragging = false;
				this->mTriedSwap = true;
				this->UpdateMatchSets();
			}
			else if (abs(yDiff) > (this->mTileSize * 0.33f))
			{
				this->TrySwap(xTileBegin, yTileBegin, xTileBegin, yTileBegin + (yDiff > 0 ? -1 : 1));
				this->mDragging = false;
				this->mTriedSwap = true;
				this->UpdateMatchSets();
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
		this->mCascadeDelay = 0.20f;
		this->mCascadeNumber++;
	}
	for ( auto& matchSet : this->mMatchSets )
	{
		int piecesPerSet = 0;
		for ( auto piece : matchSet.mPieces )
		{
			*piece = Cyrey::gNullPiece;
			piecesPerSet++;
		}
		this->mPiecesCleared += piecesPerSet;
		this->mScore += (piecesPerSet - 2) * 50 * this->mCascadeNumber;
		this->mPiecesClearedInMove += piecesPerSet;
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
				this->mBoard[i][j + k - 1] = Cyrey::gNullPiece;
				k++;
			}
		}
	}
	this->FillInBlanks();
}

void Cyrey::Board::FillInBlanks()
{
	for (auto &row : this->mBoard)
	{
		for (auto &piece : row)
		{
			if (piece.mID == 0)
				piece = Piece((static_cast<PieceColor>(GetRandomValue(1, static_cast<int>(PieceColor::Count) - 1))));
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
	raylib::Rectangle(
		(float)(this->mXOffset - 1),
		(float)(this->mYOffset - 1),
		(float)((this->mTileSize * this->mWidth) + 2),
		(float)((this->mTileSize * this->mHeight) + 2)
	).DrawRoundedLines(0.0f, 1, 3, this->mApp->mDarkMode ? raylib::Color::Gray() : raylib::Color::DarkGray());
}

void Cyrey::Board::DrawPieces() const
{
	for (int i = 0; i < this->mBoard.size(); i++)
	{
		for (int j = 0; j < this->mBoard[0].size(); j++)
		{
			Color color;
			switch (this->mBoard[i][j].mColor)
			{
			case PieceColor::Red:
				color = raylib::Color::Red(); break;
			case PieceColor::Green:
				color = raylib::Color::Green(); break;
			case PieceColor::Blue:
				color = raylib::Color::Blue(); break;
			case PieceColor::Yellow:
				color = raylib::Color::Yellow(); break;
			case PieceColor::Orange:
				color = raylib::Color::Orange(); break;
			case PieceColor::White:
				color = raylib::Color::White(); break;
			case PieceColor::Purple:
				color = raylib::Color::Purple(); break;
			case PieceColor::Uncolored:
			default:
				continue;
			}

			raylib::Rectangle(
				(float)((i * this->mTileSize) + this->mXOffset + this->mTileInset) + (this->mBoard[i][j].mDragging ? this->mBoard[i][j].mXDiff : 0),
				(float)((j * this->mTileSize) + this->mYOffset + this->mTileInset) + (this->mBoard[i][j].mDragging ? this->mBoard[i][j].mYDiff : 0),
				(float)this->mTileSize - (this->mTileInset * 2),
				(float)this->mTileSize - (this->mTileInset * 2)
			).Draw(color);
		}
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
	if (this->mCascadeDelay > 0.0f)
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
	std::string score = "Score: " + std::to_string(this->mScore);
	std::string pieces = "Pieces cleared: " + std::to_string(this->mPiecesCleared);
	std::string cascades = "Cascades: " + std::to_string(this->mCascadeNumber);
	std::string piecesInMove = "Pieces: " + std::to_string(this->mPiecesClearedInMove);
	int fontSize = this->mApp->mWindow->GetHeight() / 30;
	int fontWidthScore = raylib::MeasureText(score, fontSize);
	int fontWidthPieces = raylib::MeasureText(pieces, fontSize);
	int fontWidthCascades = raylib::MeasureText(cascades, fontSize);
	int fontWidthPiecesInMove = raylib::MeasureText(piecesInMove, fontSize);
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
}
