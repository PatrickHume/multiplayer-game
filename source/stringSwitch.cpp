#include "../headers/stringSwitch.h"

//Commands
//--------
//SUMMON
//SHOW
//REMOVE
//ADD
//END
//-----------
//NOT_DEFINED

// Map to associate the strings with the enum values

//https://stackoverflow.com/questions/39050225/extract-individual-words-from-string-c
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

