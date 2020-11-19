# Hash Generator, Release v0.1
A hash generating program, which hashes strings or files by my own hashing algorithm. Outputs 256bit (64 character) hashes. 
The program can be ran in five different modes: <br/>
  1: File input - Hashes a file, by joining treating data as a string, and produces one hash. <br/>
  2: Manual input - Hashes a string of text, entered in the console.<br/>
  3: Constitution - Hashes a file <b>konstitucija.txt</b>. Produces one hash per line.<br/>
  4: Random Pairs - Hashes a file <b>pairs.txt</b>, which contains random strings of various lengths. Produces a hash for each string and compares them.<br/>
  5: Similar Pairs - Hashes a file <b>similarPairs.txt</b>, which contains pairs of strings, which differ only by a single symbol in the middle of the input. Produces a hash for each string and compares them.<br/>
# Testing results (Speed) <br/>
Using mode 3 (Constitution)
Reads and hashes every line and outputs hashes in the console window. <br/>
  With output on - 95.5ms<br/>
  With output off - 25.1ms<br/>
  
# Testing results (Collision) <br/>
Using mode 4 and 5 (Random and similar pairs of the same length) <br/>
Both files have 100000 strings. <br/>
  Random pairs (Mode 4) - 0.002%<br/>
  Similar pairs (Mode 5) - 0%<br/>

# Testing results (Similarity) <br/>
Using mode 4 and 5 (Random and similar pairs of the same length) <br/>
Both files have 100000 strings. <br/>
  Random pairs (Mode 4) :<br/>
    Average (Binary) - 60.8% <br/>
    Average (Hex) - 24.45% <br/>
    Min (Hex) - 0% <br/>
    Max (Hex) - 100% (Collision)<br/>
    <br/>
  Similar pairs (Mode 5):<br/>
    Average (Binary) - 59.1% <br/>
    Average (Hex) - 21.78% <br/>
    Min (Hex) - 0% <br/>
    Max (Hex) - 75% <br/>

# PC used for testing : <br/>
  CPU - i7-8750H<br/>
  RAM - 16Gb DDR4-2666Mhz Dual Channel<br/>
  Storage - Toshiba XG5 512Gb SSD<br/>
  OS - Windows 10 <br/>
  <br/>
  On all testing compilation was done without any O flags. <br/>
  <br/>

# Instructions on how to run: <br/>
  1. Download and unzip or git clone.
  2. Run <b>g++ -o nameOfExe file.cpp</b> to compile.
  3. Run <b>./nameOfExe selectedModeNumber</b> to launch.
  <br/>
 # Conclusions <br/>
 While overall I'm pretty happy with the current state of the algorithm, it is by no means perfect.
 Security-wise - I think this hash could be easily reversed-engineered by an experienced developer, with the source code or without. 
 In an attempt to improve security, I've used this <b>secretSalt</b> file, which by idea, should be kept secretly.
 Speed-wise - I'm pretty happy with the way the algorithm performs. 
 Reading and outputing is taking way longer than hashing itself, so even though my testing results aren't so hot, by improving the reading and outputing the performance would increase.
 Probably the weakest point in this algorithm is collisions. While they are rare, collisions are way more common when the string size is lower (8-15 characters). 
 Also, the similarity percentages aren't great either. 
 On the other hand, in my opinion, the performance stand-point is the only place where this kind of algorithm truly shines. 
 <br/>
 # Hash Generator, Release v0.2 <br/>
  Updated the project to function as a primitive blockchain. Added users, transactions, pools, blocks, candidates, PoW mining and a final result of a chain of blocks with transactions. <br/>
  I have to admit, I've skipped a few requirements. Firstly, I've done nothing related to Merkle roots. And secondly, I don't check transaction validity between creation and addition to the pool. <br/>
  Other than that, the blockchain is fully functional. With my limited testint I've found out that: <br/>
    1) With difficulty setting set to 1, the program can deal with 10k+ validated transactions, it takes some time, mostly while validating the pool and adding/removing transactions, but after a minute or so, it terminates with remaining pool size of 0. <br/>
    2) With difficulty setting set to 2, the program can deal with ~1.5k validated transactions. Any more than that, and the "mining" process starts taking ages. In my testing, the program tries to find the hash for 30M+ tries, and in the end doesn't seem to be able to find one. Maybe it's working fine and just needs way more time, maybe it's the random number generator not being enough random, or maybe I've reached the limitations of my hashing algorithm. <br/>
