/*
    Project 6 - File Compression

    Kseniia Nastahunina

    Program that compresses and decompresses text files using Huffmans encoding
    storing the chracters into ascii values building a tree of it and compressing
    into bites.

*/

#pragma once
#include <queue> //used for priority queue


typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character; //ascii char in th tree
    int count; //number of the ascii tree
    HuffmanNode* zero; //right node of the map tree
    HuffmanNode* one; //left node of the map tree
};

struct compare //used to create priority queue in increasing order
{
    bool operator()(const HuffmanNode *lhs,
        const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};
//
//Recursive free tree function. The function clears the tree to avoid memort leaks.
//Takes current node to free as a parameter.
//
void _freeTree(HuffmanNode* cur) {
    if(cur == nullptr){ // if there is a node
        return;
    }
    _freeTree(cur->one); // clear 'zero' node
    _freeTree(cur->zero); // clear 'one' node
    delete cur;
}
//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
    HuffmanNode* cur = node;
    _freeTree(cur);
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {

    if(isFile){ //if there is a file
        ifstream infile; //opens file
        char curChar = '\0';  //used to take a char form a file
        int asciiChar;

        infile.open(filename.c_str()); //open file

        if(!infile.is_open()) { //checks if the file is open
            cout << "Error: unable to open " "\'" << filename << "\'" << endl;
        }
        else{
            while(!infile.eof()){ // until the end of the file
                int num = 1; // count number of occurances of the key
                infile.get(curChar); // read every char in file
                asciiChar = curChar; // set char to its ascii number
                if(infile.eof())break; // to prevent from reading the last char twice
                if(map.containsKey(asciiChar)){ // if the key already exists
                    num = map.get(asciiChar) + 1; // the new value is the old one + 1
                    map.put(asciiChar, num); // replace the old value
                }
                else{
                    map.put(asciiChar, num); // place a new value into map
                }  
            }
            infile.close(); // close file
            map.put(asciiChar = PSEUDO_EOF, 1); // add EOF to map
        }     
    }
    else if(!isFile){ //used for strings
        char curChar = '\0';  //used to take a char form a file
        int asciiChar;
        for(auto i = 0; i < filename.size(); i++){
            int num = 1;
            curChar = filename[i];
            asciiChar = curChar;
            if(map.containsKey(asciiChar)){ // if the key already exists
                num = map.get(asciiChar) + 1; // the new value is the old one + 1
                map.put(asciiChar, num); // replace the old value
            }
            else{
                map.put(asciiChar, num); // place a new value into map
            }  
        }
        map.put(asciiChar = PSEUDO_EOF, 1); // adds EOF to map
    }
    
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, compare> pq; // priority queue that uses the compare function
                                                                    // to create increasing order
    for(int x: map.keys()){ // for each key in the map
        HuffmanNode* newNode = new HuffmanNode(); // create new node
        newNode->character = x; // character of the node is the key of the map
        newNode->count = map.get(x); // count of the node is the value of teh map
        newNode->one = nullptr;
        newNode->zero = nullptr;
        pq.push(newNode); //add to priority queue
    }
    while(pq.size() > 1){
        HuffmanNode* node0 = pq.top(); pq.pop(); // get the first node and remove it from pq
        HuffmanNode* node1 = pq.top(); pq.pop(); // get the new first node and remove it
        HuffmanNode* newNode = new HuffmanNode(); //create new node to pe the node0 and node1 parent
        newNode->character = NOT_A_CHAR; //parent node does not have a char
        newNode->count = node0->count + node1->count; // its sum is the some of the count of 2 nodes
        newNode->one = node1; // assigns the children to parent node
        newNode->zero = node0;
        pq.push(newNode); // add the new node back to the priority queue
    }
    
    return pq.top(); // return the root of the tree which is the only node that is left in the pq
}

//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str, HuffmanNode* prev) {
    int notChar = NOT_A_CHAR;
    if(node->character != notChar){ //if the node is a valid node
        encodingMap[node->character] = str; // add it to map
        return;
    }
    _buildEncodingMap(node->zero, encodingMap, str + '0', prev); // call the next node with updated string
    _buildEncodingMap(node->one, encodingMap, str + '1', prev);
}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap; // map to return
    string str; // value of the map
    HuffmanNode* prev = nullptr;

    _buildEncodingMap(tree, encodingMap, str, prev); //calls recursive function
    
    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    char curChar = '\0'; // to trace the file
    string binary; //string that represents what is inside the output file
    while(input >> noskipws >> curChar){ // goes throught file without skipping the white spaces
        binary = binary + encodingMap[curChar]; //adds every element in file to the string
    }
    binary = binary + encodingMap[PSEUDO_EOF];

    if(makeFile){ //if a file needs to be created
        //cout << "binary file writeBit: ";
        for(int x: binary){ //go though every int in the string
            if(x == '1'){
                //cout << 1;
                output.writeBit(1); // writes in the out put file 1 bit
            }
            else{
                //cout << 0;
                output.writeBit(0); // writes in the out put file 0 bit
            }
            size++; // increments the size of the file
        }
        //cout << endl;
    }
    
    return binary; 
}

