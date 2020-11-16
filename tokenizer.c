#include <stdio.h>
#include <stdlib.h>

//tokenType is an enum that contains all the types of tokens.
enum tokenType{WORD, DECIMAL, OCTAL, HEX, FLOAT,
		LEFT_PARENTHESIS, RIGHT_PARENTHESIS, LEFT_BRACKET, RIGHT_BRACKET, STRUCTURE_MEMBER, STRUCTURE_POINTER, SIZEOF, COMMA, NEGATE, ONES_COMPLEMENT, 
		SHIFT_RIGHT, SHIFT_LEFT, BITWISE_XOR, BITWISE_OR, INCREMENT, DECREMENT, ADDITION, SUBTRACTION, DIVISION, LOGICAL_OR, LOGICAL_AND, CONDITIONAL_TRUE, CONDITIONAL_FALSE, 
		EQUALITY_TEST, INEQUALITY_TEST, LESS_THAN_TEST, GREATER_THAN_TEST, LESS_THAN_OR_EQUAL_TEST, GREATER_THAN_OR_EQUAL_TEST, ASSIGNMENT, PLUS_EQUALS, 
		MINUS_EQUALS, TIMES_EQUALS, DIVIDE_EQUALS, MOD_EQUALS, SHIFT_RIGHT_EQUALS, SHIFT_LEFT_EQUALS, BITWISE_AND_EQUALS, BITWISE_XOR_EQUALS, BITWISE_OR_EQUALS, AND, MINUS, MULTIPLY, MOD, 
		INVALID};

//The struct Token is a structure that represents a token in the given argument
//@param type holds the type of token this is
//@param start holds the starting index(inclusive)
//@param end holds the ending index(exclusive)
typedef struct Token{
	enum tokenType type;
	int start;
	int end;
} Token;

Token getToken(int start, char *s);
void printToken(Token t, char *s);
void printString(char *s);

//macro used in the main method to skip over whitespace characters
#define IS_WHITESPACE_CHARACTER(c) (c == ' ' || c == '\n' || c == '\t' || c == '\f' || c == '\r' ? 1 : 0)

//The main method
//It checks to ensure that there is only one argument
//afterwords, it loops through the argument and creates tokens, then prints them out
int main(int argc, char **argv)
{
	//The following if-else statement is to ensure that one and only one argument is given.
	if(argc < 2){
		printf("ERROR: No arguments. You must give one argument (put \"insert argument here\" if there are spaces)\n");
		return EXIT_FAILURE;
	}else if(argc > 2){
		printf("ERROR: Too many arguments. You must give only one argumet.(put \"insert argument here\" if there are spaces)\n");
		return EXIT_FAILURE;
	}

	//This function  prints out the given argument and its chars.
	//This exists only for testing purposes and is not part of the program
	//printString(argv[1]);
	
	//The following block of code iterates over the argument, grabbing each start value, then calling getToken and printToken
	//The programs also uses a while loop and a macro to skip over whitespace characters/delimeters
	//getToken constructs the token and printToken prints it out
	
	//int test = 0;//this variable exists for test purposes please ignore this
	
	int start = 0;
	struct Token tok;
	while(IS_WHITESPACE_CHARACTER(argv[1][start])){//remove whitespace characters at beginning
		start++;
	}
	while(argv[1][start] != '\0'){
		tok = getToken(start, argv[1]);
		printToken(tok, argv[1]);
		if(tok.type == STRUCTURE_MEMBER){//ensures that word after structure member is read, as per RefCard.txt
			start++;
		}else{
			start = tok.end;
		}
		while(IS_WHITESPACE_CHARACTER(argv[1][start])){//remove subsequent whitespace characters
			start++;
		}

		//This if statement and the test variable exist solely for testing and is NOT part of the program
		//This block of code ensures that we dont have an infinite loop
		/*if(test > 100){
			printf("infinite loop stupid\n");
			return EXIT_FAILURE;
		}
		test++;*/
	}

	return EXIT_SUCCESS;
}

