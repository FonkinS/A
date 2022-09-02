#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int MultiLoop = 0;
int DivisLoop = 0;
int LThanLoop = 0;
int EQOLTLoop = 0;
int BRANCHES = 0;

struct ParserNode {
	int type;
	char data[15];
	int SiblingIndex;
	struct ParserNode *Parent;
	struct ParserNode *Child1, *Child2;
} empty;

typedef struct ParserNode Node;

void compress_array(Node *parseStack[], int largestLine){
	/*for (int i = 0; i < largestLine; i++) {
		if (parseStack[i]->data[0] != *""){
			printf("%i ", &parseStack[i]);
		}
	}
	printf("\n\n");*/
	bool previousEmpty = false;
	for (int j = 0; j < largestLine-1; j++) {
		for (int i = 0; i < largestLine-1; i++){
			if (parseStack[i] == NULL){
				previousEmpty = true;
			} else {
				if (previousEmpty == true && i != 0){
					parseStack[i - 1] = parseStack[i];
					parseStack[i] = NULL;
				}
				previousEmpty = false;
			}
		}
	}
	/*for (int i = 0; i < largestLine; i++) {
		if (parseStack[i]->data[0] != *""){
			printf("%i ", &parseStack[i]);
		}
	}
	printf("\n\n");
	//printf("\n");*/
}

void printNodes(Node *self, int level){
	if (self != NULL) {
		if (self->Child1 != NULL) {
			printNodes(self->Child1, level+1);
		} 	
		for (int i = 0; i < level; i++){
			printf("\t");
		}
		printf("%s\n", self->data);
		//printf("\t%i\n", self->Child1);


		if (self->Child2 != NULL) {
			printNodes(self->Child2, level+1);
		}
	}
}

int lineParse(Node *parseStack[], int startPoint, int endpoint) {
	int forcedEnd = startPoint + 1;
	for (int i = startPoint; i < endpoint && (int)parseStack[i]->data[0] != 0 && parseStack[i]->data[0] != *")"; i++){
		if ((strcmp(parseStack[i]->data, "*") == 0 || strcmp(parseStack[i]->data, "/") == 0 || strcmp(parseStack[i]->data, "%") == 0) && parseStack[i]->Child1 == NULL && parseStack[i]->Child2 == NULL) {
			parseStack[i]->Child1 = parseStack[i-1];
			parseStack[i]->Child2 = parseStack[i+1];
			parseStack[i-1] = 0;
			parseStack[i+1] = 0;
			compress_array(parseStack, endpoint);
			i--;						// because if (7 * 8 / 4) then when the 7 and the 8 get removed, (-2) then it will skip straight to the 4, missing the / sign
		}
	}
	for (int i = startPoint; i < endpoint && (int)parseStack[i]->data[0] != 0 && parseStack[i]->data[0] != *")"; i++){
		if ((strcmp(parseStack[i]->data, "+") == 0 || strcmp(parseStack[i]->data, "-") == 0) && parseStack[i]->Child1 == NULL && parseStack[i]->Child2 == NULL) {
			parseStack[i]->Child1 = parseStack[i-1];
			parseStack[i]->Child2 = parseStack[i+1];
			parseStack[i-1] = 0;
			parseStack[i+1] = 0;
			compress_array(parseStack, endpoint);
			i--;						// because if (7 + 8 - 4) then when the 7 and the 8 get removed, (-2) then it will skip straight to the 4, missing the - sign							
		}
	}
	for (int i = startPoint; i < endpoint && (int)parseStack[i]->data[0] != 0 && parseStack[i]->data[0] != *")"; i++){
		if ((strcmp(parseStack[i]->data, "==") == 0 || strcmp(parseStack[i]->data, "!=") == 0 || strcmp(parseStack[i]->data, ">=") == 0 || strcmp(parseStack[i]->data, "<=") == 0 || strcmp(parseStack[i]->data, ">") == 0 || strcmp(parseStack[i]->data, "<") == 0) && parseStack[i]->Child1 == NULL && parseStack[i]->Child2 == NULL) {
			parseStack[i]->Child1 = parseStack[i-1];
			parseStack[i]->Child2 = parseStack[i+1];
			parseStack[i-1] = 0;
			parseStack[i+1] = 0;
			compress_array(parseStack, endpoint);
			i--;						// because if (7 + 8 - 4) then when the 7 and the 8 get removed, (-2) then it will skip straight to the 4, missing the - sign						
		}
	}
	for (int i = startPoint; i < endpoint && (int)parseStack[i]->data[0] != 0 && parseStack[i]->data[0] != *")"; i++){
		if ((strcmp(parseStack[i]->data, "and") == 0 || strcmp(parseStack[i]->data, "or") == 0) && parseStack[i]->Child1 == NULL && parseStack[i]->Child2 == NULL) {
			parseStack[i]->Child1 = parseStack[i-1];
			parseStack[i]->Child2 = parseStack[i+1];
			parseStack[i-1] = 0;
			parseStack[i+1] = 0;
			compress_array(parseStack, endpoint);
			i--;						// because if (7 + 8 - 4) then when the 7 and the 8 get removed, (-2) then it will skip straight to the 4, missing the - sign		
		}
		forcedEnd = i + 1;
	}
	return forcedEnd;
}

