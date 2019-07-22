//
//  Value.hpp
//  Database1
//
//  Created by rick gessner on 3/20/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Value_hpp
#define Value_hpp

#include <string>
#include <map>
#include <iostream>
#include "Errors.hpp"
#include "MemoryStream.hpp"

namespace ECE141 {

    //NOTE: integer is a SIGNED value
    //NOTE: timestamp is an UNSIGNED value!!!!!

    enum class DataType {
        no_type = 'N', int_type = 'I', float_type = 'F', bool_type = 'B', timestamp_type = 'T', varchar_type = 'V',
        //our enums are associated w/chars to be readable in encoded storage blocks...
    };

    //USE: define a dynamic type that allows us to hold field data in rows...
    class Value {
    public:
        //This class needs multiple ctors() for each type (book, timest, int, float varchar)...
        Value(); //default
        explicit Value(bool aBool);

        explicit Value(int anInt);

        explicit Value(int anInt, DataType aType);

        explicit Value(float aFloat);

        explicit Value(uint32_t aTime);

        explicit Value(const char *aChar);

        explicit Value(std::string aStr);

        // copy ctor
        Value(const Value &aCopy);

        ~Value();

        //      size_t getSize() {return size;}

        DataType getType() const { return type; }

        std::string toString() const;

        //This class needs operator=, for each basic type...
        //This class needs conversion operator for each basic type

        bool getBoolVal() const { return bool_val; }

        int getIntVal() const { return int_val; }

        float getFloatVal() const { return float_val; }

        unsigned int getTimeVal() const { return time_val; }

        std::string getVarcharVal() const { return varchar_val; }

        Value &operator=(const Value &aCopy);

        Value &operator=(const bool &aBoolean);

        Value &operator=(const uint32_t &aTimestamp);

        Value &operator=(const int32_t &anInt);

        Value &operator=(const float &aFloat);

        Value &operator=(const char *aString);

        Value &operator=(const std::string &aString);

        // ==
        bool operator==(const Value &aCopy) const;

        bool operator!=(const Value &aCopy) { return !(operator==(aCopy)); }

        // conversion operator
        StatusResult become(DataType aType); //cause the value to assume given type
        void debugDump(std::ostream &anOutput);  //describe (emit) to stream...

        static Value getDefault(DataType theType);

    private:

        DataType type;
        union {
            bool bool_val;
            int int_val;
            float float_val;
            unsigned int time_val;
        };

        std::string varchar_val;

        friend bool operator<(const Value &arg1, const Value &arg2);

        friend BufferReader &operator>>(BufferReader &istm, Value &aValue);

        friend BufferWriter &operator<<(BufferWriter &ostm, const Value &aValue);

    };

    using KeyValues = std::map<std::string, Value>; //a collection of key/values

}

#endif /* Value_h */
