#include <iostream>
#include<stack>
#include<string>
#include<fstream>
#include<bits/stdc++.h>
#include<vector>
#include<map>

using namespace std;

class TopDownParser {
private:
    char rootState;
    /* pointer to input/ output files */
    ifstream fin; // reads CFG
    ofstream fout; // outputs the result

    /* map that memorizes grammar transitions */
    map<char, vector<string>> transitionsMap;

    /* {neterminal, its number} -> production number*/
    map<pair<char, int>, int> mapProductionsCode;
    const int CONSTANTA_TERMINAL = -1;

    string word;
public:
    void read() {
        int countTransition = 1;
        string line, item;
        char neterminal;

        /* w-ul */
        getline(fin, word);

        getline(fin, line);
        istringstream iss(line);
        iss >> neterminal;
        rootState = neterminal;

        for (int currCount = 0; iss >> item; currCount++) {
            transitionsMap[neterminal].push_back(item);
            mapProductionsCode[{neterminal, currCount}] = countTransition++;
        }

        while (getline(fin, line)) {
            istringstream iss(line);
            iss >> neterminal;

            int currCount;

            if (transitionsMap.find(neterminal) == transitionsMap.end()) {
                transitionsMap[neterminal] = vector<string>();
                currCount = 0;
            } else {
                currCount = transitionsMap[neterminal].size();
            }

            for (; iss >> item; currCount++) {
                transitionsMap[neterminal].push_back(item);
                mapProductionsCode[{neterminal, currCount}] = countTransition++;
            }
        }
    }

    string constructTree() {
        if (word.empty()) {
            cout << "The word is empty!";
            return "";
        }

        int n = word.length();
        int indexWord = 0;
        word += "$";

        /* character for the current state {q,r,t,e} */
        char state = 'q';

        /* stacks for the path and currConfiguration */
        stack<pair<char, int>> stackPath;
        stack<char> stackCurrConfig;

        /* bottom of the stack and the root */
        stackCurrConfig.push('$');
        stackCurrConfig.push(rootState);

        while (true) {
            switch (state) {
                case 'q': {
                    // preiau din varful stivei primul caracter, verific daca e terminal sau nu
                    char top = stackCurrConfig.top();
                    if (terminal(top) && indexWord < n && word[indexWord] == top) {
                        // conditia 2
                        indexWord++;
                        stackPath.push({top, CONSTANTA_TERMINAL});
                        stackCurrConfig.pop();
                    } else if (terminal(top) && indexWord < n + 1 && word[indexWord] != top) {
                        // conditia 3
                        state = 'r';
                    } else if (indexWord == n && top == '$') {
                        // conditia 4
                        state = 't';
                    } else {
                        // conditia 1
                        stackPath.push({top, 0});
                        stackCurrConfig.pop();
                        addStringToStackReverse(stackCurrConfig, transitionsMap[top][0]);
                    }
                    break;
                }
                case 'r': {
                    pair<char, int> topStackPath = stackPath.top();
                    if (topStackPath.second == CONSTANTA_TERMINAL) {
                        // cazul 5
                        indexWord--;
                        stackPath.pop();
                        stackCurrConfig.push(topStackPath.first);
                    } else { // cazurile 6
                        if (topStackPath.second + 1 < transitionsMap[topStackPath.first].size()) {
                            //cazul 6a
                            string transactionDr = transitionsMap[topStackPath.first][topStackPath.second + 1];

                            removeFirstNCharsFromStack(stackCurrConfig,
                                                       transitionsMap[topStackPath.first][topStackPath.second].length());
                            stackPath.pop();

                            addStringToStackReverse(stackCurrConfig, transactionDr);
                            stackPath.push({topStackPath.first, topStackPath.second + 1});

                            state = 'q';
                        } else if (indexWord == 0 && topStackPath.first == rootState &&
                                   topStackPath.second + 1 >= transitionsMap[topStackPath.first].size()) {
                            // cazul 6b
                            state = 'e';
                            indexWord = n + 1;
                            while (!stackCurrConfig.empty()) {
                                stackCurrConfig.pop();
                            }
                        } else {
                            // cazul 6c
                            int len = transitionsMap[topStackPath.first][topStackPath.second].length();
                            removeFirstNCharsFromStack(stackCurrConfig, len);
                            stackPath.pop();
                            stackCurrConfig.push(topStackPath.first);
                        }
                    }
                    break;
                }
                case 't':
                    return generateAnalysis(stackPath);
                case 'e':
                    return "EROARE!";
            }
        }

    }

    static void addStringToStackReverse(stack<char> &myStack, string myString) {
        int size = myString.length();
        for (int i = size - 1; i >= 0; i--) {
            myStack.push(myString[i]);
        }
    }

    static void removeFirstNCharsFromStack(stack<char> &myStack, int n) {
        while (!myStack.empty() && n--) {
            myStack.pop();
        }
    }

    string generateAnalysis(stack<pair<char, int>> stack) {
        string result;
        while (!stack.empty()) {
            pair<char, int> top = stack.top();
            stack.pop();

            if (!terminal(top.first)) {
                int transitionNumber = mapProductionsCode[top];
                result = to_string(transitionNumber) + result;
            }
        }
        return result;
    }


    TopDownParser() = default;

    TopDownParser(const string &fileInputName, const string &fileOutputName) {
        fin.open(fileInputName);
        fout.open(fileOutputName);
    }

    ~TopDownParser() {
        fin.close();
        fout.close();
    }

    static bool terminal(char top) {
        return !isupper(top);
    }
};

int main() {
    TopDownParser topDownParser("input.in", "output.out");
    topDownParser.read();
    string result = topDownParser.constructTree();
    cout << result;
    return 0;
}
