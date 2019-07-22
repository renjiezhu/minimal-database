#ifndef Index_hpp
#define Index_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "Value.hpp"
#include "keywords.hpp"
#include "Storage.hpp"

namespace ECE141 {
    //  Index!
    class Index {
    public:
        Index(const std::string &aField, uint32_t aTableId, DataType aKeyType) :
                field(aField), tableId(aTableId), keyType(aKeyType) {}

        Index() : field(""), tableId(0), keyType(DataType::int_type) {}

        Index &addKeyValue(const Value &aKey, uint32_t aValue);

        Index &removeKeyValue(const Value &aKey);

        bool contains(const Value &aKey);

        uint32_t getValue(const Value &aKey);

        StatusResult encode(Block &aBlock, uint32_t nextBlock);

        StatusResult decode(const Block &aBlock);

        using IndexType = std::map<Value, uint32_t>;    //  C++ Typedef!

        // allow iterators
        typedef IndexType::iterator iterator;
        typedef IndexType::const_iterator const_iterator;

        iterator begin() { return index.begin(); }

        iterator end() { return index.end(); }

        uint32_t getTableID() { return tableId; }

        size_t size() { return index.size(); }

    protected:
        IndexType index;
        std::string field;
        DataType keyType;
        uint32_t tableId;
    };
}
#endif /*Index_hpp*/
