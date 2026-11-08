
//Cards can be shared between hole cards
//Need to include up to 9 hands.no


#include "randomiser.h"
#include "handComparator.h"
#include "basicFunctions.h"

#include <iostream>
#include <string>
#include <cstdint>
#include <bit>
#include <random>
#include <optional>

using Card = std::uint8_t; //Card represented by (suit * 16 + rank). Rank 0 = 2, Rank 12 = A.
using CardHand = std::uint64_t; //
using randomEngine = std::mt19937;


char bitRankToRank(int r) {
    if (r <= 7) return '0' + (r + 2);   // 0–7 → '2'–'9'
    switch (r) {
    case 8:  return 'T';
    case 9:  return 'J';
    case 10: return 'Q';
    case 11: return 'K';
    case 12: return 'A';
    default: return '?';
    }
}

char bitSuitToSuit(int s) {
    switch (s) {
    case 0: return 's';
    case 1: return 'c';
    case 2: return 'd';
    case 3: return 'h';
    default: return '?';
    }
}

std::string CardToString(Card c) {
    return { bitRankToRank(rank(c)), bitSuitToSuit(suit(c)) };   // builds "Ah", "Tc", ...
}

std::string allCardsInSet(CardHand hand) {

    std::string cards{ "" };

    for (int i = 0; i < 4; i++) { //For every suit

        for (int j = 0; j <= 12; j++) { //For every rank 0-12
            if (hand & bit(i * 16 + j)) {
                   
                cards += CardToString(i * 16 + j);
            }

            }
    }
    return cards;
}

void addToHand(Card c, CardHand& hand) {
    hand |= bit(c);
}

void removeFromHand(Card c, CardHand& hand) {
    hand &= ~bit(c);    // ~ flips every bit, so this clears only c's bit
}





struct equities {

    double h1;
    double h2;
    double tie;
};

//add in STD::INITIALISER_LIST for an unlimited number of hands to calculate equity simply.
//I have to have a REMOVE FROM HAND.
//I have to remember how integer and float mathematics works in C++ or else im cooked
//


void removeCardHandFromDeck(CardHand hand, std::vector<Card>& deck) {

    for (size_t index = 0; index < deck.size(); ) {
        if (bit(deck[index]) & hand) {
            deck.erase(deck.begin() + index);
        }
        else {
            index++;
        }
    }

}



equities calculateEquity(CardHand hand1, CardHand hand2, std::vector<Card> deck, randomEngine& gen) {
 
    int hand1Win{ 0 };
    int hand2Win{ 0 };
    int tie{ 0 };
    

       
    for (int i = 0; i < 100000; i++) {

        std::shuffle(deck.begin(), deck.end(), gen); //shuffle deck

        //Need to minus 2 for each hand played, but for now 2 hands we minus 4

       
        Card f1 = deck.back(); deck.pop_back(); //A card
        Card f2 = deck.back(); deck.pop_back();
        Card f3 = deck.back(); deck.pop_back(); //A card

        Card t1 = deck.back(); deck.pop_back();
        Card r1 = deck.back(); deck.pop_back(); //A card

        addToHand(f1, hand1);
        addToHand(f2, hand1);
        addToHand(f3, hand1);
        addToHand(t1, hand1);
        addToHand(r1, hand1);

        addToHand(f1, hand2);
        addToHand(f2, hand2);
        addToHand(f3, hand2);
        addToHand(t1, hand2);
        addToHand(r1, hand2);


        Score hand1Score = evaluate(hand1);
        Score hand2Score = evaluate(hand2);

        if (hand1Score > hand2Score) {
            hand1Win++;
        }
        else if (hand2Score > hand1Score) {
            hand2Win++;

        }
        else {
            tie++;
        }

        //RESET DECK

        removeFromHand(f1, hand1);
        removeFromHand(f2, hand1);
        removeFromHand(f3, hand1);
        removeFromHand(t1, hand1);
        removeFromHand(r1, hand1);

        removeFromHand(f1, hand2);
        removeFromHand(f2, hand2);
        removeFromHand(f3, hand2);
        removeFromHand(t1, hand2);
        removeFromHand(r1, hand2);


        deck.push_back(f1);
        deck.push_back(f2);
        deck.push_back(f3);
        deck.push_back(t1);
        deck.push_back(r1);

    }
    
    double h1P = static_cast<double>(hand1Win )/ 100000;
    double h2P = static_cast<double>(hand2Win) / 100000;
    double tP = static_cast<double>(tie) / 100000;


    return { h1P, h2P, tP };


}


std::optional<CardHand> inputHoleCards(int x, std::vector<Card>& deck) {
    std::optional<CardHand> hand;
    std::string shorthand;


    while (!hand) {
        std::cout << "Enter hand " << x << ":";
        std::cin >> shorthand;
        std::cout << std::endl;

        hand = stringToCardHand(shorthand, deck);
        if (!hand) {
            std::cout << "Invalid hand, try again.\n";
        }

    }


    return *hand;

}


// next step is to allow the input HOle Cards to work, and then remove them from the deck to calculate equities accurately




int main() {

    randomEngine gen{ std::random_device{}() };

    std::vector<Card> deck = makeDeck(); //A vector of the deck containing all remaining cards 0->12, 15-> 27 etc.

    std::shuffle(deck.begin(), deck.end(), gen);   // random order, no repeats

    int i = 0;

    std::optional<CardHand> result = inputHoleCards(i, deck);
    i += 1;
    CardHand hand = *result;
       //need to remove cards here not at calculate equity lah
    removeCardHandFromDeck(hand, deck);
    
    std::optional<CardHand> result1 = inputHoleCards(i, deck);
  
    CardHand hand2 = *result1;

    removeCardHandFromDeck(hand2, deck);






    equities Equity = calculateEquity(hand, hand2, deck, gen);

    std::cout << "Hand 1: " << Equity.h1 << " " << allCardsInSet(hand) << '\n'
        << "Hand 2: " << allCardsInSet(hand2) << " " << Equity.h2 << '\n'
        << "Tie: " << Equity.tie << '\n';
    
    

}


//Next-step is to make multiple hands inputtable
//Then next-step after that is to runout every single possible runout instead of randomly
//Then next-step after that is calculating uswing 