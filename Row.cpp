//
//  Row.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Row.hpp"
#include "Storage.hpp"
#include <sstream>
#include <cmath>

namespace ECE141 {

    Row::Row(int32_t aBlockNumber) : blockNumber(aBlockNumber), columns() {
    }

    Row::Row(const Row &aCopy) : blockNumber(aCopy.blockNumber), columns(aCopy.columns) {
    }

    Row::Row(const Block &aBlock) {
        Block temp = aBlock;
        BufferReader aReader(temp.data, kPayloadSize, kPayloadSize);
        uint8_t size;
        aReader >> size; //record number of fields being written...
        std::string tempString;
        Value tempValue;
        for (size_t i = 0; i < size; ++i) {
            aReader >> tempString >> tempValue;
            columns.emplace(std::make_pair(tempString, tempValue));
        }
    }

    Row::Row(const KeyValues &aKVlist) {
        for (auto &pair : aKVlist) {
            columns.emplace(pair);
        }
//        std::cout << "rowctor aLVlist size " << columns.size();
//        columns = aKVlist;
    }

    Row::Row(const KeyValues &aKVlist, uint32_t ablockNum) : blockNumber(ablockNum) {
        for (auto &pair : aKVlist) {
            columns.emplace(pair);
        }
    }

    Row::~Row() {
        //STUDENT: implement this too
    }

    // USE: add a new field+value into the row...
    Row &Row::addColumn(const std::string &aString, const Value &aValue) {
        columns[aString] = aValue;
        return *this;
    }

    bool Row::operator==(const Row &aCopy) {
//        return columns==aCopy.getColumns();
        auto aColumn = aCopy.getColumns().begin();

        for (const auto &kv : columns) {

            if (aColumn == columns.end())
                return false;
            if (aColumn->first != kv.first)
                return false;
            if (aColumn->second.getType() != kv.second.getType())
                return false;
            switch (kv.second.getType()) {
                case DataType::int_type :
                    if (aColumn->second.getIntVal() != kv.second.getIntVal())
                        return false;
                    break;
                case DataType::float_type :
                    if ((fabsf(aColumn->second.getFloatVal() - kv.second.getFloatVal()) > 1e-6))
                        return false;
                    break;
                case DataType::timestamp_type :
                    if (aColumn->second.getTimeVal() != kv.second.getTimeVal())
                        return false;
                    break;
                case DataType::bool_type :
                    if (aColumn->second.getBoolVal() != kv.second.getBoolVal())
                        return false;
                    break;
                case DataType::varchar_type :
                    if (aColumn->second.getVarcharVal() != kv.second.getVarcharVal())
                        return false;
                    break;
                case DataType::no_type : break;
                default: break;
            }
//            if (!(aColumn->second == kv.second) {
//                return false;
//            }
            aColumn++;

        }
        if (aColumn != aCopy.getColumns().end())
            return false;
        return true;
    }

    //---------------------------------------------------

    RowCollection::RowCollection() : rows() {}

    RowCollection::RowCollection(const RowCollection &aCopy) : rows(aCopy.rows) {}

    RowCollection::~RowCollection() {
        for (auto theRow : rows) {
            delete theRow;
        }
    }

    RowCollection &RowCollection::add(Row *aRow) {
        rows.push_back(aRow);
        return *this;
    }

    RowCollection &RowCollection::reorder(const PropertyList &anOrder) {

        Property theP = anOrder.at(0);

        auto compareValue = [&theP](Row *lhs, Row *rhs) -> bool {
            return (lhs->columns[theP.name] < rhs->columns[theP.name]);
        };

        std::sort(this->rows.begin(), this->rows.end(), compareValue);

        return *this;
    }
}
