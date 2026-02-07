#ifndef TINYCHESS_H
#define TINYCHESS_H

#include <array>
#include <cstdint>
#include <random>
#include "tinynet.h"

// Core game constants
constexpr int kBoardSize = 128;      // 0x88 board representation
constexpr float kMateScore = 10000.0f;

// Piece identifiers (positive: white, negative: black)
enum Piece
{
    EMPTY = 0,
    WP = 1,
    WN = 2,
    WB = 3,
    WR = 4,
    WQ = 5,
    WK = 6,
    BP = -1,
    BN = -2,
    BB = -3,
    BR = -4,
    BQ = -5,
    BK = -6
};

struct Move
{
    int from = 0;
    int to = 0;
    int promote = 0;
    bool enPassant = false;
    bool castle = false;
};

struct Board
{
    std::array<int8_t, kBoardSize> squares{};
    int side = 1;
    uint8_t castling = 0;
    int enPassant = -1;
};

struct TinyNet
{
    TinyNetC c_net;

    float predict(const std::array<float, TINYNET_INPUT_SIZE>& x) const;
    void init(std::mt19937& rng);
    void train_on_sample(const std::array<float, TINYNET_INPUT_SIZE>& x, float target, float lr);
};

#endif // TINYCHESS_H
