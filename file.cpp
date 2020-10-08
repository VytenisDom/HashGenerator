#include<iostream>
#include<string>
#include<cmath>
#include<fstream>
#include<vector>
#include <ctime>
#include <chrono>
#include <sstream>
using namespace std;

string hexCharToBinStr (char c)
{
    switch(c)
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'a': return "1010";
        case 'b': return "1011";
        case 'c': return "1100";
        case 'd': return "1101";
        case 'e': return "1110";
        case 'f': return "1111";
    }
    return "0000";
}

string getBinStrFromHexStr (string hex) {
    string binStr = "";
    for (int i = 0; i < hex.size(); i++) {
        binStr += hexCharToBinStr(hex[i]);
    }
    return binStr;
}

long int checkPrimaryHash (long int hash) {
    // cout<<"Hash correction active"<<endl;
    const int MINIMUM = 300000000;
    // -2147483648
    long long int MAXIMUM = 40000000000;
    // cout<<abs(hash)<<" "<< MINIMUM<< ", "<<abs(hash)<<" "<<MAXIMUM<<endl;
    while (abs(hash) < MINIMUM || abs(hash) > MAXIMUM) {
        // cout << hash <<endl;
        if (abs(hash) < MINIMUM) {
            if (abs(hash) < MINIMUM / 2) {
                hash+=MINIMUM;
            }
            hash *= 2.75;
            if(hash == -2147483648) break;
        } else if (abs(hash) > MAXIMUM) {
            hash /= 3.75;
        }
    }
    // cout << "RETURNING: " << hash << endl;
    return hash;
}
long int lastHash = 1024;
string lastInput = "";
string lastFinalHex = "";

double collisionCount = 0;
double comparisonCount = 0;
double binSumOfSimilarityPercentages = 0;
double sumOfSimilarityPercentages = 0;
double minSimilarityPercentage = 100;
double maxSimilarityPercentage = 0;
void createHash(string input, string secret) {
    long int hash = 1024;
    long int hashArr[8];
    // cout<<input<<endl;
    // cout<<"input size is : "<<input.size()<<endl;
    for (int i = 0; i < input.size(); i++) {
        hash += (input[i] * input[i]) + pow(i * 2, 2);
    }
    hash = checkPrimaryHash(hash);
    if (hash == lastHash) {
        // cout<<"COLLISION ["<<input<<","<<lastInput<<"]"<<endl;
        collisionCount++;
    }
    lastHash = hash;
    lastInput = input;
    // cout<<"hash is : "<< hex << hash<<endl;
    for (int i = 1; i <= 8; i++) {
        hashArr[i-1] = hash * secret[pow(i, 4.75) / sqrt(i)];
        hashArr[i-1] = checkPrimaryHash(hash * secret[pow(i, 4.75) / sqrt(i)]);
    }

    string finalHex = "";
    for (int i = 0; i < 8; i++) {
        // cout << hex << hashArr[i];

        stringstream stream;
        stream << hex << hashArr[i];
        string result( stream.str() );
        finalHex += result;
    }
    // cout<<endl;

    string finalBin = getBinStrFromHexStr(finalHex);
    string lastFinalBin = getBinStrFromHexStr(lastFinalHex);
    
    double similarBin = 0;
    for (int i = 0; i < finalBin.size(); i++) {
        if (finalBin[i] == lastFinalBin[i]) {
            similarBin++;
        }
    }
    double binSimilarityPercentage = (similarBin / finalBin.size()) * 100;
    binSumOfSimilarityPercentages += binSimilarityPercentage;

    double similarHex = 0;
    for (int i = 0; i < finalHex.size(); i++) {
        if (finalHex[i] == lastFinalHex[i]) {
            similarHex++;
        }
    }
    double similarityPercentage = (similarHex / finalHex.size()) * 100;
    sumOfSimilarityPercentages += similarityPercentage;
    comparisonCount++;
    if (similarityPercentage > maxSimilarityPercentage) {
        maxSimilarityPercentage = similarityPercentage;
    } else if (similarityPercentage < minSimilarityPercentage) {
        minSimilarityPercentage = similarityPercentage;
    }
    // cout << "Similarity on hex level is - " << similarityPercentage << endl;
    lastFinalHex = finalHex;
}

