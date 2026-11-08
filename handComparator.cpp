
#include <iostream>
#include <bit>
#include "basicFunctions.h"
#include "handComparator.h"


/*
Category is compared first. 
Then Major, which is the trips in a full house, or the top-pair in a two-pair. 
Then Minor, which are the kickers.

We need to export functions that take in cardhands and then returns result, but main.cpp doesn't necessarily have to touch the functions in here

*/

using Score = std::uint32_t;



constexpr Score makeScore(int cat, unsigned major, unsigned minor) { 
	return (Score(cat) << 26) | (major << 13) | minor; //Ends up being a 32-bit integer (easily representing the 2.5 million different hand combinations)
}


constexpr int topBit(unsigned m) {
	return std::bit_width(m) - 1; //bit_width finds the required no. of bits to hold value. This in-turn reveals the highest ranking card in cardSet.
}

constexpr unsigned keepTop(unsigned m, int n) {
	while (std::popcount(m) > n) m &= m - 1; //clear the loweset set bit. Used to clear our the irrelevant cards (such as counterfeit kickers)
	return m;
}

constexpr int straightHigh(unsigned m) {

	unsigned w = (m << 1) | ((m >> 12) & 1); //Left shift by 2^(1), therefore there is an extra bit at bit 0 that has value 0. Then fill that value with 1 if we have an ace at now bit 12
	//w becomes the lowest bit (Using the & 1)
	unsigned runs = w & (w >> 1) & (w >> 2) & (w >> 3) & (w >> 4); //Using AND , confirm the 5 values for the straight.
	return runs ? topBit(runs) + 3 : -1; //Return the top-bit of the straight (+ 3 for the rank

}



// evaluator.cpp, inside the anonymous namespace with topBit
const char* rankName(int r) {
	static constexpr const char* names[] = {
		"Two", "Three", "Four", "Five", "Six", "Seven", "Eight",
		"Nine", "Ten", "Jack", "Queen", "King", "Ace" };
	return names[r];
}

std::string plural(int r) {                  // "Kings", "Sixes"
	return r == 4 ? "Sixes" : std::string(rankName(r)) + "s";
}

std::string rankList(RankMask m) {           // "Ace, King, Seven"
	std::string out;
	while (m) {
		int r = topBit(m);
		if (!out.empty()) out += ", ";
		out += rankName(r);
		m &= ~(1u << r);                     // remove that rank, move to the next highest
	}
	return out;
}


// evaluator.cpp, outside the namespace
std::string readHandScore(Score s) {
	Category category = Category(s >> 26);       // top bits
	RankMask major{ (s >> 13) & 0x1FFF };        // middle 13 bits
	RankMask minor{ s & 0x1FFF };                // bottom 13 bits
	int top = topBit(major);                     // highest rank in the major field

	switch (category) {
	case HighCard:
		return "High card: " + rankList(major);
	case Pair:
		return "Pair of " + plural(top) + ", kickers " + rankList(minor);
	case TwoPair: {
		RankMask lowPair = major & ~(1u << top);
		return "Two pair, " + plural(top) + " and " + plural(topBit(lowPair))
			+ ", kicker " + rankList(minor);
	}
	case Trips:
		return "Three " + plural(top) + ", kickers " + rankList(minor);
	case Straight:
		return "Straight, " + std::string(rankName(top)) + " high";
	case Flush:
		return "Flush: " + rankList(major);
	case FullHouse:
		return "Full house, " + plural(top) + " full of " + plural(topBit(minor));
	case Quads:
		return "Four " + plural(top) + ", kicker " + rankList(minor);
	case StraightFlush:
		return top == 12 ? std::string("Royal flush")
			: "Straight flush, " + std::string(rankName(top)) + " high";
	}
	return "Invalid score";
}




Score evaluate(CardHand h) { //Returns score out of 2.5 million possible hands in a 32-bit integer

	unsigned s = suit_mask(h, 0), c = suit_mask(h, 1), d = suit_mask(h, 2), hh = suit_mask(h, 3);
	unsigned any = s | c | d | hh;                                     // rank appears at least once
	unsigned two = (s & c) | (s & d) | (s & hh) | (c & d) | (c & hh) | (d & hh);   // at least twice
	unsigned three = (s & c & d) | (s & c & hh) | (s & d & hh) | (c & d & hh);           // at least 3 times
	unsigned four = s & c & d & hh;

	for (unsigned m : {s, c, d, hh}) {
		if (std::popcount(m) >= 5) { //Checking if 5 of same suit
			int sf = straightHigh(m); //Checking if also a straight
			if (sf >= 0) return makeScore(StraightFlush, 1u << sf, 0); //Return the created score (out of 2.5 million scores), for straightflush, with major being 1 right shifted by the high card of straight
			return makeScore(Flush, keepTop(m, 5), 0); //Flush doesn't need a minor, the major is the top 5 cards.
		}
	}

	if (four) {
		return makeScore(Quads, four, keepTop(any & ~four, 1)); //Using suit_mask, if 4 within same suit, ITS QUADS. Major is the value found in four that exists in 4 suits. Minor is the kicker, which excludes the value within four from being included.
	}

	if (three) {
		unsigned t = 1u << topBit(three);
		unsigned pairs = two & ~t; //Check for pairs that exist that don't contain the value that is the trips.
		if (pairs) return makeScore(FullHouse, t, 1u << topBit(pairs)); //If found pairs, then FullHouse can be formed
	}
	
	if (int st = straightHigh(any);st >= 0) { //If a straight exists
		return makeScore(Straight, 1u << st, 0);
	}

	if (three) {
		unsigned t = 1u << topBit(three);
		return makeScore(Trips, t, keepTop(any & ~t, 2));
	}

	if (std::popcount(two) >= 2) { //remember std::popcount counts how many 1s are within the integer bitwise.
		unsigned p = keepTop(two, 2);            // best two of possibly three pairs
		return makeScore(TwoPair, p, keepTop(any & ~p, 1));
	}

	if (two)
		return makeScore(Pair, two, keepTop(any & ~two, 3));

	return makeScore(HighCard, keepTop(any, 5), 0);
}