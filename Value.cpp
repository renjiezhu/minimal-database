//
//  Value.cpp
//  Database1
//
//  Created by rick gessner on 4/8/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Value.hpp"

namespace ECE141 {

    Value::Value() {
        type = DataType::int_type;
        int_val = 0;
    }

    Value::Value(bool aBool) {
        type = DataType::bool_type;
        bool_val = aBool;
    }


    Value::Value(int anInt) {
        type = DataType::int_type;
        int_val = anInt;
    }

    Value::Value(int anInt, DataType aType) {
        type = aType;

        switch (aType) {
            case DataType::int_type :int_val = anInt;
                break;
            case DataType::float_type :float_val = (float) anInt;
                break;
            case DataType::timestamp_type :time_val = (unsigned int) anInt;
                break;
            case DataType::bool_type :bool_val = (bool) anInt;
                break;
            case DataType::varchar_type :varchar_val = std::to_string(anInt);
                break;
            case DataType::no_type :break;
            default:break;
        }
    }

    Value::Value(uint32_t aTime) {
        type = DataType::timestamp_type;
        time_val = aTime;
    }

    Value::Value(float aFloat) {
        type = DataType::float_type;
        float_val = aFloat;
    }

    Value::Value(std::string aStr) {
        type = DataType::varchar_type;
        varchar_val = aStr;
    }

    Value::Value(const char *aChar) { // problems here, probably
        type = DataType::varchar_type;
        varchar_val = aChar;
    }

    Value::Value(const Value &aCopy) {
        type = aCopy.type;

        switch (type) {
            case DataType::int_type :int_val = aCopy.int_val;
                break;
            case DataType::float_type :float_val = aCopy.float_val;
                break;
            case DataType::timestamp_type :time_val = aCopy.time_val;
                break;
            case DataType::bool_type :bool_val = aCopy.bool_val;
                break;
            case DataType::varchar_type :varchar_val = aCopy.varchar_val;
                break;
            case DataType::no_type :break;
            default:break;
        }

    }


    //DONT FORGET!
    Value::~Value() {}

    // operator=
    Value &Value::operator=(const Value &other) {
        type = other.type;
        switch (type) {
            case DataType::int_type : return *this = other.int_val;
            case DataType::float_type : return *this = other.float_val;
            case DataType::timestamp_type : return *this = other.time_val;
            case DataType::bool_type : return *this = other.bool_val;
            case DataType::varchar_type : return *this = other.varchar_val;
            case DataType::no_type :break;
            default: break;
        }
        return *this;
    }

    Value &Value::operator=(const bool &aBoolean) {
        type = DataType::bool_type;
        bool_val = aBoolean;
        return *this;
    }

    Value &Value::operator=(const uint32_t &aTimestamp) {
        type = DataType::timestamp_type;
        time_val = aTimestamp;
        return *this;
    }

    Value &Value::operator=(const int32_t &anInt) {
        type = DataType::int_type;
        int_val = anInt;
        return *this;
    }

    Value &Value::operator=(const float &aFloat) {
        type = DataType::float_type;
        float_val = aFloat;
        return *this;
    }

    Value &Value::operator=(const char *aString) {
        type = DataType::varchar_type;
        varchar_val = aString;
        return *this;
    }

    Value &Value::operator=(const std::string &aString) {
        type = DataType::varchar_type;
        varchar_val = aString;
        return *this;
    }

    std::string Value::toString() const {
        switch (type) {
            case DataType::int_type : return std::to_string(int_val);
            case DataType::float_type : return std::to_string(float_val);
            case DataType::timestamp_type : return std::to_string(time_val);
            case DataType::bool_type : return std::to_string(bool_val);
            case DataType::varchar_type : return varchar_val;
            default : return "";
        }
    }


    // conversion TODO
    StatusResult Value::become(DataType aType) {//cause the value to assume given type
        StatusResult theResult{noError};

        if (type == aType) {
            return theResult;
        }

        try {
            switch (aType) {
                case DataType::int_type :int_val = std::stoi(toString());
                    type = DataType::int_type;
//                    int_val = std::stoi(varchar_val);
                    break;

                case DataType::float_type :float_val = std::stof(toString());
                    type = DataType::float_type;
                    break;
                case DataType::timestamp_type :
                    if (std::stoi(toString()) < 0) {
                        theResult.code = Errors::invalidTypeConversion;
                        break;
                    }
                    time_val = (unsigned int) std::stoi(toString());
                    type = DataType::timestamp_type;
                    break;
                case DataType::bool_type :bool_val = std::stoi(toString());
                    type = DataType::bool_type;
                    break;
                case DataType::varchar_type :varchar_val = toString();
                    type = DataType::varchar_type;
                    break;
                case DataType::no_type :theResult.code = Errors::unknownType;
                    break;
                default:theResult.code = Errors::invalidTypeConversion;
                    break;
            }
        }

        catch (...) {
            theResult.code = Errors::invalidTypeConversion;
        }

        return theResult;
    }

