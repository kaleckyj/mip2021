#include "mbed.h"
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <map>

DigitalOut myled(LED1);
/* nefunguje???? std::map<char, string> morse = {{'A',".-"},{'B',"-..."},{'C',"-.-."},{'D',"-.."},{'E',"."},{'F',"..-."},{'G',"--."},{'H',"...."},{'I',".."},{'J',".---"},{'K',"-.-"},{'L',".-.."},{'M',"--"},{'N',"-."},{'O',"---"},{'P',".--."},{'Q',"--.-"},{'R',".-."},{'S',"..."},{'T',"-"},{'U',"..-"},{'V',"...-"},{'W',".--"},{'X',"-..-"},{'Y',"-.--"},{'Z',"--.."}};*/
string name = "Jan Kalecky";

string string_to_morse(string str);
string char_to_morse(char letter);

float base_timespan = 0.1;
float letter_space_timespan = base_timespan*3;
float word_space_timespan = base_timespan*7;
float char_timespan = base_timespan;

int main() {  
    string morse_name = string_to_morse(name);
    for (int i = 0; i < morse_name.size(); i++){        
        if (morse_name[i] == '|') {
            wait(word_space_timespan);
            continue;
        }   
        if (morse_name[i] == '*') {
            wait(base_timespan);
            continue;
        }                      
        if (morse_name[i] == '.') {
            char_timespan = base_timespan;
        }
        if (morse_name[i] == '-') {
            char_timespan = base_timespan*3;
        }
        myled = 1;
        wait(char_timespan);
        myled = 0;
        wait(letter_space_timespan);
    }
}

string string_to_morse(string str) {
    string res = "";
    for (int i = 0; i < str.size(); i++){
        res += char_to_morse(str[i]);
    }
    return res;
    
    /* proč tohle nefunguje? for(char & letter : str) {}*/
}

string char_to_morse(char letter) {
    switch (toupper(letter)) {
        case 'A':
            return ".-*";
        case 'B':
            return "-...*";
        case 'C':
            return "-.-.*";
        case 'D':
            return "-..*";
        case 'E':
            return ".*";
        case 'F':
            return "..-.*";
        case 'G':
            return "--.*";
        case 'H':
            return "....*";
        case 'I':
            return "..*";
        case 'J':
            return ".---*";
        case 'K':
            return "-.-*";
        case 'L':
            return ".-..*";
        case 'M':
            return "--*";
        case 'N':
            return "-.*";
        case 'O':
            return "---*";
        case 'P':
            return ".--.*";
        case 'Q':
            return "--.-*";
        case 'R':
            return ".-.*";
        case 'S':
            return "...*";
        case 'T':
            return "-*";
        case 'U':
            return "..-*";
        case 'V':
            return "...-*";
        case 'W':
            return ".--*";
        case 'X':
            return "-..-*";
        case 'Y':
            return "-.--*";
        case 'Z':
            return "--..*";
        case '1':
            return ".----*";
        case '2':
            return "..---*";
        case '3':
            return "...--*";
        case '4':
            return "....-*";
        case '5':
            return ".....*";
        case '6':
            return "-....*";
        case '7':
            return "--...*";
        case '8':
            return "---..*";
        case '9':
            return "----.*";
        case '0':
            return "-----*";
        case ' ':
            return "|";
        default: return "";
    }
}
