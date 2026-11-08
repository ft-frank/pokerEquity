#pragma once
#include <cstdint>   // for std::uint32_t / std::uint64_t
#include <string>    // for std::string
#include "basicFunctions.h"   // CardHand, RankMask, bit() … defined once, there

using CardHand = std::uint64_t; //


using Score = std::uint32_t;

enum Category{ HighCard, Pair, TwoPair, Trips, Straight, Flush, FullHouse, Quads, StraightFlush };

Score evaluate(CardHand hand);                          // 5–7 cards; higher score wins
std::string readHandScore(Score s);
constexpr Category category(Score s) { return Category(s >> 26); }