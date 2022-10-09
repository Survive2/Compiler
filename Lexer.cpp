
/*

a Simple compiler for training for me to start learning in browser exploit

Start Time: Sep 19 2022

by h1J4cker

*/

#include <stdio.h>
#include <iostream>
#include <string.h>

enum Tokenizer
{
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5
};
static std::string IdentifierStr;
static double NumVal;


/*

Day_1 Info:

Content: A Simple Lexer is Done!

start at 20:30
end at 21:30

*/

static int __Get_Tok()
{
    static int LastChar = " ";

    while (isspace(LastChar))
    {
        LastChar = getchar();
    }

    if (isalpha(LastChar))
    {
        while (isalnum(LastChar = getchar()))
        {
            IdentifierStr += LastChar;
        }

        if (IdentifierStr == "def")
        {
            return tok_def;
        }
        else if (IdentifierStr == "extern")
        {
            return tok_extern;
        }
        else
        {
            return tok_identifier;
        }
    }

    if (isdigit(LastChar) || LastChar == ".")
    {

        std::string NumStr;

        do
        {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == ".");

        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }

    if (LastChar == "#")
    {
        do
        {
            LastChar = getchar();
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
        {
            return __Get_Tok();
        }
    }

    if (LastChar == EOF)
    {
        return tok_eof;
    }

    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}