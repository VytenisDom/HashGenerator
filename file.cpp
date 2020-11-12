#include<iostream>
#include<string>
#include<cmath>
#include<fstream>
#include<vector>
#include <ctime>
#include <chrono>
#include <sstream>
using namespace std;

class user {
    public:
        int firstName;
        string publicKey;
        int balance;
};

class transaction {
    public:
        string transactionID;
        string senderPublicKey;
        string receiverPublicKey;
        int sum;
};

class block {
    public:
        //Header
        string prevBlockHash;
        string timestamp;
        string version = "v0.2";
        string merkelRootHash;
        int nonce = 0;
        int DiffTarget = 2;
        //Body
        vector<transaction> tx;
};

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

string IntToString(int a)
{
    ostringstream temp;
    temp << a;
    return temp.str();
}

string reverseString(string str) 
{ 
    int n = str.length(); 
  
    for (int i = 0; i < n / 2; i++) {
        swap(str[i], str[n - i - 1]); 
    }
    return str;
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

const int TRANSACTIONS_PER_BLOCK = 100;
string createHash(string input, string secret) {
    long int hash = 1024;
    long int hashArr[8];
    for (int i = 0; i < input.size(); i++) {
        hash += (input[i] * input[i]) + pow(i * 2, 2);
    }
    hash = checkPrimaryHash(hash);
    if (hash == lastHash) {
        collisionCount++;
    }
    lastHash = hash;
    lastInput = input;
    for (int i = 1; i <= 8; i++) {
        hashArr[i-1] = hash * secret[pow(i, 4.75) / sqrt(i)];
        hashArr[i-1] = checkPrimaryHash(hash * secret[pow(i, 4.75) / sqrt(i)]);
    }

    string finalHex = "";
    for (int i = 0; i < 8; i++) {
        // cout << hex << hashArr[i];
        cout << hex;

        stringstream stream;
        stream << hex << hashArr[i];
        string result( stream.str() );
        finalHex += reverseString(result);
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
    lastFinalHex = finalHex;
    return finalHex;
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

void generateNewUsers (vector<user> &u, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++){
        u.push_back(user());
        u[i].firstName = i;
        u[i].publicKey = createHash(IntToString(rand()), secret);
        u[i].balance = 99; // FOR TESTING PURPOSES
        //100 + rand() % 1000000;
        // cout << u[i].firstName << " " <<u[i].publicKey << " " << u[i].balance << endl;
    }
}

void generateNewTransactions(vector<user> &u, vector<transaction> &tx, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++){
        tx.push_back(transaction());
        tx[i].senderPublicKey = u[rand() % 1000].publicKey;
        tx[i].receiverPublicKey = u[rand() % 1000].publicKey;
        tx[i].sum = 50; // FOR TESTING PURPOSES
        // string txInput = tx[i].receiverPublicKey + tx[i].senderPublicKey + IntToString(tx[i].sum);
        // tx[i].transactionID = createHash(txInput, secret);
        // cout << u[i].firstName << " " <<u[i].publicKey << " " << u[i].balance << endl;
    }
}

void generateNewTransactionIDs(vector<transaction> &tx) {
    srand(time(NULL));
    for (int i = 0; i < tx.size(); i++){
        string txInput = tx[i].receiverPublicKey + tx[i].senderPublicKey + IntToString(tx[i].sum);
        tx[i].transactionID = createHash(txInput, secret);
    }
}

void validatePool(vector<transaction> &pool, vector<user> &u) {
    for (int i = 0; i < pool.size(); i++) {
        // TxID hash validation
        // string txInput = pool[i].receiverPublicKey + pool[i].senderPublicKey + IntToString(pool[i].sum);
        // // cout << pool[i].transactionID << " : "<<endl << createHash(txInput, secret) << endl;
        // if (i == 0) {
        //     cout<<txInput<<endl;
        // }
        // if (pool[i].transactionID != createHash(txInput, secret)) {
        //     pool.erase(pool.begin() + i);
        //     i = 0;
        // }

        // User balance validation
        for (int j = 0; j < u.size(); j++) {
            if (pool[i].senderPublicKey == u[j].publicKey && pool[i].sum > u[j].balance) {
                pool.erase(pool.begin() + i);
                i = 0;
            }
        }
        

    }
} 

void addTransactionsToPool(vector<transaction> &pool, vector<transaction> &tx) {
    for (int i = 0; i < tx.size(); i++){
        pool.push_back(tx[i]);
    }
}

void addTransactionsToBlock(vector<block> &blocks, vector<transaction> &pool, int blockNum) {
    blocks.push_back(block());
    int numOfTransactions = TRANSACTIONS_PER_BLOCK;
    if (TRANSACTIONS_PER_BLOCK > pool.size()) {
        numOfTransactions = pool.size();
    }
    for (int i = 0; i < numOfTransactions; i++){
        blocks[blockNum].tx.push_back(pool[i]);
    }
}

void makeTransactions(vector<user> &u, vector<transaction> &pool) {
    int numOfTransactions = TRANSACTIONS_PER_BLOCK;
    if (TRANSACTIONS_PER_BLOCK > pool.size()) {
        numOfTransactions = pool.size();
    }
    for (int i = 0; i < numOfTransactions; i++) {
        for (int j = 0; j < u.size(); j++) {
            if (u[j].publicKey == pool[i].senderPublicKey) {
                u[j].balance -= pool[i].sum;
            }
            if (u[j].publicKey == pool[i].receiverPublicKey) {
                u[j].balance += pool[i].sum;
            }
        }
    }

    // for (int j = 0; j < u.size(); j++) {
    //     cout << dec << u[j].balance << hex << endl;
    // }
}

void removeTransactionsFromPool(vector<transaction> &pool) {
    int numOfTransactions = TRANSACTIONS_PER_BLOCK;
    if (TRANSACTIONS_PER_BLOCK > pool.size()) {
        numOfTransactions = pool.size();
    }
    pool.erase(pool.begin(), pool.begin() + numOfTransactions);
}

bool hashMeetsRequirements (vector<block> &blocks, int currentBlock, string hash) {
    for (int i = 0; i < blocks[currentBlock].DiffTarget; i++) {
        if (hash[i] != '0') {
            return false;
        }
    }
    for (int i = 0; i < blocks.size(); i++) {
        if (hash == blocks[i].prevBlockHash) {
            return false;
        }
    }
    return true;
}

string findNewBlockHash(vector<block> &blocks, int currentBlock) {
    for (int i = 0;;i++) {
        string hash = createHash(IntToString(rand()), secret);
        
        if (hashMeetsRequirements(blocks, currentBlock, hash)){
            cout << dec << i << ":" << hex << hash << endl;
            blocks[currentBlock].nonce++;
            return hash;
        }
        blocks[currentBlock].nonce++;
        // if (i % 10000 == 0) {
        //     cout << i <<endl;
        // }
    }

}

int main (int argc, char *argv[]) {
    int selection = atoi(argv[1]);
    string input;
    string givenFileName;

    vector<user> u;
    vector<transaction> tx;
    vector<transaction> pool;
    vector<block> blocks;

    int currentBlock = 0;
    string newBlockHash = "NULL";

    generateNewUsers(u, 1000);
    generateNewTransactions(u, tx, 1000);
    generateNewTransactionIDs(tx); 
    addTransactionsToPool(pool, tx);
    validatePool(pool, u);
    cout << dec << "pool size is " << pool.size() << hex << endl;
    while (pool.size() > 0) {
        addTransactionsToBlock(blocks, pool, currentBlock);
        blocks[currentBlock].prevBlockHash = newBlockHash;
        newBlockHash = findNewBlockHash(blocks, currentBlock);

        makeTransactions(u, pool);
        removeTransactionsFromPool(pool);
        validatePool(pool, u);
        currentBlock++;
    }

    // for (int i = 0; i < blocks.size(); i++) {
    //     cout << blocks[i].prevBlockHash << endl;
    // }

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