    bool Value::operator==(const ECE141::Value &aCopy) const {
        if (type == aCopy.getType()) {
            switch (type) {
                case DataType::int_type   : return (int_val == aCopy.getIntVal());
                case DataType::bool_type  : return (bool_val == aCopy.getBoolVal());
                case DataType::float_type : return (float_val == aCopy.getFloatVal());
                case DataType::timestamp_type : return (time_val == aCopy.getTimeVal());
                case DataType::varchar_type : return (varchar_val == aCopy.getVarcharVal());
                default: break;
            }
        }

        return false;
    }

    template<typename T>
    bool isSmaller(T &arg1, T &arg2) {
        return arg1 < arg2;
    }

    bool operator<(const ECE141::Value &arg1, const ECE141::Value &arg2) {

        if (arg1.type == arg2.type) {
            switch (arg1.type) {
                case DataType::int_type   : return isSmaller(arg1.int_val, arg2.int_val);
                case DataType::bool_type  : return isSmaller(arg1.bool_val, arg2.bool_val);
                case DataType::float_type : return isSmaller(arg1.float_val, arg2.float_val);
                case DataType::timestamp_type : return isSmaller(arg1.time_val, arg2.time_val);
                case DataType::varchar_type : return isSmaller(arg1.varchar_val, arg2.varchar_val);
                default: break;
            }
        }
        return false;
    }

    BufferReader &operator>>(BufferReader &istm, Value &aValue) {
        char theCharType;
        istm >> theCharType;

        aValue.type = static_cast<DataType>(theCharType);

        switch (aValue.type) {
            case DataType::int_type   :uint32_t intval;
                istm >> intval;
                aValue.int_val = static_cast<int>(intval);
                return istm;
            case DataType::bool_type  :istm >> aValue.bool_val;
                return istm;
            case DataType::float_type :istm >> aValue.float_val;
                return istm;
            case DataType::timestamp_type :istm >> aValue.time_val;
                return istm;
            case DataType::varchar_type :istm >> aValue.varchar_val;
                return istm;
            default: return istm;
        }


    }

    BufferWriter &operator<<(BufferWriter &ostm, const Value &aValue) {

        char type = static_cast<char>(aValue.getType());
        ostm << type;

        switch (aValue.getType()) {
            case DataType::int_type   :ostm << static_cast<uint32_t>(aValue.getIntVal());
                return ostm;
            case DataType::bool_type  :ostm << static_cast<bool>(aValue.getBoolVal());
                return ostm;
            case DataType::float_type :ostm << static_cast<float>(aValue.getFloatVal());
                return ostm;
            case DataType::timestamp_type :ostm << static_cast<uint32_t>(aValue.getTimeVal());
                return ostm;
            case DataType::varchar_type :ostm << aValue.getVarcharVal();
                return ostm;
            default: return ostm;
        }

    }


    void Value::debugDump(std::ostream &anOutput) {
        std::string type_Str;
        switch (type) {
            case DataType::int_type :anOutput << "type: int, value: " << int_val << std::endl;
                break;
            case DataType::float_type :anOutput << "type: float, value: " << float_val << std::endl;
                break;
            case DataType::timestamp_type :anOutput << "type: timestamp, value: " << time_val << std::endl;
                break;
            case DataType::bool_type :anOutput << "type: bool, value: " << bool_val << std::endl;
                break;
            case DataType::varchar_type :anOutput << "type: varchar, value: " << varchar_val << std::endl;
                break;
            case DataType::no_type :break;

            default:break;
        }

    }

    Value Value::getDefault(DataType theType) {
        switch (theType) {
            case DataType::int_type: return Value((int) 0);

            case DataType::float_type: return Value((float) 0);

            case DataType::varchar_type: return Value("");

            case DataType::timestamp_type: return Value((unsigned int) std::time(nullptr));

            case DataType::bool_type: return Value(false);

            default:return Value(true);
        }
    }
}

