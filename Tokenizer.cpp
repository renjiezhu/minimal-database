//
//  Tokenizer.cpp
//  Database3
//
//  Created by rick gessner on 3/19/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Tokenizer.hpp"
#include <exception>

namespace ECE141 {

    Tokenizer::Tokenizer(std::istream &anInput) : input(anInput), index(0) {}

    bool isOperator(int anInt);

    std::string getOperator(std::istream &input);

    bool isNumber(int anInt);

    bool isDigit(int anInt);

    //convert input given in constructor to collection of tokens...
    StatusResult Tokenizer::tokenize() {
        StatusResult theResult;

        while (!input.eof()) {
            // still missing ',', ';' and some more
            int curr_char = input.peek();

            if (isOperator(curr_char)) { // operators
                std::string op = getOperator(input);
//                op.compare("b");
//                if ( (!op.compare("==")) or (!op.compare("=")) or (!op.compare(">")) or (!op.compare(">=")) or (!op.compare("<")) or (!op.compare("<=")) or (!op.compare("!"))) {
//                    tokens.push_back(Token(TokenType::operators, op));
//                }
//                else
//                {
//                    std::cout<<"TO DO: Handle error. Invalid Input: Unknown Operator";
//                }
                tokens.push_back(Token(TokenType::operators, op));
            } else if (isNumber(curr_char)) { // numbers
                std::string op(1, (char) input.get());
                while (isDigit(input.peek())) {
                    op += (char) input.get();
                }
                tokens.push_back(Token(TokenType::number, op));
            } else if ('(' == (char) curr_char) { // capture '('
                input.get();
                tokens.push_back(Token(TokenType::lparen, "("));
            } else if (')' == (char) curr_char) { // capture '('
                input.get();
                tokens.push_back(Token(TokenType::rparen, ")"));
            } else if ('{' == (char) curr_char) { // capture '('
                input.get();
                tokens.push_back(Token(TokenType::lbrace, "{"));
            } else if ('}' == (char) curr_char) { // capture ')'
                input.get();
                tokens.push_back(Token(TokenType::rbrace, "}"));
            } else if ('[' == (char) curr_char) { // capture '['
                input.get();
                tokens.push_back(Token(TokenType::lbracket, "["));
            } else if (']' == (char) curr_char) { // capture ']'
                input.get();
                tokens.push_back(Token(TokenType::rbracket, "]"));
            } else if (',' == (char) curr_char) { // capture ','
                input.get();
                tokens.push_back(Token(TokenType::comma, ","));
            } else if (';' == (char) curr_char) { // capture ';'
                input.get();
                tokens.push_back(Token(TokenType::semicolon, ";"));
            } else if (':' == (char) curr_char) { // capture ';'
                input.get();
                tokens.push_back(Token(TokenType::colon, ":"));
            } else if ('\'' == (char) curr_char) { // capture ';'
                input.get();
                tokens.push_back(Token(TokenType::apostrophe, "'"));
            } else if ('/' == (char) curr_char) { // capture ';'
                input.get();
                tokens.push_back(Token(TokenType::slash, "/"));
//            } else if ('='==(char)curr_char) { // capture ';'
//                input.get();
//                tokens.push_back(Token(TokenType::equal, "="));
            } else if ('+' == (char) curr_char) { // capture ';'
                input.get();
                tokens.push_back(Token(TokenType::plus, "+"));
            } else if ('-' == (char) curr_char) { // capture ';'
                input.get();
                tokens.push_back(Token(TokenType::minus, "-"));
            } else if ('"' == (char) curr_char) { //quoted string
                std::string op(1, (char) input.get());
                while ('"' != (char) input.peek()) {
                    if (-1 == input.peek()) { // end of command w/o closing "
                        std::cout << "TO DO: Handle error. Invalid Input: Quoted string without a closing quote"
                                  << std::endl;
                        break;
                    }
                    op += input.get();
                }
                op += input.get(); //Appending the closing "
                std::cout << "TO DO: Handle error so that only closing quote reaches here" << std::endl;
                tokens.push_back(Token(TokenType::string, op));
            } else if (' ' == curr_char) { // extra spaces
                input.get();
            } else { // string patterns
                if (-1 == curr_char)
                    break;
                char cur = input.get();
                std::string op(1, cur);
                while (' ' != input.peek() and -1 != input.peek() and '"' != input.peek() and '(' != input.peek() and
                       ')' != input.peek() and '[' != input.peek() and ']' != input.peek() and ',' != input.peek() and
                       ';' != input.peek() and !isOperator(input.peek())) {
                    op += (char) input.get();
                }
                // TODO (ashin) : can you find a way to keep a copy of the original string
                // so that identifier gets the original string. Here I convert all into
                // lowercase so that keywords (usually in uppercase) can be correctly recognized.
                if (gDictionary.count(op) > 0) { // keyword
                    // for keywords, case is not decisive
                    tokens.push_back(Token(TokenType::keyword, op, gDictionary[op]));
                } else { // identifiers
                    tokens.push_back(Token(TokenType::identifier, op));
                }
            }


        }

//        for (auto o : tokens) {
//            std::cout << o.data << std::endl;
//        }
        return theResult;
    }

    Token &Tokenizer::tokenAt(int anOffset) {
        if (anOffset < size()) return tokens[anOffset];
        throw std::range_error("bad offset");
    }

    bool Tokenizer::next(int anOffset) {
        if (index + anOffset <= size()) {
            index += anOffset;
            return true;
        }
        return false;
    }

    Token &Tokenizer::current() {
        return tokens[index];
    }

    Token &Tokenizer::peek(int anOffset) {
        return tokenAt(index + anOffset);
    }
    // helper functions

    bool isOperator(int anInt) {
        // check if this is an operator (=,!,<,>)
        if (anInt == 33 or (anInt >= 60 and anInt <= 62)) {
            return true;
        }
        return false;
    }

    std::string getOperator(std::istream &input) {
        // call only if the first is an operator (=,!,<,>)
        std::string op(1, (char) input.get());
        if (input.peek() == 61) {
            // if next char is =, add = and make a 2-char operator
            op += (char) input.get();
        }
        return op;
    }

    bool isNumber(int anInt) {
        // check if this is a number (0~9)
        if (anInt >= 48 and anInt <= 57) {
            return true;
        }
        return false;
    }

    bool isDigit(int anInt) {
        // check if anInt is a digit (0~9 and .)
        if (isNumber(anInt) or anInt == 46) { // 46 : '.'
            return true;
        }
        return false;
    }

}
