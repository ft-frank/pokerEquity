#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include <bit>
#include <random>
#include <optional>


using Card = std::uint8_t; //Card represented by (suit * 16 + rank). Rank 0 = 2, Rank 12 = A.
using CardHand = std::uint64_t; //
using RankMask = std::uint32_t;


constexpr CardHand bit(Card c) { return CardHand{ 1 } << c; }; //Returns a CardHand that only contains the card c.
constexpr int rank(Card c) { return c & 15; }; //Returns the rank of the card. Ranks are 0->12. Therefore 13th, 14th will not exist.
constexpr int suit(Card c) { return c >> 4; }; //Right shift card by 4 (divide by 4) to return the suit in integer
constexpr std::uint16_t suit_mas(CardHand s, int su) {
    return (s >> (16 * su) & 0x1FFF); //Takes a hand CardHand, then returns all cards that are within that specific suit.
}

constexpr RankMask suit_mask(CardHand h, int su) { return (h >> (16 * su)) & 0x1FFF; };// Right-shift all bits by suit value, so that bits 0-12 are related to the suit we want. 
//Then, grab the first 13 bits using 0x1FFF which is first 13 bits all covered with 1.


std::optional<CardHand> stringToCardHand(std::string shorthand, std::vector<Card>&);