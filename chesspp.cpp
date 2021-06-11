//
// Created by Aleksandr Lvov on 2021-06-10.
//

#include "chesspp.h"

#include <utility>

// ========== PIECES ========== //
Piece::Piece(Piece::Colour colour, Piece::Type type)
        : colour(colour), type(type){}

auto switched(Piece::Colour colour) -> Piece::Colour {
    return colour == Piece::Colour::White
    ? Piece::Colour::Black : Piece::Colour::White;
}

// ========== COORDINATES ========== //
auto offTheBoard(Coords0x88 coords) -> bool {
    return static_cast<unsigned char>(coords) & 0x88;
}

auto coordsTo8x8(Coords0x88 coords) -> unsigned char {
    return (static_cast<unsigned char>(coords) +
    (static_cast<unsigned char>(coords) & 7)) >> 1;
}

auto coordsFrom8x8(unsigned char coords) -> Coords0x88 {
    return Coords0x88(coords + (coords & ~7));
}

constexpr auto operator-(Direction lhs, Direction rhs) -> Direction {
    return Direction(static_cast<unsigned char>(lhs) - 0x77 + static_cast<unsigned char>(rhs));
}

constexpr auto Apply(Coords0x88 c, Direction dir) -> Coords0x88 {
    return Coords0x88(static_cast<unsigned char>(c) + 0x77 - static_cast<unsigned char>(dir));
}


// ========== GAME STATE ========== //

GameState::GameState(Piece::Colour colourToMove, std::shared_ptr<const GameState> prev) noexcept
        : colourToMove_(colourToMove), previousState_(std::move(prev)) {}

auto GameState::colourToMove() const -> Piece::Colour {
    return colourToMove_;
}

auto GameState::previousState() const -> std::shared_ptr<const GameState> {
    return previousState_;
}

auto GameState::availableMoves() const -> std::vector<Move> {
    auto result = std::vector<Move>();
    for (unsigned char c = 0; c < 0x78; ++c) {
        const auto piece = cell(Coords0x88(c));
        if (!piece || piece->colour != colourToMove_) {
            continue;
        }
        switch (piece->type) {
            case Piece::Type::Pawn:
                break;
            case Piece::Type::Bishop:
                break;
            case Piece::Type::Knight:
                break;
            case Piece::Type::Rook:
                break;
            case Piece::Type::Queen:
                break;
            case Piece::Type::King:
                break;
        }
    }
    return result;
}


FullGameState::FullGameState(
        Piece::Colour colour,
        std::shared_ptr<const GameState> prev,
        FullGameState::Board board) noexcept
        : GameState(colour, std::move(prev)), board_(std::move(board)) {}

auto FullGameState::cell(Coords0x88 coords) const -> std::optional<Piece> {
    return board_[static_cast<unsigned char>(coords)];
}

auto FullGameState::withMove(Move move) const -> std::shared_ptr<const GameState> {
    return std::make_shared<const PartialGameState>(switched(colourToMove_), shared_from_this(), move);
}


PartialGameState::PartialGameState(
        Piece::Colour colour,
        std::shared_ptr<const GameState> prev,
        Move move) noexcept
        : GameState(colour, std::move(prev)), move_(std::move(move)) {}

auto PartialGameState::cell(Coords0x88 coords) const -> std::optional<Piece> {
    if (move_.first.from == coords) {
        return {};
    } else if (move_.first.to == coords) {
        return move_.first.piece;
    } else if (move_.second) {
        if (move_.second->from == coords) {
            return {};
        } else if (move_.second->to == coords) {
            return move_.second->piece;
        }
    }
    return previousState_->cell(coords);
}

auto PartialGameState::withMove(Move move) const -> std::shared_ptr<const GameState> {
    return std::make_shared<const PartialGameState>(switched(colourToMove_), shared_from_this(), move);
}

