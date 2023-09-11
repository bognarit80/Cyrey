#include "Piece.hpp"

bool Cyrey::Piece::IsFlagSet(unsigned int flag) const
{
    return (this->mFlags & flag) != 0;
}

void Cyrey::Piece::SetFlag(unsigned int flag)
{
    this->mFlags |= flag;
}

void Cyrey::Piece::Bombify()
{
    this->SetFlag(PieceFlag::Bomb);
    this->mImmunity = true;
    //TODO: play sound and effect
}

void Cyrey::Piece::Lightningify()
{
    this->SetFlag(PieceFlag::Lightning);
    this->mImmunity = true;
}

void Cyrey::Piece::Hypercubify()
{
    this->SetFlag(PieceFlag::Hypercube);
    this->mColor = PieceColor::Uncolored;
    this->mImmunity = true;
}
