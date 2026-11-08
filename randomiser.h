#pragma once
#include <cstdint>
#include <string>
#include <vector>

using Card = std::uint8_t;        // suit * 16 + rank. Rank 0 = 2, rank 12 = A.

std::vector<Card> makeDeck();