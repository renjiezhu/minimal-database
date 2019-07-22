//
//  Schema.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Entity.hpp"
#include "Storage.hpp"
#include "Row.hpp"
#include "Tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <streambuf>
#include <sys/stat.h>
#include "Helpers.hpp"
#include "MemoryStream.hpp"
#include "View.hpp"
#include "Filters.hpp"

const int gMultiplier = 37;

namespace ECE141 {

    //utility...

    uint32_t Entity::hashString(const char *str) {
        uint32_t h{0};
        unsigned char *p;
        for (p = (unsigned char *) str; *p != '\0'; p++)
            h = gMultiplier * h + *p;
        return h;
    }

    //----------------------------------------------------

    Entity::Entity(std::string aName)
            : name(aName), attributes(), dirty(false), count(0), blockNum(0), autoincr(0) {
        hash = hashString(aName.c_str());
    }

    Entity::Entity(const Entity &aCopy) :
            name(aCopy.name), attributes(aCopy.attributes), dirty(aCopy.dirty), autoincr(aCopy.autoincr),
            count(aCopy.count), blockNum(aCopy.blockNum), hash(aCopy.hash) {
    }

    Entity::Entity(Block &aBlock, uint32_t aBlockNum) {
        //STUDENT: construct an entity (decode) from the given block...

        BufferReader aReader(aBlock.data, kPayloadSize, kPayloadSize);

        aReader >> name >> count >> autoincr >> blockNum >> hash;

        uint8_t size;
        aReader >> size;


        for (int i = 0; i < size; i++) {
            Attribute anAtt;
            aReader >> anAtt;
            addAttribute(anAtt);
        }

        aReader >> indexBlockNum;

    }

    Entity::Entity() {}

    // USE: ask the entity for name of primary key (may not have one...)
    std::string Entity::getPrimaryKey() const {
        for (auto &attr : attributes) {
            if (attr.isPrimaryKey()) return attr.getName();
        }
        return std::string("id"); //XXX-HACK wrong
    }

    DataType Entity::getPrimaryKeyType() const {
        for (auto &attr : attributes) {
            if (attr.isPrimaryKey()) return attr.getType();
        }
        return DataType::int_type;
    }

    // USE: ask entity for next auto-incr value  (for a new record)....
    uint32_t Entity::getNextAutoIncrementValue() {
        dirty = true; //so we resave when DB is released...
        return ++autoincr;
    }

    void Entity::each(attribute_callback aCallback) const {
        for (auto theAttr : attributes) {
            aCallback(theAttr);
        }
    }


    Entity &Entity::markDirty(bool aState) {
        dirty = aState;
        return *this;
    }

    Entity &Entity::addAttribute(std::string aName, DataType aType, uint8_t aSize, bool autoIncr) {
        attributes.push_back(Attribute(aName, aType, aSize));
        return *this;
    }

    Entity &Entity::addAttribute(const Attribute &anAttribute) {
        attributes.push_back(anAttribute);
        return *this;
    }

    Attribute &Entity::getAttribute(const std::string &aName) {
        for (auto theIter = attributes.begin(); theIter != attributes.end(); theIter++) {
            if (theIter->getName() == aName) {
                return *theIter;
            }
        }
        throw std::runtime_error("Attribute not found");
    }

    Value Entity::getDefaultValue(const Attribute &anAttribute) {
        Value theResult;
        return theResult;
    }

    //Encode (pack) this entity into a storage block...
    Entity::operator Block() {
        Block theBlock{kEntityBlockType};

        BufferWriter aWriter(theBlock.data, kPayloadSize);

        aWriter << name << count << autoincr << blockNum << hash;
        aWriter << (uint8_t) attributes.size();

        for (auto &attribute : attributes) {
            aWriter << attribute;
        }

        aWriter << getIndexBlockNum();
        return theBlock;
    }

    bool Entity::hasCompatibleAttribute(const std::string &name, const Value &aValue) {
        bool comp = false;
        for (auto &attr : attributes) {
            if (attr.getName() == name) {
                comp = (attr.getType() == aValue.getType());
//                comp = true;
            }
        }
        return comp;
    }

    //** USE:
    StatusResult Entity::validate(KeyValues &aList) {

        for (auto theKV : aList) {

            if (!hasCompatibleAttribute(theKV.first, theKV.second)) {
                return StatusResult{invalidAttribute};
            }

        }
        return StatusResult{noError};
    }

    // USE: write given row into given block (using entity as guide...)
    StatusResult Entity::encodeData(KeyValues &aDataList, Block &aBlock) {
        StatusResult theResult{noError};

        BufferWriter aWriter(aBlock.data, kPayloadSize);

        aBlock.header.extra = hashString(name.c_str());

        for (auto &attr : attributes) {
            if (aDataList.count(attr.getName())) {
                aWriter << aDataList[attr.getName()];
            } else {
                aWriter << Value::getDefault(attr.getType());
            }
        }

        return theResult;
    }

    bool inGivenFields(const std::string &aFieldName, const StringList &aFields) {
        auto theEnd = aFields.end();
        return std::find(aFields.begin(), theEnd, aFieldName) != theEnd;
    }

    // USE: read data from block into given keyvalue list (for row?)...
    StatusResult Entity::decodeData(const Block &aBlock, KeyValues &aValuesList,
                                    const StringList *aFields) const {
        StatusResult theResult{noError};

        Block temp(aBlock);
        BufferReader aReader(temp.data, kPayloadSize, kPayloadSize);


        for (auto &attr : attributes) {
            Value tempValue;

            aReader >> tempValue;

            if (!aFields or aFields->empty()) {
                aValuesList.emplace(std::make_pair(attr.getName(), tempValue));
            } else if (inGivenFields(attr.getName(), *aFields)) {
                aValuesList.emplace(std::make_pair(attr.getName(), tempValue));
            }
        }

        return theResult;
    }


}
