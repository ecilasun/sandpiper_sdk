#include <array>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <vector>
#include "tinynet.h"

namespace
{
constexpr int kBoardSize = 128;
constexpr float kMateScore = 10000.0f;

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

static inline int color_of(int piece)
{
	return (piece > 0) - (piece < 0);
}

static inline int abs_piece(int piece)
{
	return piece >= 0 ? piece : -piece;
}

static inline int make_sq(int file, int rank)
{
	return rank * 16 + file;
}

static inline int file_of(int sq)
{
	return sq & 7;
}

static inline int rank_of(int sq)
{
	return sq >> 4;
}

static inline bool on_board(int sq)
{
	return (sq & 0x88) == 0;
}

static void set_start_position(Board& b)
{
	b.squares.fill(EMPTY);
	b.side = 1;
	b.castling = 0x0F;
	b.enPassant = -1;

	const int back_rank[8] = {WR, WN, WB, WQ, WK, WB, WN, WR};
	for (int f = 0; f < 8; ++f)
	{
		b.squares[make_sq(f, 0)] = back_rank[f];
		b.squares[make_sq(f, 1)] = WP;
		b.squares[make_sq(f, 6)] = BP;
		b.squares[make_sq(f, 7)] = -back_rank[f];
	}
}

static int piece_value(int piece)
{
	switch (abs_piece(piece))
	{
	case WP: return 100;
	case WN: return 320;
	case WB: return 330;
	case WR: return 500;
	case WQ: return 900;
	case WK: return 0;
	default: return 0;
	}
}

static int center_bonus(int file, int rank)
{
	int df = file > 3 ? file - 3 : 3 - file;
	int dr = rank > 3 ? rank - 3 : 3 - rank;
	int dist = df + dr;
	int bonus = 6 - dist;
	return bonus > 0 ? bonus : 0;
}

static int piece_square_bonus(int piece, int sq)
{
	int file = file_of(sq);
	int rank = rank_of(sq);
	int side = color_of(piece);
	int rel_rank = side == 1 ? rank : 7 - rank;
	int abs_piece_id = abs_piece(piece);

	switch (abs_piece_id)
	{
	case WP:
		return rel_rank * 6;
	case WN:
		return center_bonus(file, rank) * 4;
	case WB:
		return center_bonus(file, rank) * 3;
	case WR:
		return rel_rank * 2;
	case WQ:
		return center_bonus(file, rank) * 2;
	case WK:
		return -rel_rank * 2;
	default:
		return 0;
	}
}

static int heuristic_eval(const Board& b)
{
	int score = 0;
	for (int r = 0; r < 8; ++r)
	{
		for (int f = 0; f < 8; ++f)
		{
			int sq = make_sq(f, r);
			int piece = b.squares[sq];
			if (piece == EMPTY)
				continue;
			int value = piece_value(piece) + piece_square_bonus(piece, sq);
			score += value * color_of(piece);
		}
	}
	return b.side == 1 ? score : -score;
}

static void encode_features(const Board& b, std::array<float, TINYNET_INPUT_SIZE>& out)
{
	out.fill(0.0f);
	for (int r = 0; r < 8; ++r)
	{
		for (int f = 0; f < 8; ++f)
		{
			int sq = make_sq(f, r);
			int piece = b.squares[sq];
			if (piece == EMPTY)
				continue;
			int index = (abs_piece(piece) - 1) + (piece < 0 ? 6 : 0);
			int flat = r * 8 + f;
			out[index * 64 + flat] = 1.0f;
		}
	}
	out[TINYNET_INPUT_SIZE - 1] = b.side == 1 ? 1.0f : -1.0f;
}

struct TinyNet
{
	TinyNetC c_net;

	float predict(const std::array<float, TINYNET_INPUT_SIZE>& x) const
	{
		return tinynet_predict(&c_net, x.data());
	}

	void init(std::mt19937& rng)
	{
		std::uniform_real_distribution<float> dist(-0.05f, 0.05f);
		for (int i = 0; i < TINYNET_HIDDEN_SIZE * TINYNET_INPUT_SIZE; ++i)
			c_net.w1[i] = dist(rng);
		for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
			c_net.b1[i] = dist(rng);
		for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
			c_net.w2[i] = dist(rng);
		c_net.b2 = dist(rng);
	}

