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

class candidate {
    public:
        int nonce = 0;
        int DiffTarget = 2;
        int id = 0;
        vector<transaction> tx;
        vector<int> poolIndexes;
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
    while (abs(hash) < MINIMUM || abs(hash) > MAXIMUM) {
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
    return hash;
}
long int lastHash = 1024;
string lastInput = "";
string lastFinalHex = "";

const int TRANSACTIONS_PER_BLOCK = 100;
string createHash(string input, string secret) {
    long int hash = 1024;
    long int hashArr[8];
    for (int i = 0; i < input.size(); i++) {
        hash += (input[i] * input[i]) + pow(i * 2, 2);
    }
    hash = checkPrimaryHash(hash);
    lastHash = hash;
    lastInput = input;
    for (int i = 1; i <= 8; i++) {
        hashArr[i-1] = hash * secret[pow(i, 4.75) / sqrt(i)];
        hashArr[i-1] = checkPrimaryHash(hash * secret[pow(i, 4.75) / sqrt(i)]);
    }

    string finalHex = "";
    for (int i = 0; i < 8; i++) {
        cout << hex;

        stringstream stream;
        stream << hex << hashArr[i];
        string result(stream.str());
        finalHex += reverseString(result);
    }
    
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

void generateNewUsers (vector<user> &u, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++){
        u.push_back(user());
        u[i].firstName = i;
        u[i].publicKey = createHash(IntToString(rand()), secret);
        u[i].balance = 100 + rand() % 1000000;
    }
}

void generateNewTransactions(vector<user> &u, vector<transaction> &tx, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++){
        tx.push_back(transaction());
        tx[i].senderPublicKey = u[rand() % 1000].publicKey;
        tx[i].receiverPublicKey = u[rand() % 1000].publicKey;
        tx[i].sum = 100 + rand() % 1000;
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

void addTransactionsToCandidates(vector<candidate> &candidates, vector<transaction> &pool) {
    int numOfTransactions = TRANSACTIONS_PER_BLOCK;
    if (TRANSACTIONS_PER_BLOCK > pool.size()) {
        numOfTransactions = pool.size();
    }
    for (int c = 0; c < 5; c++) {
        candidates.push_back(candidate());
        candidates[c].id = c;
        for (int i = 0; i < numOfTransactions; i++){
            int randomIndex = rand() % pool.size();
            candidates[c].poolIndexes.push_back(randomIndex);
            candidates[c].tx.push_back(pool[randomIndex]);
        }
    }
}

void addTransactionsToBlock(vector<block> &blocks, vector<transaction> &pool, int blockNum, vector<int> &poolIndexes, vector<candidate> candidates, int candidateIndex) {
    
    int numOfTransactions = TRANSACTIONS_PER_BLOCK;
    if (TRANSACTIONS_PER_BLOCK > pool.size()) {
        numOfTransactions = pool.size();
    }
    for (int i = 0; i < numOfTransactions; i++){
        // Pass up the poolIndexes
        poolIndexes.push_back(candidates[candidateIndex].poolIndexes[i]);
        // Pass up the transactions
        blocks[blockNum].tx.push_back(candidates[candidateIndex].tx[i]);
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
}

void removeTransactionsFromPool(vector<transaction> &pool, vector<int> &poolIndexes) {
    int numOfTransactions = TRANSACTIONS_PER_BLOCK;
    if (TRANSACTIONS_PER_BLOCK > pool.size()) {
        numOfTransactions = pool.size();
    }

    for (int i = 0; i < numOfTransactions; i++) {
        pool.erase(pool.begin() + poolIndexes[i]);
    }

    poolIndexes.clear();    
}

bool hashMeetsRequirements (vector<block> &blocks, vector<candidate> &candidates, int currentBlock, string hash) {
    for (int i = 0; i < candidates[currentBlock].DiffTarget; i++) {
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

string findNewBlockHash(vector<block> &blocks, vector<candidate> &candidates, int currentBlock, int numOfTries) {
    for (int i = 0; i < numOfTries; i++) {
        string hash = createHash(IntToString(rand()), secret);
        
        if (hashMeetsRequirements(blocks, candidates, currentBlock, hash)){
            cout << dec << i << ":" << hex << hash << endl;
            candidates[currentBlock].nonce++;
            return hash;
        }
        candidates[currentBlock].nonce++;
    }
    printf ("%s \n", "Kandidatui nepavyko rasti hasho.");
    return "";
}

int main (int argc, char *argv[]) {
    int selection = atoi(argv[1]);
    string input;
    string givenFileName;

    vector<user> u;
    vector<transaction> tx;
    vector<transaction> pool;
    vector<block> blocks;
    vector<candidate> candidates;

    vector<int> poolIndexes;

    int currentBlock = 0;
    string newBlockHash = "NULL";

    generateNewUsers(u, 1000);
    generateNewTransactions(u, tx, 1000);
    generateNewTransactionIDs(tx); 
    addTransactionsToPool(pool, tx);
    validatePool(pool, u);
    cout << dec << "pool size is " << pool.size() << hex << endl;
    while (pool.size() > 0) {
        blocks.push_back(block());
        addTransactionsToCandidates(candidates, pool);
        
        blocks[currentBlock].prevBlockHash = newBlockHash;

        int numOfTries = 10000;
        while (true) {
            int randomCandidate = rand() % candidates.size();
            printf ("%s", "Dabar bandancio kandidato id : ");
            printf ("%d \n", candidates[randomCandidate].id);
            string result = findNewBlockHash(blocks, candidates, randomCandidate, numOfTries);
            if (result != ""){
                newBlockHash = result;
                addTransactionsToBlock(blocks, pool, currentBlock, poolIndexes, candidates, randomCandidate);
                break;
            } else if (candidates.size() == 1){
                printf ("%s \n", "Kandidatai baigesi. Keliamas bandymu skaicius. ");
                numOfTries = 10000000;
            } else {
                candidates.erase(candidates.begin() + randomCandidate);
            }
        }

        makeTransactions(u, pool);
        removeTransactionsFromPool(pool, poolIndexes);
        candidates.clear();
        validatePool(pool, u);
        currentBlock++;
    }
}