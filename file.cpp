#include<iostream>
#include<string>
#include<cmath>
#include<fstream>
#include<vector>
using namespace std;
int checkPrimaryHash (int hash) {
    // cout<<"Hash correction active"<<endl;
    const int MINIMUM = 300000000;
    long long int MAXIMUM = 40000000000;
    while (abs(hash) < MINIMUM || abs(hash) > MAXIMUM) {
        // cout << hash <<endl;
        if (abs(hash) < MINIMUM) {
            hash *= 8.75;
        } else {
            hash /= 3.75;
        }
    }
    // cout << "RETURNING: " << hash << endl;
    return hash;
}
void createHash(string input, string secret) {
    int hash = 1024;
    int hashArr[8];
    cout<<"input size is : "<<input.size()<<endl;
    for (int i = 0; i < input.size(); i++) {
        hash += input[i] + pow(i * 2, 2);
    }
    hash = checkPrimaryHash(hash);
    cout<<"hash is : "<< hex << hash<<endl;
    for (int i = 1; i <= 8; i++) {
        // hashArr[i-1] = hash * secret[pow(i, 4.75) / sqrt(i)];
        hashArr[i-1] = checkPrimaryHash(hash * secret[pow(i, 4.75) / sqrt(i)]);
    }

    for (int i = 0; i < 8; i++) {
        cout << hashArr[i]<<endl;
    }
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
int main () {
    int selection;
    string input;
    string givenFileName;
    string secret = readFromFile("secretSalt");
    cout<<"Select Input Method"<<endl;
    cout<<"1: File input"<<endl;
    cout<<"2: Manual input"<<endl;
    cout<<"3: Constitution"<<endl;
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
            break;
        }
    }
}