int calculateInstruction(Node *self, char* Text, int currentIndexInText, int textSize, int ChildIndex, char funcParams[10][25], char variables[256][25], int variableTypes[256], char functions[16][25]){
	if (self->Child1 != NULL) {
		currentIndexInText = calculateInstruction(self->Child1, Text, currentIndexInText, 128, 1, funcParams, variables, variableTypes, functions);
	} if (self->Child2 != NULL) {
		currentIndexInText = calculateInstruction(self->Child2, Text, currentIndexInText, 128, 2, funcParams, variables, variableTypes, functions);
	}
	
	if (currentIndexInText + 8 < textSize) {
		textSize += 256;
		Text = realloc(Text, textSize * sizeof(char));
	}

	char XorY;
	if (ChildIndex == 1) {
		XorY = 'X';
	} else {
		XorY = 'Y';
	}

	//printf("%i\t%s\n", self->type, self->data);
	if (self->type == 3) {
		if (self->data[0] == *"+" || self->data[0] == *"-") {
			char text[] = "LDA $00\nCLC\nADC $01\nSTA $00\n";
			if (self->data[0] == *"-") {
				text[8] = 'S';
				text[9] = 'E';
				text[12] = 'S';
				text[13] = 'B';
			}
			text[26] = (char) ChildIndex + 47;
			for (int i = 0; i < strlen(text); i++) {
				Text[currentIndexInText] = text[i];
				currentIndexInText++;
			}
		} else if (self->data[0] == *"*"){
			char text[] = "LDX $00\nBEQ MEND00\nSTX $02\nLDA $01\nSEC\nSBC #1\nSTA $01\nMULTI00:\nLDA $00\nCLC\nADC $02\nSTA $00\nLDA $01\nSEC\nSBC #$1\nSTA $01\nBNE MULTI00\nMEND00:\nLDX $00\nSTX $00\n";
			
			text[16] = (char)((int) (MultiLoop / 24)) + 65;
			text[17] = (char) MultiLoop % 24 + 65;
			text[59] = (char)((int) (MultiLoop / 24)) + 65;
			text[60] = (char) MultiLoop % 24 + 65;
			text[128] = (char)((int) (MultiLoop / 24)) + 65;
			text[129] = (char) MultiLoop % 24 + 65;
			text[135] = (char)((int) (MultiLoop / 24)) + 65;
			text[136] = (char) MultiLoop % 24 + 65;
			MultiLoop++;

			text[153] = (char) ChildIndex + 47;

			for (int i = 0; i < strlen(text); i++) {
				Text[currentIndexInText] = text[i];
				currentIndexInText++;
			}
		} else if (self->data[0] == *"/") {
			char text[] = "LDX #$0\nSTX $02\nDIVIS00:\nLDA $02\nCLC\nADC #1\nSTA $02\nLDA $00\nSEC\nSBC $01\nSTA $00\nBEQ DF00\nBPL DIVIS00\nLDA $02\nSEC\nSBC #1\nSTX $02\nDF00:\nLDX $02\nSTX $00\n";
			text[21] = (char)((int) (DivisLoop / 24)) + 65;
			text[22] = (char) DivisLoop % 24 + 65;
			text[86] = (char)((int) (DivisLoop / 24)) + 65;
			text[87] = (char) DivisLoop % 24 + 65;
			text[98] = (char)((int) (DivisLoop / 24)) + 65;
			text[99] = (char) DivisLoop % 24 + 65;
			text[130] = (char)((int) (DivisLoop / 24)) + 65;
			text[131] = (char) DivisLoop % 24 + 65;
			DivisLoop++;

			text[148] = (char) ChildIndex + 47;

			for (int i = 0; i < strlen(text); i++) {
				Text[currentIndexInText] = text[i];
				currentIndexInText++;
			}
		}
	} else if (self->type == 9) {
		if (strcmp(self->data, "<") == 0 || strcmp(self->data, ">") == 0 || strcmp(self->data, "==") == 0 || strcmp(self->data, "!=") == 0){
			char text[] = "LDA $0\nCMP $1\nBEQ EQT00\nBPL LTO00\nEQT00:\nLDX #$1\nSTX $0\nJMP LTT00\nLTO00:\nLDX #$0\nSTX $0\nLTT00:\n";
			text[31] = (char)((int) (LThanLoop / 24)) + 65;
			text[32] = (char) LThanLoop % 24 + 65;
			text[21] = (char)((int) (LThanLoop / 24)) + 65;
			text[22] = (char) LThanLoop % 24 + 65;
			text[37] = (char)((int) (LThanLoop / 24)) + 65;
			text[38] = (char) LThanLoop % 24 + 65;
			text[63] = (char)((int) (LThanLoop / 24)) + 65;
			text[64] = (char) LThanLoop % 24 + 65;
			text[69] = (char)((int) (LThanLoop / 24)) + 65;
			text[70] = (char) LThanLoop % 24 + 65;
			text[91] = (char)((int) (LThanLoop / 24)) + 65;
			text[92] = (char) LThanLoop % 24 + 65;
			LThanLoop++;

			text[85] = (char) ChildIndex + 47;

			if (self->data[0] == '=' || self->data[0] == '!'){
				text[21] = 'N';
				text[22] = 'E';
			}

			if (self->data[0] == '>' || self->data[0] == '!') {
				text[47] = '0';
				text[79] = '1';
			}
			for (int i = 0; i < strlen(text); i++) {
				Text[currentIndexInText] = text[i];
				currentIndexInText++;
			}
		} else if (strcmp(self->data, "<=") == 0 || strcmp(self->data, ">=") == 0){
			char text[] = "LDA $0\nCMP $1\nBNE NEQ00\nLDX #1\nJMP ETF00\nNEQ00:\nBPL ILT00\nLDX #1\nJMP EFT00\nILT00:\nLDX #0\nEFT00:\nSTX $0\n";
			text[21] = (char)((int) (EQOLTLoop / 24)) + 65;
			text[22] = (char) EQOLTLoop % 24 + 65;
			text[38] = (char)((int) (EQOLTLoop / 24)) + 65;
			text[39] = (char) EQOLTLoop % 24 + 65;
			text[44] = (char)((int) (EQOLTLoop / 24)) + 65;
			text[45] = (char) EQOLTLoop % 24 + 65;
			text[55] = (char)((int) (EQOLTLoop / 24)) + 65;
			text[56] = (char) EQOLTLoop % 24 + 65;
			text[72] = (char)((int) (EQOLTLoop / 24)) + 65;
			text[73] = (char) EQOLTLoop % 24 + 65;
			text[78] = (char)((int) (EQOLTLoop / 24)) + 65;
			text[79] = (char) EQOLTLoop % 24 + 65;
			text[92] = (char)((int) (EQOLTLoop / 24)) + 65;
			text[93] = (char) EQOLTLoop % 24 + 65;


			text[101] = (char) ChildIndex + 47;

			for (int i = 0; i < strlen(text); i++) {
				Text[currentIndexInText] = text[i];
				currentIndexInText++;
			}
		}
	} else if (self->type == 12) {
			char text1[] = "LDA #";
			char text2[] = "\nSTA $00\n";
			text2[7] = (char) ChildIndex + 47;
			for (int i = 0; text1[i] != '\0'; i++) {
				Text[currentIndexInText] = text1[i];
				currentIndexInText++;
			}
			for (int i = 0; self->data[i] != '\0'; i++) {
				Text[currentIndexInText] = self->data[i];
				currentIndexInText++;
			}
			for (int i = 0; text2[i] != '\0'; i++) {
				Text[currentIndexInText] = text2[i];
				currentIndexInText++;
			}
			
	} else if (self->type == 2) {
		int location = -1;

		for (int i = 0; i < 16; i++){
			if (strcmp(self->data, functions[i]) == 0){
				location = i;
				char text1[] = "LDX $0\nSTX $100\n";
				if (ChildIndex == 1) {
					text1[5] = '1';
				} else {
					text1[5] = '0';
				}
				for (int i = 0; i < strlen(text1); i++) {
					Text[currentIndexInText] = text1[i];
					currentIndexInText++;
				}


				Text[currentIndexInText + 0] = 'J';
				Text[currentIndexInText + 1] = 'S';
				Text[currentIndexInText + 2] = 'R';
				Text[currentIndexInText + 3] = ' ';
				currentIndexInText = currentIndexInText + 4;
				for (int j = 0; j < 25; j++) {
					if (self->data[j] != '\0'){
						Text[currentIndexInText] = self->data[j];
						currentIndexInText++;
					}
				}
				Text[currentIndexInText] = '\n';
				currentIndexInText = currentIndexInText + 1;
				//printf("%s\n\n", Text);

				char text2[] = "LDX $0\nSTX $0\nLDX $100\nSTX $0\n";
				if (ChildIndex == 1) {
					text2[12] = '0';
					text2[28] = '1';
				} else {
					text2[12] = '1';
					text2[28] = '0';
				}

				for (int i = 0; i < strlen(text2); i++) {
					Text[currentIndexInText] = text2[i];
					currentIndexInText++;
				}
				return currentIndexInText;
			}
		}

		for (int i = 0; i < 10; i++){
			if (strcmp(self->data, funcParams[i]) == 0){
				location = i + 3;
			}
		}
		for (int i = 0; i < 256; i++){
			if (strcmp(self->data, variables[i]) == 0){
				location = i + 13;
			}
		}
		
		if (location == -1){ // STILL?? (not found)
			printf("ERROR!!!");
			return -1;
		}
		char text[] = "LDX 00\nSTX 0\n";
		text[4] = (char)((int) (location / 10)) + 48;
		text[5] = (char) location % 10 + 48;
		text[11] = (char) ChildIndex + 47;
		for (int i = 0; i < strlen(text); i++) {
			Text[currentIndexInText] = text[i];
			currentIndexInText++;
			}
	}
	return currentIndexInText;
}

