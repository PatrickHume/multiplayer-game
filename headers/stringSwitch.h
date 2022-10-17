#ifndef STRINGSWITCH_CLASS_H
#define STRINGSWITCH_CLASS_H

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

void split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
bool isFloat(std::string myString);
bool isNumber(const std::string &s);
//void replaceAll(std::string& str, const std::string& from, const std::string& to);

#endif