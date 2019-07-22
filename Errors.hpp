//
//  basetypes.hpp
//  Database3
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Errors_h
#define Errors_h

namespace ECE141 {

    enum Errors {
        //parse related...
                keywordExpected = 100,
        identifierExpected = 110,
        unexpectedIdentifier = 115,
        keyExpected = 120,
        valueExpected = 130,
        unexpectedValue = 135,
        syntaxError = 140,
        expectedQuote = 145,
        eofError = 199,

        //statement related...
                statementExpected = 200,
        noDatabaseSpecified = 210,

        //table/DB related...
                tableExists = 300,
        unknownTable = 310,
        unknownDatabase = 320,
        cannotDescribeDatabase = 330,
        databaseExists = 340,
        cannotCreateDatabase = 350,

        //type related...
                unknownType = 400,
        unknownAttribute = 405,
        invalidAttribute = 410,
        invalidArguments = 420,
        invalidTypeConversion = 440,
        keyValueMismatch = 430, //# of fieldnames doesn't match values...

        //storage/io related...
                readError = 500,
        writeError = 510,
        seekError = 520,
        storageFull = 530,

        //command related...
                unknownCommand = 3000,
        invalidCommand = 3010,

        //general purpose...
                userTerminated = 4998,
        notImplemented = 4999,
        noError = 5000
    };


    struct StatusResult {
        Errors code;
        uint32_t value;

        StatusResult(Errors aCode = noError, uint32_t aValue = 0) : code(aCode), value(aValue) {}

        operator bool() { return Errors::noError == code; }
    };

}

#endif /* errors */
