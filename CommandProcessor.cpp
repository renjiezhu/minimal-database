//
//  CommandProcessor.cpp
//  Database3
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "CommandProcessor.hpp"
#include "Statement.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {

    CommandProcessor::CommandProcessor(IInterpreter *aNext) : next(aNext) {}

    CommandProcessor::~CommandProcessor() {}

    // USE: -----------------------------------------------------
    StatusResult CommandProcessor::interpret(const Statement &aStatement) {
        switch (aStatement.getType()) {
            case Keywords::quit_kw:std::cout << "Database is shutting down. Have a nice day." << std::endl;
                return StatusResult(ECE141::userTerminated);
                break;
            case Keywords::version_kw: std::cout << "ECEDB 1.0 - for ECE 141B final" << std::endl;
                break;
            case Keywords::help_kw:
                std::cout << "  version -- shows the current version of this application\n"
                             "  help -- shows this list of commands\n"
                             "  quit -- terminates the execution of this DB application"
                          << std::endl;
                break;
            default: break;
        }
        return StatusResult();
    }

    // USE: retrieve a statement based on given text input...
    Statement *CommandProcessor::getStatement(Tokenizer &aTokenizer) {

        if (aTokenizer.more()) {
            Token &theToken = aTokenizer.current();
            switch (theToken.keyword) {
                case Keywords::help_kw    :
                case Keywords::version_kw :
                case Keywords::quit_kw    :aTokenizer.next(); //skip ahead...
                    return new Statement(theToken.keyword);

                default: break;
            }
        }
        return nullptr;
    }

    // USE: parse tokens in tokenizer and create assocated statement;
    //      If you can't do it, ask your next (chain-of-resp) to try...
    StatusResult CommandProcessor::processInput(Tokenizer &aTokenizer) {
        std::unique_ptr<Statement> theStatement(getStatement(aTokenizer));
        if (theStatement) {
            return interpret(*theStatement);
        } else if (next) {
            return next->processInput(aTokenizer);
        }
        return StatusResult{ECE141::unknownCommand};
    }

    // USE: retrieve active db (if available) to subystem...
    Database *CommandProcessor::getActiveDatabase() {
        return next ? next->getActiveDatabase() : nullptr;
    }

}
