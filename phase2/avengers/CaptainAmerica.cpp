#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<string> fullJustify(vector<string>& words, int maxWidth) {
    int n = words.size();
    vector<string> print;
    int i = 0;
    while (i < n)
    {
        vector<string> thisLineWords(1, words[i]);
        string thisLine = words[i++];
        while (i < n && thisLine.length() + words[i].length() < maxWidth) {
            thisLineWords.push_back(words[i]);
            thisLine += " " + words[i++];
        }
        if (i < n) goto addWords;
        lastLine:
            while(thisLine.length() < maxWidth) thisLine += " ";
            print.push_back(thisLine);
            continue;
        addWords:
            int rem = thisLineWords.size() - 1;
            int c = maxWidth - thisLine.size() + rem;
            if (rem == 0) goto lastLine;
            thisLine.clear();
            for (int j = 0; j < rem; j++)
            {
                thisLine += thisLineWords[j];
                for (int k = 0; k < c/rem; k++) thisLine += ' ';
                if (j < c % rem) thisLine += ' ';
            }
            thisLine += thisLineWords[rem];
            print.push_back(thisLine);
    }
    return print;
}
