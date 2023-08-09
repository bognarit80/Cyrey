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
	int screenWidth = this->mApp->mWindow->GetWidth();
	int screenHeight = this->mApp->mWindow->GetHeight();

	if (screenWidth > 0 && screenHeight > 0)
	{
		this->mTileSize = (screenHeight * this->mZoomPct / 100) / this->mHeight;
		this->mTileInset = this->mTileSize / 10;
		this->mXOffset = (screenWidth / 2) - (this->mWidth * mTileSize / 2);
		this->mYOffset = (screenHeight / 2) - (this->mWidth * mTileSize / 2);
	}

	this->mZoomPct += raylib::Mouse::GetWheelMove();
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
				(float)((j * this->mTileSize) + this->mXOffset + this->mTileInset),
				(float)((i * this->mTileSize) + this->mYOffset + this->mTileInset),
				(float)this->mTileSize - (this->mTileInset * 2),
				(float)this->mTileSize - (this->mTileInset * 2)
			).Draw(color);
		}
	}
}

void Cyrey::Board::DrawHoverSquare() const
{
	int mouseX = raylib::Mouse::GetX() - this->mXOffset;
	int mouseY = raylib::Mouse::GetY() - this->mYOffset;
	if (mouseX <= 0 || mouseY <= 0 || this->mTileSize <= 0) return;

	int xTile = mouseX / mTileSize;
	int yTile = mouseY / mTileSize;
	if (xTile >= this->mWidth ||
		yTile >= this->mHeight)
		return;

	raylib::Rectangle(
		(float)((xTile * mTileSize) + this->mXOffset + 1),
		(float)((yTile * mTileSize) + this->mYOffset + 1),
		(float)(this->mTileSize - 2),
		(float)(this->mTileSize - 2)
	).DrawRoundedLines(0.0f, 1, this->mTileInset / 2.0f, raylib::Color::Orange());
}
