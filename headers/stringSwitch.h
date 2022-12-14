#ifndef STRINGSWITCH_CLASS_H
#define STRINGSWITCH_CLASS_H

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

void split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
bool isFloat(std::string myString);
bool isNumber(const std::string &s);
// Reads a text file and outputs its contents to a string.
std::string getFileContents(const char* filename);
//void replaceAll(std::string& str, const std::string& from, const std::string& to);

#endif