string readFromFile(string fileName) {
    string finalString = "";
    string word;
    vector<string> words;
    ifstream infile (fileName);
    for (int i = 0; ;i++) {
        if (infile.eof()) {
            if (!i) {
                finalString = "";
            }
            break;
        }
        infile>>word;
        words.push_back(word);
    }
    for (int i = 0; i < words.size(); i++) {
        finalString += words[i];
    }
    return finalString;
}
string secret = readFromFile("secretSalt");

void readFromFileByLine (string fileName) {
    ifstream infile (fileName);
    string line = "as";
    
    for (int i = 0; ;i++) {
        if (infile.eof()) break;
        getline(infile, line);
        createHash(line, secret);
    }
}

int main (int argc, char *argv[]) {
    int selection = atoi(argv[1]);
    string input;
    string givenFileName;
    
    switch (selection){
        case 0: {
            cout<<"Select Input Method via CommandLine Argument"<<endl;
            cout<<"1: File input"<<endl;
            cout<<"2: Manual input"<<endl;
            cout<<"3: Constitution"<<endl;
            cout<<"4: Random Pairs"<<endl;
            cout<<"5: Similar Pairs"<<endl;
        }
        case 1: {
            cout<<"Enter file name"<<endl;
            getline(cin, givenFileName);
            input = readFromFile(givenFileName);
            createHash(input, secret);
            break;
        }
        case 2: {
            cout<<"Enter any string"<<endl;
            getline(cin, input);
            createHash(input, secret);
            break;
        }
        case 3: {
            auto start = std::chrono::steady_clock::now();
            readFromFileByLine("konstitucija.txt");
            auto ending = std::chrono::steady_clock::now();
            cout<<"Done in : "<<std::chrono::duration <double, milli>(ending - start).count()<<" ms"<<endl;
            break;
        }
        case 4: {
            auto start = std::chrono::steady_clock::now();
            readFromFileByLine("pairs.txt");
            cout << "Collision Percentage - " << (collisionCount * 100) / 100000 << "%" << endl;
            cout << "Similarity Percentage Average - " << sumOfSimilarityPercentages / comparisonCount << "%" << endl;
            cout << "Similarity Percentage Average (Binary-level) - " << binSumOfSimilarityPercentages / comparisonCount << "%" << endl;
            cout << "Lowest Similarity Percentage - " << minSimilarityPercentage << "%" << endl;
            cout << "Highest Similarity Percentage - " << maxSimilarityPercentage << "%" << endl;
            auto ending = std::chrono::steady_clock::now();
            cout<<"Done in : "<<std::chrono::duration <double, milli>(ending - start).count()<<" ms"<<endl;
            break;
        }
        case 5: {
            auto start = std::chrono::steady_clock::now();
            readFromFileByLine("similarPairs.txt");
            cout << "Collision Percentage - " << (collisionCount * 100) / 100000 << "%" << endl;
            cout << "Similarity Percentage Average - " << sumOfSimilarityPercentages / comparisonCount << "%" << endl;
            cout << "Similarity Percentage Average (Binary-level) - " << binSumOfSimilarityPercentages / comparisonCount << "%" << endl;
            cout << "Lowest Similarity Percentage - " << minSimilarityPercentage << "%" << endl;
            cout << "Highest Similarity Percentage - " << maxSimilarityPercentage << "%" << endl;
            auto ending = std::chrono::steady_clock::now();
            cout<<"Done in : "<<std::chrono::duration <double, milli>(ending - start).count()<<" ms"<<endl;
            break;
        }
    }
}