char *calculateMath(Node Nodes[], char funcParams[10][25], char variables[256][25], int variableTypes[256], int tabAmount, char functions[16][25]) {
	char* Text = (char *) calloc(512,sizeof(char));
	calculateInstruction(&Nodes[tabAmount], Text, 0, 512, 1, funcParams, variables, variableTypes, functions);
	//printf("%s", Text);	
	printf("%s\n", Text);
	return Text;
}

void functionParametersParse(Node *parseStack[], int startpoint, int endpoint){
	if (parseStack[startpoint-1]->type == 2) {
		if (parseStack[startpoint+1]->data[0] != ')'){
			parseStack[startpoint-1]->Child1 = parseStack[startpoint+1];
		}
		int previousFound = startpoint-1;
		for (int i = startpoint; i < endpoint; i++) {
			if (parseStack[i]->type == 11) {
				parseStack[previousFound]->Child2 = parseStack[i];
				parseStack[i]->Child1 = parseStack[i+1];
				previousFound = i;
			}
		}

		for (int i = startpoint; i < endpoint; i++) {
			if (parseStack[i]->type == 4 && parseStack[i+1]->type == 2) {
				parseStack[i]->Child1 = parseStack[i+1];
				//printf("%s\n", parseStack[i+1]->data);
			}
		}
	} 
}

