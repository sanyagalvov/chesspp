//
// Created by Aleksandr Lvov on 2021-06-10.
//

#pragma once

#include <array>
#include <optional>
#include <memory>

// TODO: write tests for all these classes.
// It should be done using GTest framework.


// ========== PIECES ========== //

class Piece {
public:
    enum class Type : char {
        Pawn, Bishop, Knight,
        Rook, King, Queen,
        };
    enum class Colour : char {
        White, Black
    };

    Piece(Colour colour, Type type);;

    Piece() = delete;

    const Colour colour;
    const Type   type;
};

inline auto switched(Piece::Colour) -> Piece::Colour;

// ========== COORDINATES ========== //

// I am using 0x88 coordinates system,
// because it is simple yet more convenient than using 0...64
// To know more: https://www.chessprogramming.org/0x88#Off_the_Board
enum class Coords0x88 : unsigned char {};

inline auto offTheBoard(Coords0x88 coords) -> bool;
inline auto coordsTo8x8(Coords0x88 coords) -> unsigned char;
inline auto coordsFrom8x8(unsigned char coords) -> Coords0x88;

// ========== MOVE ========== //

struct MoveStage {
    MoveStage(Piece piece, Coords0x88 from, Coords0x88 to)
    : piece(piece), from(from), to(to) {}

    const Piece piece;
    const Coords0x88 from, to;
};

// First member is the main part.
// Second member can be used for castling and en passant.
using Move = std::pair<MoveStage, std::optional<MoveStage>>;


// ========== GAME STATE ========== //

struct GameState : std::enable_shared_from_this<GameState> {
    GameState(Piece::Colour, std::shared_ptr<const GameState>) noexcept;

    virtual auto cell(Coords0x88) const -> std::optional<Piece> = 0;
    virtual auto withMove(Move) const -> std::shared_ptr<const GameState> = 0;

    virtual auto colourToMove() const -> Piece::Colour final;
    virtual auto previousState() const -> std::shared_ptr<const GameState> final;

    virtual ~GameState() = default;

protected:
    const Piece::Colour colourToMove_;
    const std::shared_ptr<const GameState> previousState_;
};

struct FullGameState final : GameState {
    using Board = std::array<std::optional<Piece>, 128>;

    FullGameState(Piece::Colour, std::shared_ptr<const GameState>, Board) noexcept;

    auto cell(Coords0x88) const -> std::optional<Piece> final;
    auto withMove(Move) const -> std::shared_ptr<const GameState> final;

private:
    const Board board_;
};

struct PartialGameState final : GameState {
    PartialGameState(Piece::Colour, std::shared_ptr<const GameState>, Move) noexcept;

    auto cell(Coords0x88) const -> std::optional<Piece> final;
    auto withMove(Move) const -> std::shared_ptr<const GameState> final;

private:
    const Move move_;
};


