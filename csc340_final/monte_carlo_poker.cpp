#include "monte_carlo_poker.h"
#include "card.h"
#include "deck.h"
#include "hand_eval.h"
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <iomanip>



Monte::Monte() { this->eval = new HandEval; }

Monte::~Monte() { delete this->eval; }

void Monte::sim(std::vector<std::vector<Card*> >& playerHands, std::vector<Card*>& com, unsigned int nc, unsigned int np, unsigned int it, std::vector<std::vector<int> >* results) {

    // get variables
    this->n_com_cards = nc;
    this->n_players = np;
    this->sim_iterations = it;

    // create new coppies of cards and store, dont want to modify parameters passed
    this->com_cards = std::vector<Card*>(7, nullptr);
    this->players = std::vector<std::vector<Card*> >(n_players, std::vector<Card*>(p_hand_size, nullptr));

    make_vectors(playerHands, com);

    // add static cards to set
    add_static_com_cards();
    add_static_player_cards();

    // make deck without static cards
    int sz = static_cards.size();
    this->sim_deck = new Deck(static_cards,sz);


    run_sim(results);

    // delete static cards in player_hands & com_cards
    delete_vectors();   

    // delete deck
    delete this->sim_deck;

    return;
}


void Monte::run_sim(std::vector<std::vector<int> >* results) {

    // score.at(i).at(0) == player wins
    // score.at(i).at(1) == player splits
    std::vector<std::vector<int> > score(this->n_players, std::vector<int>(2, 0));
    std::vector<int> winners;

    for (int i = 0; i < this->sim_iterations; i++) {

        winners.clear();
        run_round(winners);

        if (winners.empty()) {
            std::cout << "Error" << std::endl;
            return;
        }
        else if (winners.size() == 1) {

            score.at(winners.at(0)).at(0)++;
        }
        else {
            
            for (int j = 0; j < winners.size(); j++) {

                score.at(winners.at(j)).at(1)++;
            }
            
        }
    }

    if (results) {
        (*results) = score;
    }
    else {
        print_res(score);
    }
    return;
}


void Monte::run_round(std::vector<int>& winners) {

    std::vector<int> ranks;

    // deal deck to nonstatic players & com cards
    deal_deck();

    int score;
    //players.at(1).at(0)->displayCard();
    for (int i = 0; i < this->n_players; i++) {

        this->com_cards.at(5) = this->players.at(i).at(0);
        this->com_cards.at(6) = this->players.at(i).at(1);

        // evaluate 7 card hand of every player
        score = eval->eval(com_cards);
        // add to ranks
        ranks.push_back(score);
    }
    // clears com cards & nonstatic player's hands hand
    clear_hands();



    // determine winner(s)
    int best = 21474836;
    int p;
    for (int i = 0; i < this->n_players; i++) {
        if (ranks.at(i) < best) {
            best = ranks.at(i);
            winners = {i};
        }
        else if (ranks.at(i) == best) {
            winners.push_back(i);
        }
    }
    return;
}

// good
void Monte::make_vectors(std::vector<std::vector<Card*> >& playersHands, std::vector<Card*>& com) {

    int card_num;

    // add com cards
    for (int i = 0; i < this->n_com_cards; i++) {
        card_num = com.at(i)->ID;
        this->com_cards.at(i) = new Card(card_num);
    }

    //std::cout << "COMCARDS ADDED" << std::endl;

    // add players cards
    for (int i = 0; i < this->n_players; i++) {

        for (int j = 0; j < this->p_hand_size; j++) {

            if (playersHands.at(i).at(j)) {
                card_num = playersHands.at(i).at(j)->ID;
                this->players.at(i).at(j) = new Card(card_num);
            }
        }
    }
}

// good
void Monte::delete_vectors() {

    for (int i = 0; i < 7; i++) {
        if (com_cards.at(i)) {
            delete this->com_cards.at(i);
        }
    }

    for (int i = 0; i < this->n_players; i++) {
        //std::cout << i << " ";
        for (int j = 0; j < this->p_hand_size; j++) {
            if (this->players.at(i).at(j)) {
                delete this->players.at(i).at(j);
            }
        }
    }

    this->static_cards.clear();
}


void Monte::add_static_com_cards() {

    int card_num;
    for (int i = 0; i < this->n_com_cards; i++ ) {
        card_num = this->com_cards.at(i)->ID;
        this->static_cards.emplace(card_num);
    }

}


void Monte::add_static_player_cards() {

    if (this->players.empty()) {
        std::cout << "No players entered" << std::endl;
        return;
    }

    int card_num;
    for (int i = 0; i < this->n_players; i++) {

        for (int j = 0; j < this->p_hand_size; j++) {

            if (this->players.at(i).at(j)) {
                card_num = this->players.at(i).at(j)->ID;
                this->static_cards.emplace(card_num);
            }
        }
    }
}


void Monte::deal_deck() {

    sim_deck->shuffleDeck();

    for (int i = 0; i < this->p_hand_size; i++) {
        
        for (int j = 0; j < this->n_players; j++) {

            if (this->players.at(j).at(i) == nullptr) {
                this->players.at(j).at(i) = this->sim_deck->deal();
            }
            //std::cout << "Player: " << j+1 << " gets ";
            //this->players.at(j).at(i)->displayCard();
        }
    }

    for (int i = this->n_com_cards; i < 5; i++) {
        this->com_cards.at(i) = this->sim_deck->deal();
        //std::cout << "ComCard: ";
        //this->com_cards.at(i)->displayCard();
    }
}


void Monte::clear_hands() {

    // clear nonstatic com_cards
    for (int i = this->n_com_cards; i < 7; i++) {
        this->com_cards.at(i) = nullptr;
    }

    // clear nonstatic player_hands
    for (int i = 0; i < this->n_players; i++) {
        for (int j = 0; j < this->p_hand_size; j++) {
            
            if (this->static_cards.count(this->players.at(i).at(j)->ID)) { continue; }
            this->players.at(i).at(j) = nullptr;
        }
    }
    
    
}


void Monte::print_res(std::vector<std::vector<int> >& score) {

    std::cout << "***** OUT OF " << this->sim_iterations << " GAMES *****" << std::endl;
    std::cout << std::endl;

    for (int i = 0; i < this->n_players; i++) {
        double d1 = score.at(i).at(0);
        double d2 = score.at(i).at(1);
        std::cout << "Player " << (i+1) << " Wins:   " << score.at(i).at(0) << " --- " << ((d1 / this->sim_iterations) * 100) << "%" <<  std::endl;
        std::cout << "Player " << (i+1) << " Splits: " << score.at(i).at(1) << " --- " << ((d2 / this->sim_iterations) * 100) << "%" <<  std::endl;
        std::cout << std::endl;
    }

    std::cout << std::endl;
}
