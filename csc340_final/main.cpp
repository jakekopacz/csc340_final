#include "monte_carlo_poker.h"
#include "card.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


// convert time to int
int time_to_num(std::string& ctime_str) {

    int res;
    std::string num;

    for (int i = 0; i < ctime_str.size(); i++) {
        if (ctime_str.at(i) >= '0' && ctime_str.at(i) <= '9') {
            num.push_back(ctime_str.at(i));
        }
        if (num.size() >= 6) {
            break;
        }
    }

    res = std::stoi(num);
    return res;
}

void split_line(std::string& line, std::vector<std::string>& vec, char deli) {


    if (line.size() == 0) {
        return;
    }
    vec.clear();
    std::string temp;
    int start = 0;
    int index;
    int length;

    while (line.find(deli, start) != std::string::npos) {
        index = line.find(deli, start);
        length = index - start;
        temp = line.substr(start, length);
        vec.push_back(temp);
        start += length + 1;
    }

    length = line.size() - start;
    temp = line.substr(start, length);
    vec.push_back(temp);
}

int main(int argc, char* argv[]) {


    /* seed random with current time */
    time_t curr_time;
    time(&curr_time);
    std::string time_str(ctime(&curr_time));
    int seed_val = time_to_num(time_str);
    std::srand(seed_val);

    /* CREATE  MONTE CARLO OBJECT*/
    Monte sim;

    /* Itterations, PLAYERS HAND, COM CARDS */
    unsigned int itterations;
    unsigned int numComCards;
    unsigned int numPlayers;
    std::vector<Card*> com_cards;
    std::vector<std::vector<Card*> > playerHands;


    //get file name
    std::string fname(argv[1]);
    //open file
    std::ifstream myfile;
    myfile.open(fname);

    //temporary line holders
    std::string str;
    std::vector<std::string> vec;
    
    //get itterations
    getline(myfile, str);
    itterations = std::stoi(str);
    str.clear();
    //std::cout << itterations << std::endl;

    // get numComCards, numPlayers
    getline(myfile, str);
    split_line(str, vec, ',');
    numComCards = std::stoi(vec.at(0));
    numPlayers = std::stoi(vec.at(1));
    str.clear();
    //std::cout << numComCards << ", " << numPlayers << std::endl;

    // get com_cards
    getline(myfile, str);
    split_line(str, vec, ',');
    for (int i = 0; i < numComCards; i++) {
        //std::cout << vec.at(i) << std::endl;
        com_cards.push_back(new Card(std::stoi(vec.at(i))));
    }

    // get playerHands
    std::vector<Card*> temp;
    for (int i = 0; i < numPlayers; i++) {

        temp.clear();
        getline(myfile, str);
        split_line(str, vec, ',');

        if (vec.size() != 2) {
            temp.push_back(nullptr);
            temp.push_back(nullptr);
            playerHands.push_back(temp);
        }
        else {
            temp.push_back(new Card(std::stoi(vec.at(0))));
            temp.push_back(new Card(std::stoi(vec.at(1))));
            playerHands.push_back(temp);
        }

    }
    //close file
    myfile.close();

    sim.sim(playerHands, com_cards, numComCards, numPlayers, itterations, NULL);
    
    for  (int i = 0; i < playerHands.size(); i++) {
        
        if (playerHands.at(i).at(0)) {
            delete playerHands.at(i).at(0);
        }
        if (playerHands.at(i).at(1)) {
            delete playerHands.at(i).at(1);
        }
    }

    for (int i = 0; i < com_cards.size(); i++) {

        if (com_cards.at(i)) {
            delete com_cards.at(i);
        }
    }

    return 0;
}
