#include "randomiser.h"


std::vector<Card> makeDeck() {
    std::vector<Card> deck;
    deck.reserve(52);
    for (int su = 0; su < 4; ++su)
        for (int r = 0; r < 13; ++r)
            deck.push_back(static_cast<Card>(su * 16 + r));
    return deck;
}