//The following macros are to be used in method getToken, true/false doesn't seem to work here so I use 1/0
#define IS_CAPITAL_LETTER(c) (c >= 'A' && c <= 'Z' ? 1 : 0)
#define IS_LOWERCASE_LETTER(c) (c >= 'a' && c <= 'z' ? 1 : 0)
#define IS_DIGIT(c) (c >= '0' && c <= '9' ? 1 : 0)
#define IS_OCTAL(c) (c >= '0' && c <= '7' ? 1 : 0)
#define IS_HEX(c) ((c >= '0' && c <= '9')||(c >= 'A' && c <= 'F')||(c >= 'a' && c <= 'f') ? 1 : 0)
//The method getToken obtains an index and its string
//it then determines its type, and returns a token struct and its variables
//The method will iterate through the token, taking O(n) time where n is the size of the TOKEN
//@param start This is the starting index
//@param s This is the string
Token getToken(int start, char *s){
	
	//initialize Token tok
	struct Token tok;
	tok.start = start;

	//Determining the type and end index(exclusive)
	char first = s[start];
	if(first == '0'){//Hex, Octal, Decimal, or Float
		if(IS_DIGIT(s[start+1]) || s[start+1] == '.'){//Decimal Octal or Float
			tok.type = OCTAL;//initially set type to Octal
			int i = start+1;
			int wasOctal = 1;
			while(s[i] != '\0' && (IS_DIGIT(s[i]) || (s[i] == '.' && tok.type != FLOAT))){//ensures that all are digits, does not go pass end of string, and that it goes through only one period for floats
				if(IS_OCTAL(s[i]) == 0 && tok.type != FLOAT && s[i] != '.'){//checking if Octal or Decimal
					tok.type = DECIMAL;
					wasOctal = 0;
				}
				if(s[i] == '.'){//checking for Float
					tok.type = FLOAT;
				}
				i++;
			}
			if(s[i-1] == '.'){//check if '.' is the last char if its a float
				if(wasOctal == 1){
					tok.type = OCTAL;
				}else{
					tok.type = DECIMAL;
				}
				i -= 1;
			}else if(tok.type == FLOAT && s[i] == 'e' && ((s[i+1] == '-' && IS_DIGIT(s[i+2])) || IS_DIGIT(s[i+1]))){//check for scientific notation
				i += s[i+1] == '-' ? 2 : 1;//set i to the next digit, based on whether or not the notation is negative or positive
				while(IS_DIGIT(s[i])){
					i++;
				}
			}
			tok.end = i;
		}else if((s[start+1] == 'x' || s[start+1] == 'X') && s[start+2] != '\0' && IS_HEX(s[start+2])){//Hex, must have 0 then x/X then must have digits following it
			tok.type = HEX;
			int i = start+2;
			while(s[i] != '\0' && IS_HEX(s[i])){
				i++;
			}
			tok.end = i;
		}else{//Single Digit Decimal
			tok.type = DECIMAL;
			tok.end = start+1;
		}
	}else if(IS_DIGIT(first)){//Decimal or Float
		tok.type = DECIMAL;//initially set type to Decimal
		int i = start+1;
		while(s[i] != '\0' && (IS_DIGIT(s[i]) || (s[i] == '.' && tok.type != FLOAT))){//ensures that all are digits, does not go pass end of string, and that it goes through only one period for floats
			if(s[i] == '.'){//checking for Float
				tok.type = FLOAT;
			}
			i++;
		}
		if(s[i-1] == '.'){//check if '.' is the last char
			tok.type = DECIMAL;
			i -= 1;
		}if(tok.type == FLOAT && s[i] == 'e' && ((s[i+1] == '-' && IS_DIGIT(s[i+2])) || IS_DIGIT(s[i+1]))){//check for scientific notation
			i += s[i+1] == '-' ? 2 : 1;//set i to the next digit, based on whether or not the notation is negative or positive
			while(IS_DIGIT(s[i])){
				i++;
			}
		}
		tok.end = i;
	}else if(IS_CAPITAL_LETTER(first) || IS_LOWERCASE_LETTER(first)){//Word
		tok.type = WORD;//set token to Word
		int i = start+1;
		while(s[i] != '\0' && (IS_CAPITAL_LETTER(s[i]) || IS_LOWERCASE_LETTER(s[i]) || IS_DIGIT(s[i]))){
			i++;
		}
		tok.end = i;
		if(s[start] == 's' && s[start+1] == 'i' && s[start+2] == 'z' && s[start+3] == 'e' && s[start+4] == 'o' && s[start+5] == 'f'){//check if string is type sizeOf
			tok.type = SIZEOF;
			tok.end = start+6;
		}
	}else{//Uncommon Types
		int i = start;
		switch(s[i]){//Determine operators
			case '(':
				tok.type = LEFT_PARENTHESIS;
				tok.end = i+1;
				break;
			case ')':
				tok.type = RIGHT_PARENTHESIS;
				tok.end = i+1;
				break;
			case '[':
				tok.type = LEFT_BRACKET;
				tok.end = i+1;
				break;
			case ']':
				tok.type = RIGHT_BRACKET;
				tok.end = i+1;
				break;
			case '.':
				if(IS_CAPITAL_LETTER(s[i+1]) || IS_LOWERCASE_LETTER(s[i+1])){
					int x = i+1;
					while(s[x] != '\0' && (IS_CAPITAL_LETTER(s[x]) || IS_LOWERCASE_LETTER(s[x]) || IS_DIGIT(s[x]))){
						x++;
					}
					tok.type = STRUCTURE_MEMBER;
					tok.end = x;
					tok.start = start+1;
					break;
				}
				tok.type = STRUCTURE_MEMBER;
				tok.end = i+1;
				break;
			case ',':
				tok.type = COMMA;
				tok.end = i+1;
				break;
			case '!':
				if(s[i+1] == '='){
					tok.type = INEQUALITY_TEST;
					tok.end = i+2;
					break;
				}	
				tok.type = NEGATE;
				tok.end = i+1;
				break;
			case '~':
				tok.type = ONES_COMPLEMENT;
				tok.end = i+1;
				break;
			case '>':
				if(s[i+1] == '>'){
					if(s[i+2] == '='){
						tok.type = SHIFT_RIGHT_EQUALS;
						tok.end = i+3;
						break;
					}
					tok.type = SHIFT_RIGHT;
					tok.end = i+2;
					break;
				}
				if(s[i+1] == '='){
					tok.type = GREATER_THAN_OR_EQUAL_TEST;
					tok.end = i+2;
					break;
				}
				tok.type = GREATER_THAN_TEST;
				tok.end = i+1;
				break;
			case '<':
				if(s[i+1] == '<'){
					if(s[i+2] == '='){
						tok.type = SHIFT_LEFT_EQUALS;
						tok.end = i+3;
						break;
					}
					tok.type = SHIFT_LEFT;
					tok.end = i+2;
					break;
				}
				if(s[i+1] == '='){
					tok.type = LESS_THAN_OR_EQUAL_TEST;
					tok.end = i+2;
					break;
				}
				tok.type = LESS_THAN_TEST;
				tok.end = i+1;
				break;
			case '^':
				if(s[i+1] == '='){
					tok.type = BITWISE_XOR_EQUALS;
					tok.end = i+2;
					break;
				}
				tok.type = BITWISE_XOR;
				tok.end = i+1;
				break;
			case '|':
				if(s[i+1] == '|'){
					tok.type = LOGICAL_OR;
					tok.end = i+2;
					break;
				}
				if(s[i+1] == '='){
					tok.type = BITWISE_OR_EQUALS;
					tok.end = i+2;
					break;
				}
				tok.type = BITWISE_OR;
				tok.end = i+1;
				break;
			case '+':
				if(s[i+1] == '+'){
					tok.type = INCREMENT;
					tok.end = i+2;
					break;
				}
				if(s[i+1] == '='){
					tok.type = PLUS_EQUALS;
					tok.end = i+2;
					break;
				}
				tok.type = ADDITION;
				tok.end = i+1;
				break;
			case '-':
				if(s[i+1] == '-'){
					tok.type = DECREMENT;
					tok.end = i+2;
					break;
				}
				if(s[i+1] == '='){
					tok.type = MINUS_EQUALS;
					tok.end = i+2;
					break;
				}
				if(s[i+1] == '>'){
					tok.type = STRUCTURE_POINTER;
					tok.end = i+2;
					break;
				}
				tok.type = SUBTRACTION;
				tok.end = i+1;
				break;
			case '/':
				if(s[i+1] == '='){
					tok.type = DIVIDE_EQUALS;
					tok.end = i+2;
					break;
				}
				tok.type = DIVISION;
				tok.end = i+1;
				break;
			case '&':
				if(s[i+1] == '&'){
					tok.type = LOGICAL_AND;
					tok.end = i+2;
					break;
				}
				if(s[i+1] == '='){
					tok.type = BITWISE_AND_EQUALS;
					tok.end = i+2;
					break;
				}
				tok.type = AND;
				tok.end = i+1;
				break;
			case '?':
				tok.type = CONDITIONAL_TRUE;
				tok.end = i+1;
				break;
			case ':':
				tok.type = CONDITIONAL_FALSE;
				tok.end = i+1;
				break;
			case '=':
				if(s[i+1] == '='){
					tok.type = EQUALITY_TEST;
					tok.end = i+2;
					break;
				}
				tok.type = ASSIGNMENT;
				tok.end = i+1;
				break;
			case '*':
				if(s[i+1] == '='){
					tok.type = TIMES_EQUALS;
					tok.end = i+2;
					break;
				}
				tok.type = MULTIPLY;
				tok.end = i+1;
				break;
			case '%':
				if(s[i+1] == '='){
					tok.type = MOD_EQUALS;
					tok.end = i+2;
					break;
				}
				tok.type = MOD;
				tok.end = i+1;
				break;
			default:
				tok.type = INVALID;
				tok.end = i+1;
				break;
			
		}
	}
	return tok;
}