//
// This function recursivly decodes the text by reading bit by bit. 
// Takes current node of the tree to trace it, the root of the tree to reset, the file to read bits from
// and the file to store the decoded text and the string to update to return.
//
string _decode(HuffmanNode* cur, HuffmanNode* encodingTree, ifbitstream &input, ofstream &output, string &decodedInput){
    int bites = input.readBit(); // reads bits from the file to decode it
    int notChar = NOT_A_CHAR;
    
    if(cur->character != notChar){ // if the node is valid
        if(cur->character == PSEUDO_EOF){ //if the last node
            return decodedInput; //break the recursion
        }
        decodedInput = decodedInput + char(cur->character); // convert the int type character into char type character and 
                                                            // add to the string to return
        output.put(cur->character); // add to the file with decoded text
        cur = encodingTree; // reset cur to the root
    }
    if(bites == 1){ //if the current bit is 1
        _decode(cur->one, encodingTree, input, output, decodedInput); // call the 'one' child node
    }
    else{
        _decode(cur->zero, encodingTree, input, output, decodedInput);
    }

    return decodedInput;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    if(encodingTree == nullptr){ //if the tree is empty
        return "";
    }
    string decodedInput; //string to return
    HuffmanNode* cur = encodingTree; //current node to pass to the recursive function, initially set to the root of the tree

    decodedInput = _decode(cur, encodingTree, input, output, decodedInput);

    return decodedInput;  
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    hashmapF map; //used to build frequency
    ifstream infile; //used to open the given file
    ofbitstream output; //used to store the encoded text
    int size; //use for the size of the output file
    
    output.open(filename + ".huf"); //opens the output file with the new name
    infile.open(filename); //opens file to encode

    buildFrequencyMap(filename, true, map); //fills the map
    output << map; // stores the map into the encoded file as a header
    HuffmanNode* newNode = buildEncodingTree(map); // builds the tree and sets the root
    hashmapE emap = buildEncodingMap(newNode); // builds the encoded map
    freeTree(newNode); // frees the memory of the tree
    
    string binary = encode(infile, emap, output, size, true); // creates the bit pattern
    output.close(); // close all the opened files
    infile.close();

    //cout << binary << endl;
    return binary; 
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    string newFileName; // string used to change the name of the decompressed file
    ifbitstream infile; // used to open the encoded file
    infile.open(filename); //open file
    for(auto x = 0; x < filename.size(); x++){ //find the file name before the first '.' to create the new name.
        if(filename[x] == '.'){
            break;
        }
        newFileName = newFileName + filename[x];
    }
    ofstream output; // used to store the decompressed text
    hashmapF map; //map to store and build the frequency
    
    output.open(newFileName + "_unc.txt"); // open file to store decode
    infile >> map; //remove the values of the map from the given file
    HuffmanNode* newNode = buildEncodingTree(map); //build new tree with the map
    string decompressed = decode(infile, newNode, output); //decodes the remainig bits in the file and stores the output to the string
    freeTree(newNode); // frees the tree
    output.close(); //close all of the files.
    infile.close();

    return decompressed;
}
