/***************************************************************
 *      Scanner routine for Mini C language                    *
 *                                   2020. 5. 11               *
 ***************************************************************/



#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "Scanner.h"

enum tsymbol tnum[NO_KEYWORD] = {
    tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
    tchar,     tdouble,   tstring, tfor,     tswitch,   tcase,     tdefault,
    tcontinue, tbreak
};




char* tokenName[] = {
    "!",        "!=",      "%",       "%=",     "%ident",   "%number",
    /* 0          1           2         3          4          5        */
    "&&",       "(",       ")",       "*",      "*=",       "+",
    /* 6          7           8         9         10         11        */
    "++",       "+=",      ",",       "-",      "--",	    "-=",
    /* 12         13         14        15         16         17        */
    "/",        "/=",      ";",       "<",      "<=",       "=",
    /* 18         19         20        21         22         23        */
    "==",       ">",       ">=",      "[",      "]",        "eof",
    /* 24         25         26        27         28         29        */
    //   ...........    word symbols ................................. //
    /* 30         31         32        33         34         35        */
    "const",    "else",     "if",      "int",     "return",  "void",
    /* 36         37         38        39         40         41        */
    "while",    "{",        "||",       "}",      "char",    "double",
    /* 42         43         44        45         46         47        */
    "string",   "for",      "switch",   "case",    "default", "continue",
    /* 48                                                              */
    "break", ":", "%char", "%string", "%real_number", "linecomment",
    "docstringcomment", "multilinecomment"
};

char* keyword[NO_KEYWORD] = {
    "const",  "else",    "if",    "int",    "return",  "void",    "while",
    "char", "double", "string", "for", "switch", "case", "default", "continue",
    "break"};

struct tokenType scanner(FILE *sourceFile)
{
    struct tokenType token;
    int i, index;
    char ch, id[ID_LENGTH];
    token.number = tnull;

