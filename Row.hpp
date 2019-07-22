//
//  Row.hpp
//  Database3
//
//  Created by rick gessner on 3/22/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Row_h
#define Row_h

#include <stdio.h>
#include <vector>
#include "Value.hpp"
#include "Attribute.hpp"
#include "Entity.hpp"

namespace ECE141 {

    class Block;

    class Row {
    public:
        Row(int32_t aBlockNumber = 0);

        Row(const Row &aRow);

        Row(const Block &aBlock);

        Row(const KeyValues &aKVlist);

        Row(const KeyValues &aKVlist, uint32_t ablockNum);

        ~Row();

        int32_t getBlockNumber() { return blockNumber; }

        const KeyValues &getColumns() const { return columns; }

        KeyValues &setColumns() { return columns; }

        bool operator==(const Row &aCopy);

        Row &addColumn(const std::string &aString, const Value &aValue);

        friend class RowCollection;

        friend class Database;

        friend class RecordsView;

        friend class Filters;

    protected:
        uint32_t blockNumber;
        KeyValues columns;
    };

    using RowList = std::vector<Row *>;

    class RowCollection {
    public:
        RowCollection();

        RowCollection(const RowCollection &aCopy);

        ~RowCollection();

        RowList &getRows() { return rows; }

        RowCollection &add(Row *aRow);

        RowCollection &reorder(const PropertyList &anOrder);

    protected:
        RowList rows;
    };

}
#endif /* Row_h */
