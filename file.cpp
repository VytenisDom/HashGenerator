#include<iostream>
#include<string>
#include<cmath>
#include<fstream>
#include<vector>
#include <ctime>
#include <chrono>
using namespace std;
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
double collisionCount = 0;
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
        cout<<"COLLISION ["<<input<<","<<lastInput<<"]"<<endl;
        collisionCount++;
    }
    // IivJa1yAZS,KgPfPABvaY
    lastHash = hash;
    lastInput = input;
    // cout<<"hash is : "<< hex << hash<<endl;
    for (int i = 1; i <= 8; i++) {
        hashArr[i-1] = hash * secret[pow(i, 4.75) / sqrt(i)];
        hashArr[i-1] = checkPrimaryHash(hash * secret[pow(i, 4.75) / sqrt(i)]);
    }

    for (int i = 0; i < 8; i++) {
        cout << hex << hashArr[i];
    }
    cout<<endl;
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

int main () {
    int selection;
    string input;
    string givenFileName;
    cout<<"Select Input Method"<<endl;
    cout<<"1: File input"<<endl;
    cout<<"2: Manual input"<<endl;
    cout<<"3: Constitution"<<endl;
    cout<<"4: Random Pairs"<<endl;
    cin>>selection;
    switch (selection){
        case 1: {
            cin.ignore();
            cout<<"Enter file name"<<endl;
            getline(cin, givenFileName);
            input = readFromFile(givenFileName);
            createHash(input, secret);
            break;
        }
        case 2: {
            cin.ignore();
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
            cout<<"Collision Percentage - "<<(collisionCount * 100) / 100000<<"%"<<endl;
            auto ending = std::chrono::steady_clock::now();
            cout<<"Done in : "<<std::chrono::duration <double, milli>(ending - start).count()<<" ms"<<endl;
            break;
        }
    }
}