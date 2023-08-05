#include "Board.hpp"
#include "raylib-cpp.hpp"
#include <sstream>
#include <map>

Cyrey::Board::Board(int width, int height)
{
	this->mWidth = width;
	this->mHeight = height;
}

void Cyrey::Board::Init()
{
	this->mXOffset = 100;
	this->mYOffset = 100;
	this->mTileSize = 30;
	this->mBoardAlpha = 0.25f;

	/*std::vector<std::vector<Piece>> boardMock = {
		{ Piece(PieceColor::Red), Piece(PieceColor::Blue) },
		{ Piece(PieceColor::Purple), Piece(PieceColor::Orange) },
		{ Piece(PieceColor::Green), Piece(PieceColor::White) }
	};*/

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
	this->mBoard = std::make_unique<std::vector<std::vector<Piece>>>(boardMock);
	this->mWidth = mBoard->at(0).size();
	this->mHeight = mBoard->size();
}

void Cyrey::Board::Draw()
{
	this->mXOffset = (::GetScreenWidth() / 2) - (this->mWidth * mTileSize / 2);
	this->mYOffset = (::GetScreenHeight() / 2) - (this->mWidth * mTileSize / 2);
	this->DrawCheckerboard();
	this->DrawPieces();
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

void Cyrey::Board::DrawCheckerboard()
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
	).DrawRoundedLines(0.0f, 1, 3, raylib::Color::Gray());
}

void Cyrey::Board::DrawPieces()
{
	for (int i = 0; i < this->mBoard->size(); i++)
	{
		for (int j = 0; j < this->mBoard->at(0).size(); j++)
		{
			Color color;
			switch (this->mBoard->at(i).at(j).mColor)
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
				(float)((j* this->mTileSize) + this->mXOffset + 3),
					(float)((i* this->mTileSize) + this->mYOffset + 3),
					(float)this->mTileSize - 6,
					(float)this->mTileSize - 6
			).Draw(color);
		}
	}
}