//printToken() takes a token and its argument, prints out its type, the token, then goes to the next line
//example: for token HelloWorld: word: "HelloWorld"
//printing out the string takes O(n) time, so the Method is O(n) where n is the number of characters in the token
//@param t This is the given token
//@param s This is the token's String
void printToken(Token t, char *s){
	char *typePrint;
	switch(t.type){//determine the type and its associated string such as "word" and "right bracket"
		case WORD:
			typePrint = "word";
			break;
		case DECIMAL:
			typePrint = "decimal integer";
			break;
		case FLOAT:
			typePrint = "floating point";
			break;
		case HEX:
			typePrint = "hexadecimal integer";
			break;
		case OCTAL:
			typePrint = "octal integer";
			break;
		case LEFT_PARENTHESIS:
			typePrint = "left parenthesis";
			break;
		case RIGHT_PARENTHESIS:
			typePrint = "right parenthesis";
			break;
		case LEFT_BRACKET:
			typePrint = "left bracket";
			break;
		case RIGHT_BRACKET:
			typePrint = "right bracket";
			break;
		case STRUCTURE_MEMBER:
			typePrint = "structure member";
			break;
		case STRUCTURE_POINTER:
			typePrint = "structure pointer";
			break;
		case SIZEOF:
			typePrint = "sizeof";
			break;
		case COMMA:
			typePrint = "comma";
			break;
		case NEGATE:
			typePrint = "negate";
			break;
		case ONES_COMPLEMENT:
			typePrint = "1s complement";
			break;
		case SHIFT_RIGHT:
			typePrint = "shift right";
			break;
		case SHIFT_LEFT:
			typePrint = "shift left";
			break;
		case BITWISE_XOR:
			typePrint = "bitwise xor";
			break;
		case BITWISE_OR:
			typePrint = "bitwise or";
			break;
		case INCREMENT:
			typePrint = "increment";
			break;
		case DECREMENT:
			typePrint = "decrement";
			break;
		case ADDITION:
			typePrint = "addition";
			break;
		case SUBTRACTION:
			typePrint = "subtraction";
			break;
		case DIVISION:
			typePrint = "division";
			break;
		case LOGICAL_OR:
			typePrint = "logical or";
			break;
		case LOGICAL_AND:
			typePrint = "logical and";
			break;
		case CONDITIONAL_TRUE:
			typePrint = "conditional true";
			break;
		case CONDITIONAL_FALSE:
			typePrint = "conditional false";
			break;
		case EQUALITY_TEST:
			typePrint = "equality test";
			break;
		case INEQUALITY_TEST:
			typePrint = "inequality test";
			break;
		case LESS_THAN_TEST:
			typePrint = "less than test";
			break;
		case GREATER_THAN_TEST:
			typePrint = "greater than test";
			break;
		case LESS_THAN_OR_EQUAL_TEST:
			typePrint = "less than or equal test";
			break;
		case GREATER_THAN_OR_EQUAL_TEST:
			typePrint = "greater than or equal test";
			break;
		case ASSIGNMENT:
			typePrint = "assignment";
			break;
		case PLUS_EQUALS:
			typePrint = "plus equals";
			break;
		case MINUS_EQUALS:
			typePrint = "minus equals";
			break;
		case TIMES_EQUALS:
			typePrint = "times equals";
			break;
		case DIVIDE_EQUALS:
			typePrint = "divide equals";
			break;
		case MOD_EQUALS:
			typePrint = "mod equals";
			break;
		case SHIFT_RIGHT_EQUALS:
			typePrint = "shift right equals";
			break;
		case SHIFT_LEFT_EQUALS:
			typePrint = "shift left equals";
			break;
		case BITWISE_AND_EQUALS:
			typePrint = "bitwise AND equals";
			break;
		case BITWISE_XOR_EQUALS:
			typePrint = "bitwise XOR equals";
			break;
		case BITWISE_OR_EQUALS:
			typePrint = "bitwise OR equals";
			break;
		case AND:
			typePrint = "and/address operator";
			break;
		case MINUS:
			typePrint = "minus/subtract operator";
			break;
		case MULTIPLY:
			typePrint = "multiply/dereference operator";
			break;
		case MOD:
			typePrint = "modulus";
			break;
		default:
			typePrint = "INVALID TYPE";		
			break;
	}
	printf("%s: \"", typePrint);
	int i = t.start;
	while(i < t.end){
		printf("%c", s[i]);
		i++;
	}
	printf("\"\n");
}

//The method printString takes a string and prints it out, alongside all its chars.
//This exists only for testing purposes and is not part of the program
//@param s This is the string that will be printed
void printString(char *s){
	printf("%s\n", s);
	int i;
	for(i = 0; s[i] != '\0'; i++){
		if(s[i] == '\n'){
			printf("THERE IS A BACKSLASH THING");
		}
		printf("\'%c\'\n", s[i]);
	}
	printf("\n");
}