	void train_on_sample(const std::array<float, TINYNET_INPUT_SIZE>& x, float target, float lr)
	{
		tinynet_train_on_sample(&c_net, x.data(), target, lr);
	}
};

static Move find_best_move(const TinyNet& net, const Board& b, int depth, std::mt19937& rng);

static bool is_square_attacked(const Board& b, int sq, int bySide)
{
	int pawnDir = bySide == 1 ? -16 : 16;
	int pawnLeft = pawnDir - 1;
	int pawnRight = pawnDir + 1;
	int pawnSq = sq + pawnLeft;
	if (on_board(pawnSq) && b.squares[pawnSq] == (bySide == 1 ? WP : BP))
		return true;
	pawnSq = sq + pawnRight;
	if (on_board(pawnSq) && b.squares[pawnSq] == (bySide == 1 ? WP : BP))
		return true;

	const int knightOffsets[8] = {31, 33, 14, 18, -31, -33, -14, -18};
	for (int off : knightOffsets)
	{
		int nsq = sq + off;
		if (!on_board(nsq))
			continue;
		int piece = b.squares[nsq];
		if (piece == (bySide == 1 ? WN : BN))
			return true;
	}

	const int bishopOffsets[4] = {15, 17, -15, -17};
	for (int off : bishopOffsets)
	{
		int nsq = sq + off;
		while (on_board(nsq))
		{
			int piece = b.squares[nsq];
			if (piece != EMPTY)
			{
				if (color_of(piece) == bySide && (abs_piece(piece) == WB || abs_piece(piece) == WQ))
					return true;
				break;
			}
			nsq += off;
		}
	}

	const int rookOffsets[4] = {16, -16, 1, -1};
	for (int off : rookOffsets)
	{
		int nsq = sq + off;
		while (on_board(nsq))
		{
			int piece = b.squares[nsq];
			if (piece != EMPTY)
			{
				if (color_of(piece) == bySide && (abs_piece(piece) == WR || abs_piece(piece) == WQ))
					return true;
				break;
			}
			nsq += off;
		}
	}

	const int kingOffsets[8] = {16, -16, 1, -1, 15, 17, -15, -17};
	for (int off : kingOffsets)
	{
		int nsq = sq + off;
		if (!on_board(nsq))
			continue;
		int piece = b.squares[nsq];
		if (piece == (bySide == 1 ? WK : BK))
			return true;
	}

	return false;
}

static bool is_in_check(const Board& b, int side)
{
	int king = side == 1 ? WK : BK;
	int kingSq = -1;
	for (int r = 0; r < 8 && kingSq == -1; ++r)
	{
		for (int f = 0; f < 8; ++f)
		{
			int sq = make_sq(f, r);
			if (b.squares[sq] == king)
			{
				kingSq = sq;
				break;
			}
		}
	}
	if (kingSq == -1)
		return true;
	return is_square_attacked(b, kingSq, -side);
}

static void add_move(std::vector<Move>& moves, int from, int to, int promote = 0, bool enPassant = false, bool castle = false)
{
	Move m;
	m.from = from;
	m.to = to;
	m.promote = promote;
	m.enPassant = enPassant;
	m.castle = castle;
	moves.push_back(m);
}

static void generate_pseudo_moves(const Board& b, std::vector<Move>& moves)
{
	moves.clear();
	int side = b.side;

	for (int r = 0; r < 8; ++r)
	{
		for (int f = 0; f < 8; ++f)
		{
			int sq = make_sq(f, r);
			int piece = b.squares[sq];
			if (piece == EMPTY || color_of(piece) != side)
				continue;

			int absP = abs_piece(piece);
			if (absP == WP)
			{
				int dir = side == 1 ? 16 : -16;
				int startRank = side == 1 ? 1 : 6;
				int promoRank = side == 1 ? 6 : 1;
				int next = sq + dir;
				if (on_board(next) && b.squares[next] == EMPTY)
				{
					if (r == promoRank)
					{
						add_move(moves, sq, next, WQ);
						add_move(moves, sq, next, WR);
						add_move(moves, sq, next, WB);
						add_move(moves, sq, next, WN);
					}
					else
					{
						add_move(moves, sq, next);
						if (r == startRank)
						{
							int jump = sq + dir * 2;
							if (b.squares[jump] == EMPTY)
								add_move(moves, sq, jump);
						}
					}
				}

				int capOffsets[2] = {dir - 1, dir + 1};
				for (int off : capOffsets)
				{
					int tsq = sq + off;
					if (!on_board(tsq))
						continue;
					if (tsq == b.enPassant)
					{
						add_move(moves, sq, tsq, 0, true);
						continue;
					}
					int tpiece = b.squares[tsq];
					if (tpiece != EMPTY && color_of(tpiece) == -side)
					{
						if (r == promoRank)
						{
							add_move(moves, sq, tsq, WQ);
							add_move(moves, sq, tsq, WR);
							add_move(moves, sq, tsq, WB);
							add_move(moves, sq, tsq, WN);
						}
						else
						{
							add_move(moves, sq, tsq);
						}
					}
				}
			}
			else if (absP == WN)
			{
				const int offsets[8] = {31, 33, 14, 18, -31, -33, -14, -18};
				for (int off : offsets)
				{
					int tsq = sq + off;
					if (!on_board(tsq))
						continue;
					int tpiece = b.squares[tsq];
					if (tpiece == EMPTY || color_of(tpiece) == -side)
						add_move(moves, sq, tsq);
				}
			}
			else if (absP == WB || absP == WR || absP == WQ)
			{
				int offsets[8];
				int count = 0;
				if (absP == WB || absP == WQ)
				{
					offsets[count++] = 15;
					offsets[count++] = 17;
					offsets[count++] = -15;
					offsets[count++] = -17;
				}
				if (absP == WR || absP == WQ)
				{
					offsets[count++] = 16;
					offsets[count++] = -16;
					offsets[count++] = 1;
					offsets[count++] = -1;
				}
				for (int i = 0; i < count; ++i)
				{
					int off = offsets[i];
					int tsq = sq + off;
					while (on_board(tsq))
					{
						int tpiece = b.squares[tsq];
						if (tpiece == EMPTY)
						{
							add_move(moves, sq, tsq);
						}
						else
						{
							if (color_of(tpiece) == -side)
								add_move(moves, sq, tsq);
							break;
						}
						tsq += off;
					}
				}
			}
			else if (absP == WK)
			{
				const int offsets[8] = {16, -16, 1, -1, 15, 17, -15, -17};
				for (int off : offsets)
				{
					int tsq = sq + off;
					if (!on_board(tsq))
						continue;
					int tpiece = b.squares[tsq];
					if (tpiece == EMPTY || color_of(tpiece) == -side)
						add_move(moves, sq, tsq);
				}

				if (side == 1 && sq == make_sq(4, 0))
				{
					if ((b.castling & 0x01) && b.squares[make_sq(5, 0)] == EMPTY && b.squares[make_sq(6, 0)] == EMPTY)
					{
						if (!is_square_attacked(b, make_sq(4, 0), -side) &&
							!is_square_attacked(b, make_sq(5, 0), -side) &&
							!is_square_attacked(b, make_sq(6, 0), -side))
							add_move(moves, sq, make_sq(6, 0), 0, false, true);
					}
					if ((b.castling & 0x02) && b.squares[make_sq(3, 0)] == EMPTY && b.squares[make_sq(2, 0)] == EMPTY && b.squares[make_sq(1, 0)] == EMPTY)
					{
						if (!is_square_attacked(b, make_sq(4, 0), -side) &&
							!is_square_attacked(b, make_sq(3, 0), -side) &&
							!is_square_attacked(b, make_sq(2, 0), -side))
							add_move(moves, sq, make_sq(2, 0), 0, false, true);
					}
				}
				else if (side == -1 && sq == make_sq(4, 7))
				{
					if ((b.castling & 0x04) && b.squares[make_sq(5, 7)] == EMPTY && b.squares[make_sq(6, 7)] == EMPTY)
					{
						if (!is_square_attacked(b, make_sq(4, 7), -side) &&
							!is_square_attacked(b, make_sq(5, 7), -side) &&
							!is_square_attacked(b, make_sq(6, 7), -side))
							add_move(moves, sq, make_sq(6, 7), 0, false, true);
					}
					if ((b.castling & 0x08) && b.squares[make_sq(3, 7)] == EMPTY && b.squares[make_sq(2, 7)] == EMPTY && b.squares[make_sq(1, 7)] == EMPTY)
					{
						if (!is_square_attacked(b, make_sq(4, 7), -side) &&
							!is_square_attacked(b, make_sq(3, 7), -side) &&
							!is_square_attacked(b, make_sq(2, 7), -side))
							add_move(moves, sq, make_sq(2, 7), 0, false, true);
					}
				}
			}
		}
	}
}

static Board make_move(const Board& b, const Move& m)
{
	Board nb = b;
	int piece = nb.squares[m.from];
	int side = color_of(piece);
	int captured = nb.squares[m.to];

	nb.squares[m.from] = EMPTY;
	nb.enPassant = -1;

	if (m.enPassant)
	{
		int capSq = m.to + (side == 1 ? -16 : 16);
		captured = nb.squares[capSq];
		nb.squares[capSq] = EMPTY;
	}

	if (m.castle)
	{
		if (side == 1 && m.to == make_sq(6, 0))
		{
			nb.squares[make_sq(5, 0)] = WR;
			nb.squares[make_sq(7, 0)] = EMPTY;
		}
		else if (side == 1 && m.to == make_sq(2, 0))
		{
			nb.squares[make_sq(3, 0)] = WR;
			nb.squares[make_sq(0, 0)] = EMPTY;
		}
		else if (side == -1 && m.to == make_sq(6, 7))
		{
			nb.squares[make_sq(5, 7)] = BR;
			nb.squares[make_sq(7, 7)] = EMPTY;
		}
		else if (side == -1 && m.to == make_sq(2, 7))
		{
			nb.squares[make_sq(3, 7)] = BR;
			nb.squares[make_sq(0, 7)] = EMPTY;
		}
	}

	if (m.promote != 0)
		piece = side == 1 ? m.promote : -m.promote;

	nb.squares[m.to] = piece;

	if (abs_piece(piece) == WK)
		nb.castling &= side == 1 ? 0x0C : 0x03;
	if (abs_piece(piece) == WR)
	{
		if (m.from == make_sq(0, 0))
			nb.castling &= ~0x02;
		else if (m.from == make_sq(7, 0))
			nb.castling &= ~0x01;
		else if (m.from == make_sq(0, 7))
			nb.castling &= ~0x08;
		else if (m.from == make_sq(7, 7))
			nb.castling &= ~0x04;
	}
	if (abs_piece(captured) == WR)
	{
		if (m.to == make_sq(0, 0))
			nb.castling &= ~0x02;
		else if (m.to == make_sq(7, 0))
			nb.castling &= ~0x01;
		else if (m.to == make_sq(0, 7))
			nb.castling &= ~0x08;
		else if (m.to == make_sq(7, 7))
			nb.castling &= ~0x04;
	}

	if (abs_piece(piece) == WP && std::abs(m.to - m.from) == 32)
		nb.enPassant = m.from + (side == 1 ? 16 : -16);

	nb.side = -nb.side;
	return nb;
}

static void generate_legal_moves(const Board& b, std::vector<Move>& moves)
{
	std::vector<Move> pseudo;
	generate_pseudo_moves(b, pseudo);
	moves.clear();
	moves.reserve(pseudo.size());

	for (const Move& m : pseudo)
	{
		Board nb = make_move(b, m);
		if (!is_in_check(nb, -nb.side))
			moves.push_back(m);
	}
}

static std::string move_to_uci(const Move& m)
{
	char buf[6] = {0};
	buf[0] = 'a' + file_of(m.from);
	buf[1] = '1' + rank_of(m.from);
	buf[2] = 'a' + file_of(m.to);
	buf[3] = '1' + rank_of(m.to);
	if (m.promote != 0)
	{
		char c = 'q';
		if (m.promote == WR)
			c = 'r';
		else if (m.promote == WB)
			c = 'b';
		else if (m.promote == WN)
			c = 'n';
		buf[4] = c;
	}
	return std::string(buf);
}

static bool parse_square(const std::string& text, int offset, int& outSq)
{
	if (offset + 1 >= static_cast<int>(text.size()))
		return false;
	char file = std::tolower(text[offset]);
	char rank = text[offset + 1];
	if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
		return false;
	outSq = make_sq(file - 'a', rank - '1');
	return true;
}

static bool parse_move(const std::string& text, const std::vector<Move>& legal, Move& outMove)
{
	if (text.size() < 4)
		return false;
	int from = 0;
	int to = 0;
	if (!parse_square(text, 0, from) || !parse_square(text, 2, to))
		return false;

	int promote = 0;
	if (text.size() >= 5)
	{
		char p = std::tolower(text[4]);
		switch (p)
		{
		case 'q': promote = WQ; break;
		case 'r': promote = WR; break;
		case 'b': promote = WB; break;
		case 'n': promote = WN; break;
		default: break;
		}
	}

	for (const Move& m : legal)
	{
		if (m.from == from && m.to == to)
		{
			if (m.promote == promote || (promote == 0 && m.promote == WQ))
			{
				outMove = m;
				return true;
			}
		}
	}
	return false;
}

static void print_board(const Board& b)
{
	for (int r = 7; r >= 0; --r)
	{
		std::printf("%d ", r + 1);
		for (int f = 0; f < 8; ++f)
		{
			int sq = make_sq(f, r);
			int piece = b.squares[sq];
			char c = '.';
			switch (abs_piece(piece))
			{
			case WP: c = 'P'; break;
			case WN: c = 'N'; break;
			case WB: c = 'B'; break;
			case WR: c = 'R'; break;
			case WQ: c = 'Q'; break;
			case WK: c = 'K'; break;
			default: c = '.'; break;
			}
			if (piece < 0)
				c = static_cast<char>(std::tolower(c));
			std::printf("%c ", c);
		}
		std::printf("\n");
	}
	std::printf("  a b c d e f g h\n\n");
}

static Board random_position(std::mt19937& rng)
{
	Board b;
	set_start_position(b);
	std::uniform_int_distribution<int> pliesDist(6, 18);
	int plies = pliesDist(rng);
	std::vector<Move> moves;
	for (int i = 0; i < plies; ++i)
	{
		generate_legal_moves(b, moves);
		if (moves.empty())
			break;
		std::uniform_int_distribution<int> pick(0, static_cast<int>(moves.size()) - 1);
		b = make_move(b, moves[pick(rng)]);
	}
	return b;
}

static void train_model(TinyNet& net, std::mt19937& rng)
{
	const int kSamples = 256;
	const int kEpochs = 2;
	const float kLearnRate = 0.01f;
	std::array<float, TINYNET_INPUT_SIZE> features{};

	for (int e = 0; e < kEpochs; ++e)
	{
		std::printf("Epoch %d/%d: ", e + 1, kEpochs);
		for (int i = 0; i < kSamples; ++i)
		{
			Board b = random_position(rng);
			encode_features(b, features);
			float target = static_cast<float>(heuristic_eval(b)) / 100.0f;
			net.train_on_sample(features, target, kLearnRate);
			if ((i + 1) % 64 == 0 || i + 1 == kSamples)
			{
				int pct = (i + 1) * 100 / kSamples;
				std::printf("%d%% ", pct);
				std::fflush(stdout);
			}
		}
		std::printf("\n");
	}
}

static void self_play_train(TinyNet& net, std::mt19937& rng)
{
	const int kGames = 6;
	const int kMaxPlies = 24;
	const int kSearchDepth = 2;
	const float kLearnRate = 0.005f;
	std::array<float, TINYNET_INPUT_SIZE> features{};

	for (int g = 0; g < kGames; ++g)
	{
		Board b;
		set_start_position(b);
		int plies = 0;
		std::printf("Self-play game %d/%d\n", g + 1, kGames);
		for (int p = 0; p < kMaxPlies; ++p)
		{
			std::vector<Move> legal;
			generate_legal_moves(b, legal);
			if (legal.empty())
				break;

			Move m = find_best_move(net, b, kSearchDepth, rng);
			encode_features(b, features);
			float target = static_cast<float>(heuristic_eval(b)) / 100.0f;
			net.train_on_sample(features, target, kLearnRate);
			b = make_move(b, m);
			++plies;
			if ((p + 1) % 8 == 0)
			{
				std::printf("  progress: %d/%d plies\n", p + 1, kMaxPlies);
			}
		}
		std::printf("  done (%d plies)\n", plies);
	}
}

static float evaluate_net(const TinyNet& net, const Board& b)
{
	std::array<float, TINYNET_INPUT_SIZE> features{};
	encode_features(b, features);
	return net.predict(features);
}

static float negamax(const TinyNet& net, const Board& b, int depth, float alpha, float beta)
{
	std::vector<Move> moves;
	generate_legal_moves(b, moves);
	if (moves.empty())
	{
		if (is_in_check(b, b.side))
			return -kMateScore + depth;
		return 0.0f;
	}
	if (depth == 0)
		return evaluate_net(net, b);

	float best = -kMateScore;
	for (const Move& m : moves)
	{
		Board nb = make_move(b, m);
		float score = -negamax(net, nb, depth - 1, -beta, -alpha);
		if (score > best)
			best = score;
		if (score > alpha)
			alpha = score;
		if (alpha >= beta)
			break;
	}
	return best;
}

static Move find_best_move(const TinyNet& net, const Board& b, int depth, std::mt19937& rng)
{
	std::vector<Move> moves;
	generate_legal_moves(b, moves);
	Move best = moves.empty() ? Move{} : moves[0];
	float bestScore = -kMateScore;
	for (const Move& m : moves)
	{
		Board nb = make_move(b, m);
		float score = -negamax(net, nb, depth - 1, -kMateScore, kMateScore);
		if (score > bestScore + 0.0001f)
		{
			bestScore = score;
			best = m;
		}
		else if (std::abs(score - bestScore) < 0.0001f)
		{
			std::uniform_int_distribution<int> pick(0, 1);
			if (pick(rng) == 1)
				best = m;
		}
	}
	return best;
}

static bool select_opening_book_move(const std::vector<std::string>& history, const std::vector<Move>& legal, std::mt19937& rng, Move& outMove)
{
	struct Line
	{
		const char* moves[12];
		int count;
	};

	const Line lines[] = {
		{{"e2e4", "e7e5", "g1f3", "b8c6", "f1c4", "g8f6", "d2d3"}, 7},
		{{"e2e4", "c7c5", "g1f3", "d7d6", "d2d4", "c5d4", "f3d4"}, 7},
		{{"d2d4", "d7d5", "c2c4", "e7e6", "b1c3", "g8f6"}, 6},
		{{"c2c4", "e7e5", "b1c3", "g8f6", "g2g3", "d7d5"}, 6}
	};

	std::vector<Move> candidates;
	for (const Line& line : lines)
	{
		if (static_cast<int>(history.size()) >= line.count)
			continue;
		bool match = true;
		for (size_t i = 0; i < history.size(); ++i)
		{
			if (history[i] != line.moves[i])
			{
				match = false;
				break;
			}
		}
		if (!match)
			continue;
		Move candidate;
		if (parse_move(line.moves[history.size()], legal, candidate))
			candidates.push_back(candidate);
	}

	if (candidates.empty())
		return false;
	std::uniform_int_distribution<int> pick(0, static_cast<int>(candidates.size()) - 1);
	outMove = candidates[pick(rng)];
	return true;
}

static bool ask_yes_no(const char* prompt)
{
	std::printf("%s", prompt);
	char line[32] = {0};
	if (!std::fgets(line, sizeof(line), stdin))
		return true;
	return std::tolower(line[0]) == 'y';
}

} // namespace

