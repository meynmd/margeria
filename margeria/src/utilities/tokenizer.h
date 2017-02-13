#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <set>
#include <string>

using namespace std;

vector<string> tokenize(string s, set<char> separators, bool changeToLower);


#endif
