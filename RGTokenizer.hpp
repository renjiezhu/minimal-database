//
//  RGTokenizer.hpp
//  Database3
//
//  Created by rick gessner on 3/24/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef RGTokenizer_hpp
#define RGTokenizer_hpp

#include "Tokenizer.hpp"
#include "Value.hpp"
#include "Helpers.hpp"

namespace ECE141 {


    class RGTokenizer : public Tokenizer {
    public:

        RGTokenizer(std::istream &anInputStream);

        virtual StatusResult tokenize();

        void dump();

        std::string readWhile(callback aCallback);

        std::string readUntil(char aTerminal, bool addTerminal = false);

        std::string readUntil(callback aCallback, bool addTerminal = false);

    };

}

#endif /* RGTokenizer_hpp */
