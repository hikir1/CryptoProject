#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <random>
#include <gmpxx.h>
#include <unordered_map>
#include <unistd.h>
#include <bits/stdc++.h>
#include <algorithm>
#include <stdio.h>
#include <tgmath.h>
#include <cstdint>
#include <fstream>
#include <iomanip>

unsigned long long int Bin_to_Dec(unsigned long long int given){
	//takes in a long long int of binary converts it to decimal
    unsigned long long int bin_answer = 0;
    unsigned long long int count = 0;
    unsigned long long int remainder = 0;
    //changes it from base 2 to decimal format
    while (given != 0){
        remainder = given % 10;
        given = given / 10;
        bin_answer = bin_answer + remainder * pow(2,count);
        count++;
    }
    //returns the answer
    return bin_answer;
}

std::string addBin(std::string first, std::string second){ 
	//adds two binary strings together in binary number form
    std::string result = "";
    int count = 0;
    int x = first.size() - 1;
    int y = second.size() - 1; 
    //loops through
    while (x >= 0 || y >= 0 || count == 1){ 
        count = count + ((x >= 0)? first[x] - '0': 0); 
        count = count + ((y >= 0)? second[y] - '0': 0); 
        result = char(count % 2 + '0') + result; 
        count = count/2; 
        x--;
        y--; 
    } 
    return result; 
} 

std::string xor_string(std::string first, std::string second){
	//takes in two strings and outputs a string based on xor
	//(in they are equal return 0 return 1 otherwise)
	std::string answer = "";
	for(unsigned int x = 0; x < first.length(); x++){
		if (first[x] == second[x]){
			answer = answer + "0";
		}else{
			answer = answer + "1";
		}
	}
	return answer;
}

std::string and_string(std::string first, std::string second){
	//takes in two strings and outputs a string based on and
	//if they both equal 1 return 1 0 otherwise
	std::string answer = "";
	for(unsigned int x = 0; x < first.length(); x++){
		if (first[x] == '1' && '1' == second[x]){
			answer = answer + "1";
		}else{
			answer = answer + "0";
		}
	}
	return answer;
}

std::string not_string(std::string reverse){
	//takes in one string and outputs a string based on not
	//reverses each bit
	std::string answer = "";
	for(unsigned int x = 0; x < reverse.length(); x++){
		if (reverse[x] == '0'){
			answer = answer + "1";
		}else{
			answer = answer + "0";
		}
	}
	return answer;
}

std::string or_string(std::string first, std::string second){
	//takes in two strings and outputs a string based on and
	//if they both equal 0 return 0, 1 otherwise
	std::string answer = "";
	for(unsigned int x = 0; x < first.length(); x++){
		if (first[x] == '0' && '0' == second[x]){
			answer = answer + "0";
		}else{
			answer = answer + "1";
		}
	}
	return answer;
}

void conversionMap(std::unordered_map<std::string, char> *bth){ 
    (*bth)["0000"] = '0'; 
    (*bth)["0001"] = '1'; 
    (*bth)["0010"] = '2'; 
    (*bth)["0011"] = '3'; 
    (*bth)["0100"] = '4'; 
    (*bth)["0101"] = '5'; 
    (*bth)["0110"] = '6'; 
    (*bth)["0111"] = '7'; 
    (*bth)["1000"] = '8'; 
    (*bth)["1001"] = '9'; 
    (*bth)["1010"] = 'a'; 
    (*bth)["1011"] = 'b'; 
    (*bth)["1100"] = 'c'; 
    (*bth)["1101"] = 'd'; 
    (*bth)["1110"] = 'e'; 
    (*bth)["1111"] = 'f'; 
} 

std::string Hex_to_Bin(std::string given){ 
    long int x = 0;
    std::string answer = "";
    while (given[x]) { 
        switch (given[x]) { 
        case '0': 
            answer = answer + "0000"; 
            break; 
        case '1': 
            answer = answer + "0001"; 
            break; 
        case '2': 
            answer = answer + "0010"; 
            break; 
        case '3': 
            answer = answer + "0011"; 
            break; 
        case '4': 
            answer = answer + "0100"; 
            break; 
        case '5': 
            answer = answer + "0101"; 
            break; 
        case '6': 
            answer = answer + "0110"; 
            break; 
        case '7': 
            answer = answer + "0111"; 
            break; 
        case '8': 
            answer = answer + "1000"; 
            break; 
        case '9': 
            answer = answer + "1001"; 
            break; 
        case 'A': 
        case 'a': 
            answer = answer + "1010"; 
            break; 
        case 'B': 
        case 'b': 
            answer = answer + "1011"; 
            break; 
        case 'C': 
        case 'c': 
            answer = answer + "1100"; 
            break; 
        case 'D': 
        case 'd': 
            answer = answer + "1101"; 
            break; 
        case 'E': 
        case 'e': 
            answer = answer + "1110"; 
            break; 
        case 'F': 
        case 'f': 
            answer = answer + "1111"; 
            break; 
        default: 
            std::cout << "\nIncorrect digit "<< given[x] << std::endl; 
        } 
        x++; 
    } 
    return answer;
}

std::string Bin_to_Hex(std::string given) { 
    while(given.length() % 4 != 0){
        given = "0" + given; 
    }
    std::unordered_map<std::string, char> hex_map; 
    conversionMap(&hex_map);
    std::string hex = ""; 
    for(unsigned int x = 0; x < given.length();){ 
        hex = hex + hex_map[given.substr(x, 4)]; 
        x = x + 4; 
    }
    while(hex.length() % 8 != 0){
        hex = "0" + hex; 
    }
    // required hexadecimal number 
    return hex;     
} 

std::string Dec_to_Bin(unsigned long long int value, bool flag=false){
	//recursive algorithm to convert decimal to binary
    static const std::string digits [2] = {"0", "1"};
    if (value != 0){
        return Dec_to_Bin(value/2, true) + digits[value&1];
    }
    return (flag) ? "" : "0";
}

std::string convert_binary(std::string swapped){
	//takes in a string and converts it to binary
	std::string full_binary = "";
	std::string part_binary = "";
	//loops through each character and turns its ascii value
	//and turns it into 
    for (unsigned int x = 0; x < swapped.length(); x++){
		part_binary = std::bitset<8>(swapped.c_str()[x]).to_string();
        full_binary = full_binary + part_binary;
    }
    //makes sure the length is correct
    while(full_binary.length()%8 != 0){
    	full_binary = "0" + full_binary;
    }
    return full_binary;
}

std::string convert_plaintext(std::string answer){
	std::string plaintext = "";
	//creates a stringstream pointing to answer
    std::stringstream sstream(answer);
    //while there are no error states
    while(sstream.good())
    {
    	//find 8 bits in the stream
        std::bitset<8> bits;
        sstream >> bits;
        //form a char from them
        char character = char(bits.to_ulong());
        //adds it to plaintext
        plaintext = plaintext + character;
    }
    //fixes any trailing spaces
    if(plaintext.length() > (answer.length()/8)){
    	plaintext = plaintext.substr(0, answer.length()/8);
    }
    return plaintext;
}
