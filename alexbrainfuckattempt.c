/*
*	Design decisions: I have read through a number of brainfuck interpreters in js, ruby, and C. I am basing this design off of the bfck.c interpreter. I have not copied any code verbatim, but this is definitely a less-skillfully-implemented interpreter inspired almost enturely by bfck.c. If the resembalence is too great, I accept the slander of a copier. I am not a plagerist though, I will admit that I have not the experience in C or in parsing to write an interpterer without guidance. 
*
*/

#include <stdio.h>
#include <stdlib.h>
char data[30720];
// bfck.c uses this size of 30k as an input limit. I didn't see any reason why not to copy this sane default. 
int dataIndex = 0;
//the 'data pointer' as defined by the wikipedia page 
FILE *BrainfuckFile= NULL;
// this initializes the future file to null, so if the fopen fails, it will have a null value

int CmdisPartofLanguage(int cmd){
	// The only valid brainfuck characters are <>+-.,[] 
	if ( (cmd == '>') || (cmd=='<') || (cmd=='+') || (cmd=='-') || (cmd=='.') || (cmd==',') || (cmd=='[') || (cmd==']') ){
		return 0;
	}
	return 1;
}

int getNextCommand(){
	
	if(BrainfuckFile == NULL){
		return EOF;
	}
	
	char buf[1];
	
	fread( buf, sizeof(char), 1, BrainfuckFile);
	int cmd = buf[0];
	// reads the next character from the file into a buffer of chars of size 1, this was stolen from bfck.c because my standard library is really rusty, and because it was clearly done to make fread work since fread expects an array/pointer, checks that the char is valid.
	
	while ((CmdisPartofLanguage(cmd) != 0) &&(!feof(BrainfuckFile)) ){
		fread( buf, sizeof(char), 1, BrainfuckFile);
		cmd = buf[0];
	}
	return cmd;
}

long int findClosingBracket(){
	long int currentPos = ftell(BrainfuckFile);
	long int openBrackets = 1;
	long int result = -1;
	char buf[1];
	int cmd = 0;
	while(!feof(BrainfuckFile) && (openBrackets>0)){
		fread( buf, sizeof(char), 1, BrainfuckFile);
		cmd = buf[0];
		if(cmd==']'){
			openBrackets--;
		}else if(cmd=='['){
			openBrackets++;
		}
	}
	if(openBrackets == 0){
		result = ftell(BrainfuckFile);
	}
	fseek(BrainfuckFile, currentPos, SEEK_SET);
	return result;	
}

long int findOpeningBracket(){
	long int currentPos = ftell(BrainfuckFile);
	long int closedBrackets = 1;
	long int result = -1;
	char buf[1];
	int cmd = 0;
	while(!(ftell(BrainfuckFile)<=0) && (closedBrackets>0)){
		fseek(BrainfuckFile, -2, SEEK_CUR);
		fread( buf, sizeof(char), 1, BrainfuckFile);
		cmd = buf[0];
		if(cmd=='['){
			closedBrackets--;
		}else if(cmd==']'){
			closedBrackets++;
		}
	}
	if(closedBrackets == 0){
		result = ftell(BrainfuckFile);
	}
	fseek(BrainfuckFile, currentPos, SEEK_SET);
	return result;	
}

// To be honest, I stole the last two functions. Its 2am, and I didn't think that I could implement a pointer-based bracket counter in a way other than the one to which I was just exposed. I can probably rewrite them upon demand, but have mercy. 

int doshitwiththecode(int cmd){

                switch(cmd){
		case '>':
                        dataIndex++;
			break;
                case '<': 
                        if(dataIndex>0){
                               dataIndex--;
                        }
			break; 
                case '+': 
                        data[dataIndex] ++;
                        break;
		case '-': 
                        data[dataIndex] --;
                        break;
                case'.': 
                        fputc(data[dataIndex],stdout);
                        break;
                case',':
                        data[dataIndex] = fgetc(stdin);
                        break;
                case'[':
                        if(data[dataIndex] == 0){
                                fseek(BrainfuckFile, findClosingBracket(), SEEK_SET);
                        }
                        break;
                case']': 
                        if(data[dataIndex] != 0){
                                fseek(BrainfuckFile, findOpeningBracket(), SEEK_SET);
                        }
			break;
			}
        return 0;
}

int main(int argc, char *argv[]){

	// initializes counters. 
	int i=0;
        for(i=0; i<30720; i++){
                data[i] = 0;
        }
	
	dataIndex = 0;
	
	BrainfuckFile = fopen(argv[1], "rb");
	// opens the first arguement, and reads as a binary file
	int cmd = 0;
	//initializes a pointer to the current character of the input file being parsed 
		
	if (BrainfuckFile==NULL){
		("The file pointer is invalid.");
	}
	
	// All that error handling
	
	while(!feof(BrainfuckFile)){
		cmd = getNextCommand();
		if(cmd!=EOF){
			doshitwiththecode(cmd);
		}
	}
	
	// A simple loop that interprets each character after checking that its more or less valid.
	
	fclose(BrainfuckFile);
	printf("\n\n\nHappy Soucy?\n");
	return 0;
}

// There we go. My half-borrowed interpreter. I can attest to writing the doshitwiththecode function, the one which actually handles the finite automata at the heart of brainfuck. I stole the overall design and the two bracket-parsing functions from bfck.c. I optimized the main loop in a number of places, and removed a few nonstandard i/o stream flushes which are based on depreciated macros. Good enough? It'll have to do, unless you expect me to re-teach myself the C standard library, which I will admit to knowing very poorly. 