int main(int argc, char *argv[]){
	if (argc == 1){
		return -1;
	}

	FILE *file = fopen(argv[1], "r");


	/*								LEXICAL TOKEN ANALYZER							*/

	// Initialise the Lines Array
	int dataSize = 100;
	char *data = (char *) calloc(dataSize, sizeof(char));

	// Load stuff from file
	char c;
	int tokenAmount = 0;
	for (int i = 0; c != EOF; i++){
		c = fgetc(file);
		if (i >= dataSize) {
			dataSize += 100;
			data = realloc(data, dataSize * sizeof(char));
		}
		data[i] = c;
		if (c == ' ' || c == '(' || c == '\n' || c == '\t' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '=' || c == '>' || c == '<' || c == '+' || c == '-' || c == '%' || c == '*' || c == '/' || c == ',') {
			tokenAmount += 2;
		}
	}

	// Find Tokens
	char tokens[tokenAmount][15];
	for (int t = 0; t < tokenAmount; t++){	// CLEAR THE ARRAY
		for (int c = 0; c < 15; c++){
			tokens[t][c] = '\0';
		}
	}
	int cToken = 0;
	int cInToken = 0;
	for (int i = 0; i < dataSize; i++){
		if (data[i] == ' '){
			cToken++;
			cInToken = 0;
		} else if ((data[i] == '=' && data[i+1] == '=') || (data[i] == '!' && data[i+1] == '=') || (data[i] == '<' && data[i+1] == '=') || (data[i] == '>' && data[i+1] == '=') || (data[i] == '+' && data[i+1] == '=') || (data[i] == '-' && data[i+1] == '=') || (data[i] == '*' && data[i+1] == '=') || (data[i] == '/' && data[i+1] == '=')){
			cToken++;
			cInToken = 0;
			tokens[cToken][0] = data[i]; // cInToken will always be 0, so we can just put 0 as a constant instead
			tokens[cToken][1] = data[i+1];
			i++;
			cToken++;
		} else if (data[i] == '(' || data[i] == '\n' || data[i] == '\t' || data[i] == ')' || data[i] == '{' || data[i] == '}' || data[i] == '[' || data[i] == ']' || data[i] == '=' || data[i] == '>' || data[i] == '<' || data[i] == '+' || data[i] == '-' || data[i] == '%' || data[i] == '*' || data[i] == '/' || data[i] == '!' || data[i] == '<' || data[i] == '>' || data[i] == ':' || data[i] == ','){
			cToken++;
			cInToken = 0;
			tokens[cToken][0] = data[i]; // cInToken will always be 0, so we can just put 0 as a constant instead
			cToken++;
		} else if ((int)data[i] == -1){
			// ????? end of file weird artifact
		} else {
			tokens[cToken][cInToken] = data[i];
			cInToken++;
		}
	}

	fclose(file);

	// Combine the tokens into lines
	int numOfLines = 2; // we start with 2 to leave some fake buffer space at the end of the file
	int lineLength = 0;
	int largestLine = 0;
	for (int t = 0; t < tokenAmount; t++){
		if (strcmp(tokens[t], "\n") == 0){
			numOfLines ++;
			if (lineLength > largestLine) {
				largestLine = lineLength;
			}
			lineLength = 0;
		} else {
			lineLength ++;
		}
	}

	//largestLine = largestLine + 1; // To do with parsing later on, so last character doesn't get clipped.
	char lines[numOfLines][largestLine][15];
	for (int l = 0; l < numOfLines; l++){
		for (int t = 0; t < largestLine; t++){
			strcpy(lines[l][t], "");
		}
	}
	int currentLine = 0;
	int tokenInLine = 0;
	for (int t = 0; t < tokenAmount; t++){
		if (strcmp(tokens[t], "\n") == 0){
			currentLine++;
			tokenInLine = 0;
		} else {
			strcpy(lines[currentLine][tokenInLine], tokens[t]);
			tokenInLine++;
		}
	}

	// Delete NULLS from lines:
	char newLines[numOfLines][largestLine][15];
	for (int l = 0; l < numOfLines; l++){
		int actT = 0;
		for (int t = 0; t < largestLine; t++){
			strcpy(newLines[l][t], "");
			if (strcmp(lines[l][t], "") != 0){
				strcpy(newLines[l][actT], lines[l][t]);
				actT++;
			}
		}
	}

	for (int l = 0; l < numOfLines; l++){
		for (int t = 0; t < largestLine; t++){
			strcpy(lines[l][t], newLines[l][t]);
		}
	}


	// Analyze tokens
	enum TAML {ERROR, NONE, IDENTIFIER, OPERATOR, CONSTANT, TAB, NEWLINE, SPECIAL, BRACKET, BOOOLEAN, EQUALS_SIGN, COMMA, NUMBER}; // Token Analysation Markup Language
	int aTokens[numOfLines][largestLine];
	for (int l = 0; l < numOfLines; l++){
		for (int t = 0; t < largestLine; t++) {
			if (strcmp(lines[l][t], "\n") == 0){
				aTokens[l][t] = NEWLINE;
			} else if (strcmp(lines[l][t], "\t") == 0){
				aTokens[l][t] = TAB;
			} else if (strcmp(lines[l][t], "(") == 0 || strcmp(lines[l][t], ")") == 0 || strcmp(lines[l][t], "[") == 0 || strcmp(lines[l][t], "]") == 0 || strcmp(lines[l][t], "{") == 0 || strcmp(lines[l][t], "}") == 0){
				aTokens[l][t] = BRACKET;
			} else if (strcmp(lines[l][t], "==") == 0 || strcmp(lines[l][t], "!=") == 0 || strcmp(lines[l][t], ">=") == 0 || strcmp(lines[l][t], "<=") == 0 || strcmp(lines[l][t], "<") == 0 || strcmp(lines[l][t], ">") == 0 || strcmp(lines[l][t], "and") == 0 || strcmp(lines[l][t], "or") == 0){
				aTokens[l][t] = BOOOLEAN;
			} else if (strcmp(lines[l][t], "+") == 0 || strcmp(lines[l][t], "-") == 0 || strcmp(lines[l][t], "*") == 0 || strcmp(lines[l][t], "/") == 0 || strcmp(lines[l][t], "%") == 0) {
				aTokens[l][t] = OPERATOR;
			} else if (strcmp(lines[l][t], "+=") == 0 || strcmp(lines[l][t], "-=") == 0 || strcmp(lines[l][t], "*=") == 0 || strcmp(lines[l][t], "/=") == 0 || strcmp(lines[l][t], "=") == 0){
				aTokens[l][t] = EQUALS_SIGN;
			} else if (strcmp(lines[l][t], "int") == 0 || strcmp(lines[l][t], "bool") == 0 || strcmp(lines[l][t], "float") == 0 || strcmp(lines[l][t], "false") == 0 || strcmp(lines[l][t], "true") == 0){
				aTokens[l][t] = CONSTANT;
			} else if ((int)lines[l][t][0] > 47 && (int)lines[l][t][0] < 58) {
				aTokens[l][t] = NUMBER;
			} else if (strcmp(lines[l][t], "func") == 0 || strcmp(lines[l][t], "return") == 0 || strcmp(lines[l][t], "if") == 0 || strcmp(lines[l][t], "else") == 0 || strcmp(lines[l][t], "elif") == 0 || strcmp(lines[l][t], "for") == 0 || strcmp(lines[l][t], "while") == 0 || strcmp(lines[l][t], ":") == 0 || strcmp(lines[l][t], "in") == 0){
				aTokens[l][t] = SPECIAL;
			} else if (strcmp(lines[l][t], ",") == 0) {
				aTokens[l][t] = COMMA;
			} else if (strcmp(lines[l][t], "") == 0){
				aTokens[l][t] = NONE;
			} else {
				aTokens[l][t] = IDENTIFIER;
			}
		}
	}

	// Analyze Lines
	enum LAML {LINE_ERROR, IF_STATEMENT, ELSE_STATEMENT, FOR_LOOP, WHILE_LOOP, FUNCTION_DECLERATION, FUNCTION_RETURN, VARIABLE_INITIALISATION, VARIABLE_REDECLERATION, EMPTY}; // Line Analysation Markup Language
	int aLines[numOfLines];
	int tabAmounts[numOfLines];
	for (int l = 0; l < numOfLines; l++){
		aLines[l] = 0;
	}
	for (int l = 0; l < numOfLines; l++){
		int tabAmount = 0;
		for (int t = 0; t < largestLine; t++){
			if (strcmp(lines[l][t], "\t") == 0) {
				tabAmount += 1;
			}
		tabAmounts[l] = tabAmount; // For later use in the parser (so we don't have to do this again a thousand times)
		}
		
		if (aTokens[l][tabAmount] == SPECIAL) {
			if (strcmp(lines[l][tabAmount], "func") == 0) {
				aLines[l] = FUNCTION_DECLERATION;
			} else if (strcmp(lines[l][tabAmount], "for") == 0) {
				aLines[l] = FOR_LOOP;
			} else if (strcmp(lines[l][tabAmount], "while") == 0) {
				aLines[l] = WHILE_LOOP;
			} else if (strcmp(lines[l][tabAmount], "if") == 0) {
				aLines[l] = IF_STATEMENT;
			} else if (strcmp(lines[l][tabAmount], "else") == 0) {
				aLines[l] = ELSE_STATEMENT;
			} else if (strcmp(lines[l][tabAmount], "return") == 0) {
				aLines[l] = FUNCTION_RETURN;
			} else {
				aLines[l] = LINE_ERROR;
			}
		} else if (aTokens[l][tabAmount] == CONSTANT) {
			if (strcmp(lines[l][tabAmount], "int") == 0 || strcmp(lines[l][tabAmount], "bool") == 0|| strcmp(lines[l][tabAmount], "float") == 0) {
				aLines[l] = VARIABLE_INITIALISATION;
			}
		} else if (aTokens[l][tabAmount+1] == EQUALS_SIGN){
			aLines[l] = VARIABLE_REDECLERATION;
		}

		else if (strcmp(lines[l][tabAmount], "") == 0) {
			aLines[l] = EMPTY;
		} 
	}

	/*for (int l = 0; l < numOfLines; l++){
		switch (aLines[l]){
			case IF_STATEMENT:
				printf("\n-------- IF STATEMENT --------\n");
				break;
			case ELSE_STATEMENT:
				printf("\n-------- ELSE STATEMENT --------\n");
				break;
			case FOR_LOOP:
				printf("\n-------- FOR LOOP --------\n");
				break;
			case WHILE_LOOP:
				printf("\n-------- WHILE LOOP --------\n");
				break;
			case FUNCTION_DECLERATION:
				printf("\n-------- FUNCTION DECLERATION --------\n");
				break;
			case FUNCTION_RETURN:
				printf("\n-------- FUNCTION RETURN --------\n");
				break;
			case VARIABLE_INITIALISATION:
				printf("\n-------- VARIABLE INITIALISATION --------\n");
				break;
			case VARIABLE_REDECLERATION:
				printf("\n-------- VARIABLE REDECLERATION --------\n");
				break;
			case EMPTY:
				printf("\n-------- EMPTY --------\n");
				break;
			default:
				printf("\n-------- ???????????????????? --------\n");
				break;
		}

		printf("INDENTATION LEVEL: %i\n", (int)(aLines[l] / 10));
		for (int t = 0; t < largestLine; t++) {
			if (strcmp(lines[l][t], "") != 0) {
				if (aTokens[l][t] == NEWLINE){
					printf("\tNEWLINE: '\\n'\n");
				} else if (aTokens[l][t] == TAB){
					printf("\tTAB: '\\t'\n");
				} else if (aTokens[l][t] == IDENTIFIER){
					printf("\tIDENTIFIER: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == OPERATOR){
					printf("\tOPERATOR: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == CONSTANT){
					printf("\tCONSTANT: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == BRACKET){
					printf("\tBRACKET: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == SPECIAL) {
					printf("\tSPECIAL: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == BOOOLEAN){
					printf("\tBOOLEAN: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == EQUALS_SIGN) {
					printf("\tEQUALS SIGN: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == COMMA) {
					printf("\tCOMMA: '%s'\n", lines[l][t]);
				} else if (aTokens[l][t] == NUMBER) {
					printf("\tNUMBER: '%s'\n", lines[l][t]);
				} else {
					printf("\t???: '%s'\n", lines[l][t]);
				}
			}
		}
	}*/


	/* 									PARSER								*/
	Node *parseStacks[numOfLines][largestLine];

	// Create the tree Nodes for Parse Tree
	for (int l = 0; l < numOfLines; l++){
		Node *parseTree = (Node*) calloc(largestLine+1, sizeof(Node));
		Node *parseStack[largestLine];
		Node *RootNode = &parseTree[tabAmounts[l]];
		for (int t = 0; t < largestLine+1; t++){
			if (aTokens[l][t] != NONE){
				Node *node = &parseTree[t];
				node->type = aTokens[l][t];
				strcpy(node->data, lines[l][t]);
			}
		}
		for (int t = 0; t < largestLine; t++){
			parseStack[t] = &parseTree[t];
		}
		for (int t = largestLine-1; t > 0; t--) {
			if (parseStack[t]) {
				if (parseStack[t]->data[0] == *"(") {
					int closeBracket = lineParse(parseStack, t, largestLine);
					functionParametersParse(parseStack, t, closeBracket);
					parseStack[t] = 0;
					parseStack[closeBracket] = 0;
					compress_array(parseStack, largestLine);
				}
			}
		}
		lineParse(parseStack, 0, largestLine);
		functionParametersParse(parseStack, 0, largestLine);

		if (aLines[l] == VARIABLE_INITIALISATION) {
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
			parseStack[tabAmounts[l]+1]->Child1 = parseStack[tabAmounts[l]+2];
			parseStack[tabAmounts[l]+2]->Child1 = parseStack[tabAmounts[l]+3];

		} else if (aLines[l] == VARIABLE_REDECLERATION) {
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
			parseStack[tabAmounts[l]+1]->Child1 = parseStack[tabAmounts[l]+2];

		} else if (aLines[l] == IF_STATEMENT) {
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
			bool found = false;
			for (int t = largestLine-1; t > 0 && found == false; t--){
				if (parseStack[t]){
					if (parseStack[t]->data[0] != '\0'){
						found = true;
						parseStack[tabAmounts[l]]->Child2 = parseStack[t];
					}
				}
			}
		} else if (aLines[l] == ELSE_STATEMENT) {
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
			bool found = false;
			for (int t = largestLine-1; t > 0 && found == false; t--){
				if (parseStack[t]){
					if (parseStack[t]->data[0] != '\0'){
						found = true;
						parseStack[tabAmounts[l]]->Child2 = parseStack[t];
					}
				}
			}
		} else if (aLines[l] == FOR_LOOP){
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
			parseStack[tabAmounts[l]+1]->Child1 = parseStack[tabAmounts[l]+2];
			parseStack[tabAmounts[l]+2]->Child1 = parseStack[tabAmounts[l]+3];
			bool found = false;
			for (int t = largestLine-1; t > 0 && found == false; t--){
				if (parseStack[t]){
					if (parseStack[t]->data[0] != '\0'){
						found = true;
						parseStack[tabAmounts[l]]->Child2 = parseStack[t];
					}
				}
			}
		} else if (aLines[l] == WHILE_LOOP){
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
			bool found = false;
			for (int t = largestLine-1; t > 0 && found == false; t--){
				if (parseStack[t]){
					if (parseStack[t]->data[0] != '\0'){
						found = true;
						parseStack[tabAmounts[l]]->Child2 = parseStack[t];
					}
				}
			}
		} else if (aLines[l] == FUNCTION_RETURN){
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
		} else if (aLines[l] == FUNCTION_DECLERATION){
			RootNode->Child1 = parseStack[tabAmounts[l]+1];
			parseStack[tabAmounts[l]+1]->Child1 = parseStack[tabAmounts[l]+2];
			bool found = false;
			for (int t = largestLine-1; t > 0 && found == false; t--){
				if (parseStack[t]){
					if (parseStack[t]->data[0] != '\0'){
						found = true;
						parseStack[tabAmounts[l]]->Child2 = parseStack[t];
					}
				}
			}
		}

		/*if (aLines[l] != LINE_ERROR){
			printNodes(parseStack[tabAmounts[l]], 0);
			printf("\n\n------------------------------------------------------------------------------------------------------------------------\n\n");
		}*/
		//printNodes(parseStack[tabAmounts[l]], 0);
		for (int t = 0; t < largestLine; t++) {
			parseStacks[l][t] = parseStack[t];
		}
	}


	/*								COMPILING TO 6502								*/

	// First find the ends of loops and conditional statements
	int ends[numOfLines];
	for (int l = 0; l < numOfLines; l++){
		if (aLines[l] == IF_STATEMENT || aLines[l] == FOR_LOOP || aLines[l] == WHILE_LOOP || aLines[l] == ELSE_STATEMENT || aLines[l] == FUNCTION_DECLERATION) {
			bool quit = false;
			for (int fl = l+1; fl < numOfLines && quit == false; fl++){
				if (tabAmounts[fl] == tabAmounts[l] && aLines[fl] != EMPTY) {
					quit = true;
					ends[l] = fl;
					//printf("num: %i\n", fl);
				}
			}
			if (ends[l] > numOfLines){
				ends[l] = numOfLines;
			}
		} else {
			ends[l] = -1;
		}
	}
	enum VARTYPES {INTEGER, CHARACTER, BOLEAN, FLOATING};
	char currentFuncParams[10][25];
	char variables[256][25];
	int variableTypes[256];
	char functions[16][25];
	int currentFurthestFunc = 0;
	int currentFurthestVar = 0;
	// Then go through each line and turn it into assembly
	file = fopen("Test.asm", "w");
	fprintf(file, ".org 8000\nJMP main\n");
	for (int l = 0; l < numOfLines; l++) {
		for (int e = 0; e < numOfLines; e++) {
			if (ends[e] == l) {
				if (aLines[e] == IF_STATEMENT){
					fprintf(file, "BRANCH%c%c:\n", (char)((int) (e / 24)) + 65, (char) e % 24 + 65);
				} else if (aLines[e] == WHILE_LOOP) {
					char *text = calculateMath(*parseStacks[e], currentFuncParams, variables, variableTypes, tabAmounts[e]+2, functions);
					fprintf(file, "BRANCH%c%c:\n", (char)((int) (e / 24)) + 65, (char) e % 24 + 65);
					fprintf(file, "%s", text);
					fprintf(file, "LDA $0\nBNE WHILE%c%c\n", (char)((int) (e / 24)) + 65, (char) e % 24 + 65);
				}
			}
		}
		if (aLines[l] == FUNCTION_DECLERATION){
			fprintf(file, "%s:\n", parseStacks[l][1]->data);
			strcpy(functions[currentFurthestFunc], parseStacks[l][1]->data);
			currentFurthestFunc++;

		} else if (aLines[l] == VARIABLE_INITIALISATION) {
			char *text = calculateMath(*parseStacks[l], currentFuncParams, variables, variableTypes, tabAmounts[l]+2, functions);
			strcpy(variables[currentFurthestVar], parseStacks[l][tabAmounts[l]+1]->data);
			
			variableTypes[currentFurthestVar] = 0;
			if (strcmp(parseStacks[l][tabAmounts[l]]->data, "char") == 0) {
				variableTypes[currentFurthestVar] = 1;
			} else if (strcmp(parseStacks[l][tabAmounts[l]]->data, "bool") == 0) {
				variableTypes[currentFurthestVar] = 2;
			} else if (strcmp(parseStacks[l][tabAmounts[l]]->data, "float") == 0) {
				variableTypes[currentFurthestVar] = 3;
			}

			fprintf(file, "%s", text);
			fprintf(file, "LDX $0\nSTX %i\n", currentFurthestVar+13);
			currentFurthestVar++;
			free(text);
		} else if (aLines[l] == VARIABLE_REDECLERATION) {
			char *text = calculateMath(*parseStacks[l], currentFuncParams, variables, variableTypes, tabAmounts[l]+1, functions);
			fprintf(file, "%s", text);
			
			int location = -1;
			for (int i = 0; i < 256; i++){
				if (strcmp(parseStacks[l][tabAmounts[l]]->data, variables[i]) == 0){
					location = i + 13;
				}
			}
			if (location == -1){ // STILL?? (not found)
				printf("ERROR!!!");
				return -1;
			}

			if (parseStacks[l][tabAmounts[l]+1]->data[0] != '=') {
				if (parseStacks[l][tabAmounts[l]+1]->data[0] == '+'){
					fprintf(file, "LDA %i\nCLC\nADC 0\nSTA %i\n", location, location);
				} else if (parseStacks[l][tabAmounts[l]+1]->data[0] == '-') {
					fprintf(file, "LDA %i\nSEC\nSBC 0\nSTA %i\n", location, location);
				} else if (parseStacks[l][tabAmounts[l]+1]->data[0] == '*') {
					fprintf(file, "LDX %i\nSTX $02\nLDA 0\nSEC\nSBC #1\nSTA $01\nSTX $02\nSTX $00\nMULTI%c%c:\nLDA $00\nCLC\nADC $02\nSTA $00\nLDA $01\nSEC\nSBC #$1\nSTA $01\nBNE MULTI%c%c\nLDX $00\nSTX %i\n", location, (char)((int) (MultiLoop / 24)) + 65, (char) MultiLoop % 24 + 65, (char)((int) (MultiLoop / 24)) + 65, (char) MultiLoop % 24 + 65, location);
					MultiLoop++;
				} else if (parseStacks[l][tabAmounts[l]+1]->data[0] == '/') {
					fprintf(file, "LDX %i\nSTX $01\nLDX #$0\nSTX $02\nDIVIS%c%c:\nLDA $02\nCLC\nADC #1\nSTA $02\nLDA $01\nSEC\nSBC $00\nSTA $01\nBEQ DF%c%c\nBPL DIVIS%c%c\nLDA $02\nSEC\nSBC #1\nSTX $02\nDF%c%c:\nLDX $02\nSTX %i\n", location, (char)((int) (DivisLoop / 24)) + 65, (char) DivisLoop % 24 + 65, (char)((int) (DivisLoop / 24)) + 65, (char) DivisLoop % 24 + 65, (char)((int) (DivisLoop / 24)) + 65, (char) DivisLoop % 24 + 65, (char)((int) (DivisLoop / 24)) + 65, (char) DivisLoop % 24 + 65, location);
					DivisLoop++;
				}
			} else {
				fprintf(file, "LDX $0\nSTX %i\n", location);
			}

			free(text);
		} else if (aLines[l] == IF_STATEMENT) {
			char *text = calculateMath(*parseStacks[l], currentFuncParams, variables, variableTypes, tabAmounts[l]+2, functions);
			strcpy(variables[currentFurthestVar], parseStacks[l][tabAmounts[l]+1]->data);

			fprintf(file, "%s", text);
			fprintf(file, "LDX $0\nBEQ BRANCH%c%c\n", (char)((int) (l / 24)) + 65, (char) l % 24 + 65);
			free(text);
		} else if (aLines[l] == WHILE_LOOP) {
			char *text = calculateMath(*parseStacks[l], currentFuncParams, variables, variableTypes, tabAmounts[l]+2, functions);

			fprintf(file, "%s", text);
			fprintf(file, "LDX $0\nBEQ BRANCH%c%c\nWHILE%c%c:\n", (char)((int) (l / 24)) + 65, (char) l % 24 + 65, (char)((int) (l / 24)) + 65, (char) l % 24 + 65);

		} else if (aLines[l] == FUNCTION_RETURN) {
			char *text = calculateMath(*parseStacks[l], currentFuncParams, variables, variableTypes, tabAmounts[l]+1, functions);

			fprintf(file, "%s", text);
			fprintf(file, "RTS\n");
		}


		/*else if (aLines[l] == ELSE_STATEMENT) {
			fprintf(file, "ELSE%c%c:\n", (char)((int) (l / 24)) + 65, (char) l % 24 + 65);
		} */


	}

	fclose(file);


	return 0;
}








