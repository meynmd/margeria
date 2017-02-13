#include "tokenizer.h"

vector<string> tokenize( string s, set<char> separators, bool changeToLower ) {
	vector<string> tokens;
	string token;

	for (char c : s) {
		if (separators.find(c) == separators.end()) {
			if (changeToLower) {
				c = tolower(c);
			}
			token += c;
		} else {
			if (!token.empty()) {
				tokens.push_back(token);
				token = "";
			}
		}
	}
	if (!token.empty()) {
		tokens.push_back(token);
	}

	return tokens;
}
