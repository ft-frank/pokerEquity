#include "basicFunctions.h"




#include <optional>
#include <string>
#include <string_view>

std::optional<Card> parseCard(std::string_view s) {
	constexpr std::string_view ranks = "23456789TJQKA";
	constexpr std::string_view suits = "scdh";

	const auto r = ranks.find(s[0]);
	const auto su = suits.find(s[1]);

	if (r == std::string_view::npos || su == std::string_view::npos) {
		return std::nullopt;
	}

	return static_cast<Card>(su * 16 + r);
}

std::optional<CardHand> stringToCardHand(std::string shorthand, std::vector<Card>& deck) {

	if (shorthand.length() != 4) {
		return std::nullopt;
	}

	const std::string_view view = shorthand;
	const auto c1 = parseCard(view.substr(0, 2));
	const auto c2 = parseCard(view.substr(2, 2));

	if (!c1 || !c2 || *c1 == *c2) {
		return std::nullopt;
	}
	
	if (std::find(deck.begin(), deck.end(), *c1) == deck.end()) { //checks if card is found in deck
		return std::nullopt;
	}

	if (std::find(deck.begin(), deck.end(), *c2) == deck.end()) { //checks if card2 is found in the deck
		return std::nullopt;
	}

	CardHand hand = bit(*c1) | bit(*c2);

	return hand;
}