int main()
{
	std::mt19937 rng(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
	TinyNet net;
	net.init(rng);

	std::printf("TinyChess: training a very small model...\n");
	train_model(net, rng);
	std::printf("Initial training complete.\n");
	std::printf("Starting self-play refinement...\n");
	self_play_train(net, rng);
	std::printf("Self-play complete.\n\n");

	Board board;
	set_start_position(board);

	bool humanWhite = ask_yes_no("Play as White? (y/n): ");
	int humanSide = humanWhite ? 1 : -1;
	int depth = 3;

	std::vector<std::string> history;
	while (true)
	{
		print_board(board);
		std::vector<Move> legal;
		generate_legal_moves(board, legal);
		if (legal.empty())
		{
			if (is_in_check(board, board.side))
				std::printf("Checkmate! %s wins.\n", board.side == humanSide ? "Engine" : "Human");
			else
				std::printf("Stalemate.\n");
			break;
		}

		if (board.side == humanSide)
		{
			std::printf("Your move (e2e4, resign, or quit): ");
			char line[64] = {0};
			if (!std::fgets(line, sizeof(line), stdin))
				break;
			std::string input(line);
			size_t end = input.find_last_not_of(" \t\r\n");
			if (end == std::string::npos)
				input.clear();
			else
				input.erase(end + 1);
			if (input == "quit" || input == "resign")
			{
				std::printf("Game over.\n");
				break;
			}
			Move chosen;
			if (!parse_move(input, legal, chosen))
			{
				std::printf("Illegal move. Try again.\n");
				continue;
			}
			board = make_move(board, chosen);
			history.push_back(move_to_uci(chosen));
		}
		else
		{
			Move best;
			bool usedBook = select_opening_book_move(history, legal, rng, best);
			if (!usedBook)
				best = find_best_move(net, board, depth, rng);
			std::string uci = move_to_uci(best);
			std::printf("Engine plays: %s\n\n", uci.c_str());
			board = make_move(board, best);
			history.push_back(uci);
		}
	}

	return 0;
}
