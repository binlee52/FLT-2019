
/***************************************************************
 *      Header file of scanner routine for Mini C language     *
 *                                   2020. 5. 11               *
 ***************************************************************/

#ifndef SCANNER_H

#define SCANNER_H 1


#include <stdio.h>

#define NO_KEYWORD 16
#define ID_LENGTH 12
#define MAX_LENGTH 1000     // string type's max length
#define MAX_COMMENT_LENGTH 1000     // comment's max length


struct tokenType {
    int number;     // token number
    union {
        char id[ID_LENGTH];     // 변수
        int num;    // 정수
        double real;    // 실수
        char c;     // 문자
        char s[MAX_LENGTH];     // 문자열
        char comment[MAX_COMMENT_LENGTH];       // 주석
    } value;
};

enum tsymbol {
    tnull = -1,
    tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
    /* 0          1            2         3            4          5     */
    tand, tlparen, trparen, tmul, tmulAssign, tplus,
    /* 6          7            8         9           10         11     */
    tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
    /* 12         13          14        15           16         17     */
    tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
    /* 18         19          20        21           22         23     */
    tequal, tgreat, tgreate, tlbracket, trbracket, teof,
    /* 24         25          26        27           28         29     */
    //   ...........    word symbols ................................. //
    tconst, telse, tif, tint, treturn, tvoid,
    /* 30         31          32        33           34         35     */
    twhile, tlbrace, tor, trbrace, tchar, tdouble,
    /* 36         37          38        39           40         41     */
    tstring, tfor, tswitch, tcase, tdefault, tcontinue,
    /* 42         43          44        45           46         47     */
    tbreak, tcolon, tschar, tsstring, trealnumber, tlinecomment,
    /* 48         49          50        51           52         53     */
    tdocumentcomment, tmultilinecomment
    /* 54         55*/
};


int superLetter(char ch);
int superLetterOrDigit(char ch);
int getNumber(FILE *sourceFile, char firstCharacter);
double getRealNumber(int num);
int hexValue(char ch);
char getCharacter(FILE* sourceFile, char ch);
char* getString(FILE* sourceFile, char ch);
char* getLineComment(FILE* sourceFile, char ch);
char* getDocComment(FILE* sourceFile, char ch);
double getFloatingPoint(FILE* sourceFile, char ch, double num);
void deleteStar(char* s, char* s2, int maxLen);
void lexicalError(int n);
struct tokenType scanner(FILE *sourceFile);
void writeToken(struct tokenType token, FILE* outputFile);

#endif // !SCANNER_H

