#include "Board.hpp"
#include "raylib-cpp.hpp"
#include <sstream>
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

	auto boardMock = ParseBoardString(
R"(brygyrgr
ygbpwpwg
gygwygyw
gwygwywg
gwywgygy
rwprpwrw
ryygbgbb
pprpprpr
)");
	this->mBoard = boardMock;
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
	
}

void Cyrey::Board::Draw() const
{
	this->DrawCheckerboard();
	this->DrawPieces();
	this->DrawHoverSquare();
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

bool Cyrey::Board::TrySwap(int x, int y, int toX, int toY)
{
	if (!this->IsSwapLegal(x, y, toX, toY))
		return false;

	Cyrey::Piece temp = this->mBoard[x][y];
	this->mBoard[x][y] = this->mBoard[toX][toY];
	this->mBoard[toX][toY] = temp;
	return true;
}

bool Cyrey::Board::IsSwapLegal(int x, int y, int toX, int toY) const
{
	if (toX < 0 || toY < 0 || toX >= this->mWidth || toY >= this->mHeight)
		return false; //out of bounds
	
	int xDiff = abs(x - toX);
	int yDiff = abs(y - toY);

	//swap with only adjacent squares. no diagonal swaps. swap with itself is also considered illegal
	if (xDiff > 1 || yDiff > 1 || !(xDiff ^ yDiff))
		return false;

	return true;
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
		else if (this->mDragging)
		{
			//this->mBoard[this->mDragTileBegin.x][this->mDragTileBegin.y].mDragging = true;

			float xDiff = this->mDragMouseBegin.x - mousePos.x;
			float yDiff = this->mDragMouseBegin.y - mousePos.y;

			float xTileBegin = this->mDragTileBegin.x;
			float yTileBegin = this->mDragTileBegin.y;

			if (abs(xDiff) > (this->mTileSize * 0.50f))
			{
				this->TrySwap(xTileBegin, yTileBegin, xTileBegin + (xDiff > 0 ? -1 : 1), yTileBegin);
				this->mDragging = false;
				this->mTriedSwap = true;
			}
			else if (abs(yDiff) > (this->mTileSize * 0.50f))
			{
				this->TrySwap(xTileBegin, yTileBegin, xTileBegin, yTileBegin + (yDiff > 0 ? -1 : 1));
				this->mDragging = false;
				this->mTriedSwap = true;
			}
		}
	}
	else if (raylib::Mouse::IsButtonReleased(MouseButton::MOUSE_BUTTON_LEFT))
	{
		//this->mBoard[this->mDragTileBegin.x][this->mDragTileBegin.y].mDragging = false;
		this->mDragging = false;
		this->mDragMouseBegin = raylib::Vector2::Zero();
		this->mDragTileBegin = raylib::Vector2::Zero();
		this->mTriedSwap = false;
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

	raylib::Rectangle(
		(float)(((*hoveredTile).x * mTileSize) + this->mXOffset + 1),
		(float)(((*hoveredTile).y * mTileSize) + this->mYOffset + 1),
		(float)(this->mTileSize - 2),
		(float)(this->mTileSize - 2)
	).DrawRoundedLines(0.0f, 1, this->mTileInset / 2.0f, this->mDragging ? raylib::Color::Green() : raylib::Color::Orange());
}