    do {
        while (isspace(ch = fgetc(sourceFile)));	// state 1: skip blanks
        if (superLetter(ch)) { // identifier or keyword
            i = 0;
            do {
                if (i < ID_LENGTH) id[i++] = ch;
                ch = fgetc(sourceFile);
            } while (superLetterOrDigit(ch));
            if (i >= ID_LENGTH) lexicalError(1);
            id[i] = '\0';
            ungetc(ch, sourceFile);  //  retract
            // find the identifier in the keyword table
            for (index = 0; index < NO_KEYWORD; index++)
                if (!strcmp(id, keyword[index])) break;
            if (index < NO_KEYWORD)    // found, keyword exit
                token.number = tnum[index];
            else {                     // not found, identifier exit
                token.number = tident;
                strcpy_s(token.value.id, ID_LENGTH, id);
            }
        }  // end of identifier or keyword
        else if (isdigit(ch)) {  // number
            token.value.num = getNumber(sourceFile, ch);
            ch = fgetc(sourceFile);
            if (ch != '.')
            {
                token.number = tnumber;
                if (ch == 'e' || ch == 'E')
                {
                    token.number = trealnumber;
                    token.value.real = getFloatingPoint(sourceFile, ch, token.value.num);
                 }
            }
            else
            {
                token.number = trealnumber;
                ch = fgetc(sourceFile);
                if (isdigit(ch))
                {
                    token.value.real = token.value.num + getRealNumber(getNumber(sourceFile, ch));
                    ch = fgetc(sourceFile);
                }
                else
                    lexicalError(8);
                
                if (ch == 'e' || ch == 'E') //TODO:
                    token.value.real = getFloatingPoint(sourceFile, ch, token.value.real);
            }
        }
        else switch (ch) {  // special character
        case '/':
            ch = fgetc(sourceFile);
            if (ch == '*')			// text comment
            {
                char temp[MAX_COMMENT_LENGTH];
                strcpy_s(temp, MAX_COMMENT_LENGTH, getDocComment(sourceFile, ch));
                if (*temp == '*') // document comment
                {
                    *temp = *(temp + 1); // document comment는 /** */와 같은 구조이므로 token.value.comment에 저장될 때 *하나를 추가 삭제해야 함
                    token.number = tdocumentcomment;
                    strcpy_s(token.value.comment, MAX_COMMENT_LENGTH, temp);
                }
                else // multilinecomment
                {
                    token.number = tmultilinecomment;
                    strcpy_s(token.value.comment, MAX_COMMENT_LENGTH, temp);
                }
            }
            else if (ch == '/')		// line comment
            {
                token.number = tlinecomment;
                strcpy_s(token.value.comment, MAX_COMMENT_LENGTH, getLineComment(sourceFile, ch));
            }
            else if (ch == '=')  token.number = tdivAssign;
            else {
                token.number = tdiv;
                ungetc(ch, sourceFile); // retract
            }
            break;
        case '!':
            ch = fgetc(sourceFile);
            if (ch == '=')  token.number = tnotequ;
            else {
                token.number = tnot;
                ungetc(ch, sourceFile); // retract
            }
            break;
        case '%':
            ch = fgetc(sourceFile);
            if (ch == '=') {
                token.number = tremAssign;
            }
            else {
                token.number = tremainder;
                ungetc(ch, sourceFile);
            }
            break;
        case '&':
            ch = fgetc(sourceFile);
            if (ch == '&')  token.number = tand;
            else {
                lexicalError(2);
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '*':
            ch = fgetc(sourceFile);
            if (ch == '=')  token.number = tmulAssign;
            else {
                token.number = tmul;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '+':
            ch = fgetc(sourceFile);
            if (ch == '+')  token.number = tinc;
            else if (ch == '=') token.number = taddAssign;
            else {
                token.number = tplus;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '-':
            ch = fgetc(sourceFile);
            if (ch == '-')  token.number = tdec;
            else if (ch == '=') token.number = tsubAssign;
            else {
                token.number = tminus;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '<':
            ch = fgetc(sourceFile);
            if (ch == '=') token.number = tlesse;
            else {
                token.number = tless;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '=':
            ch = fgetc(sourceFile);
            if (ch == '=')  token.number = tequal;
            else {
                token.number = tassign;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '>':
            ch = fgetc(sourceFile);
            if (ch == '=') token.number = tgreate;
            else {
                token.number = tgreat;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '|':
            ch = fgetc(sourceFile);
            if (ch == '|')  token.number = tor;
            else {
                lexicalError(3);
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '\'':
            token.number = tschar;
            token.value.c = getCharacter(sourceFile, ch);
            break;
        case '\"': // TODO
            token.number = tsstring;
            strcpy_s(token.value.s, MAX_LENGTH, getString(sourceFile, ch));
            break;

        case '(': token.number = tlparen;         break;
        case ')': token.number = trparen;         break;
        case ',': token.number = tcomma;          break;
        case ';': token.number = tsemicolon;      break;
        case '[': token.number = tlbracket;       break;
        case ']': token.number = trbracket;       break;
        case '{': token.number = tlbrace;         break;
        case '}': token.number = trbrace;         break;
        case ':': token.number = tcolon;          break;
        case EOF: token.number = teof;            break;
        default: {
            printf("Current character : %c", ch);
            lexicalError(4);
            break;
        }

        } // switch end
    } while (token.number == tnull);
    return token;
} // end of scanner

void lexicalError(int n)
{
    printf(" *** Lexical Error : ");
    switch (n) {
    case 1: printf("an identifier length must be less than 12.\n");
        break;
    case 2: printf("next character must be &\n");
        break;
    case 3: printf("next character must be |\n");
        break;
    case 4: printf("invalid character\n");
        break;
    case 5: printf("missing closing quote\n");
        break;
    case 6: printf("string length must be less than 12.\n");
        break;
    case 7: printf("You need to close document\n");
        break;
    case 8: printf("invalid number\n");
        break;
    default: printf("Error.\n");
        break;
    }
}

int superLetter(char ch)
{
    if (isalpha(ch) || ch == '_') return 1;
    else return 0;
}

int superLetterOrDigit(char ch)
{
    if (isalnum(ch) || ch == '_') return 1;
    else return 0;
}

int getNumber(FILE *sourceFile, char firstCharacter)
{
    int num = 0;
    int value;
    char ch;

    if (firstCharacter == '0') {
        ch = fgetc(sourceFile);
        if ((ch == 'X') || (ch == 'x')) {		// hexa decimal
            while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
                num = 16 * num + value;
        }
        else if ((ch >= '0') && (ch <= '7'))	// octal
            do {
                num = 8 * num + (int)(ch - '0');
                ch = fgetc(sourceFile);
            } while ((ch >= '0') && (ch <= '7'));
        else num = 0;						// zero
    }
    else
    {									// decimal
        ch = firstCharacter;
        do {
                num = 10 * num + (int)(ch - '0');
                ch = fgetc(sourceFile);
        } while (isdigit(ch));
    }
    ungetc(ch, sourceFile);  /*  retract  */
    return num;
}

int hexValue(char ch)
{
    switch (ch) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return (ch - '0');
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return (ch - 'A' + 10);
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return (ch - 'a' + 10);
    default: return -1;
    }
}

char getCharacter(FILE* sourceFile, char ch) // file에서 문자 추출 후 반환
{
    char c = '\0';
    if (ch == '\'')
    {
        ch = fgetc(sourceFile);
        c = ch;
        ch = fgetc(sourceFile);
    }

    if (ch != '\'') // error 예외처리
    {
        lexicalError(5);
        ungetc(ch, sourceFile);
    }
    return c;
}

char* getString(FILE* sourceFile, char ch) // file에서 문자열 추출 후 반환
{
    static char s[MAX_LENGTH]; // 문자열 반환을 위한 주소값 유지를 위해 static 변수로 반환
    int i = 0;
    if (ch == '\"')
    {
        ch = fgetc(sourceFile);
        while (ch != '\"' && i < MAX_LENGTH - 1)
        {
            s[i++] = ch;
            ch = fgetc(sourceFile);
        }
        if (ch != '\"') // error 예외처리
        {
            lexicalError(6);
            ungetc(ch, sourceFile);
        }
        ch = fgetc(sourceFile);
    }
    return s;
}


char* getLineComment(FILE* sourceFile, char ch)
/**  한 줄로 된 line comment 반환**/
{
    int i = 0;
    static char s[MAX_COMMENT_LENGTH];
    ch = fgetc(sourceFile);
    // '/'를 제외한 comment만 반환ㄱㄱ
    while (ch != '\n' && i < MAX_COMMENT_LENGTH - 1) // \n이 나오거나 범위를 초과할 때까지
    {
        s[i++] = ch;
        ch = fgetc(sourceFile);
    }
    return s;
}

char* getDocComment(FILE* sourceFile, char ch)
/** multi line comment 반환 **/
{
    int i = 0;

    char doc1[MAX_COMMENT_LENGTH];
    static char doc2[MAX_COMMENT_LENGTH];
    doc1[i++] = ch;
    ch = fgetc(sourceFile);

    do {
        while (ch != '*' && ch != -1)
        {
            if (ch == '\n')     // '\n'을 \n으로 출력하기 위해 조작
            {
                doc1[i++] = '\\';
                doc1[i++] = 'n';
            }
            else
                doc1[i++] = ch;
            ch = fgetc(sourceFile);
         }
        if (ch == -1)
            break;
        doc1[i++] = ch;
        ch = fgetc(sourceFile);
    } while (ch != '/');
    if (ch == -1) // 주석이 닫히지 않으면
        lexicalError(7);
    deleteStar(&doc1[0], &doc2[0], i); // multi line comment를 나타내는 /* */ 의 '*' 삭제
    return doc2;
}

void deleteStar(char* str1, char* str2, int maxLen)
/** multi line comment 수정 함수 **/
{
    for (int x = 0; x < maxLen / 2; x++)
    {
        if (*(str1+ x) == '*' && *(str1 +maxLen - x - 1) == '*') // 앞 뒤가 모두 * 이면 삭제
            *(str1 + x) = '\0', *(str1 + maxLen - x - 1) = '\0';
        else // 문자가 등장하면
            break;
    }
    int j = 0;
    for (int x = 0; x < maxLen; x++)
    {
        if (*(str1+x) != '\0')
            *(str2 + j++) = *(str1+x);
    }
}

double getFloatingPoint(FILE* sourceFile, char ch, double num)
/** 부동소수 반환 **/
{
    double floatNum = num;
    ch = fgetc(sourceFile);
    if (isdigit(ch) || ch == '+')
    {
        if (ch == '+') ch = fgetc(sourceFile);
        num = getNumber(sourceFile, ch);
        floatNum *= pow(10, num);
        ch = fgetc(sourceFile);

    }
    else if (ch == '-')
    {
        ch = fgetc(sourceFile);
        num = getNumber(sourceFile, ch);
        floatNum /= pow(10, num);
        ch = fgetc(sourceFile);
    }
    else
        lexicalError(4);

    // 4.2e4-2와 같이 지수부에 정수가 들어가지 않았을 때
    if (ch != ';' && ch != '\t' && ch != '\n' && ch != '.')
        lexicalError(4);
    return floatNum;
}

void writeToken(struct tokenType token, FILE *outputFile)
{
    if (token.number == tident) {
        fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.id);
    }
    else if (token.number == tnumber) {
        fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.num);
    }
    else if (token.number == trealnumber) {
        fprintf(outputFile, "Token %10s ( %3d, %12lf)\n", tokenName[token.number], token.number, token.value.real);
    }
    else if (token.number == tschar) {
        fprintf(outputFile, "Token %10s ( %3d, %12c)\n", tokenName[token.number], token.number, token.value.c);
    }
    else if (token.number == tsstring) {
        fprintf(outputFile, "Token %10s ( %3d, %12s)\n", tokenName[token.number], token.number, token.value.s);
    }
    else if (token.number == tlinecomment) {
        fprintf(outputFile, "Token %10s ( %3d, %s)\n", tokenName[token.number], token.number, token.value.comment);
    }
    else if (token.number == tdocumentcomment) {
        fprintf(outputFile, "Token %10s ( %3d, %s)\n", tokenName[token.number], token.number, token.value.comment);
    }
    else if (token.number == tmultilinecomment) {
        fprintf(outputFile, "Token %10s ( %3d, %s)\n", tokenName[token.number], token.number, token.value.comment);
    }
    else {
        fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, "0");
    }
    
}

double getRealNumber(int num) // 실수부 반환
{
    // '.' 뒤에 나오는 소수부를 정수로 입력 받아 실수로 반환한다.
    char temp[1024] = { 0, };
    int len = sprintf_s(temp, 1024, "%d", num); // 소수 자릿수 계산
    return num / pow(10, len);
}