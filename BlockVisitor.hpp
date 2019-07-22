//
//  BlockVisitor_h
//  Datatabase4
//
//  Created by rick gessner on 4/25/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef BlockVisitor_h
#define BlockVisitor_h

#include "Storage.hpp"
#include "Entity.hpp"
#include "Filters.hpp"
#include "Row.hpp"

namespace ECE141 {

    class BlockVisitor {
    public:
        BlockVisitor(RowCollection &aCollection, const Entity &anEntity, const Filters &aFilters,
                     const StringList *aFields)
                : collection(aCollection), entity(anEntity), filters(aFilters), fields(aFields) {}

        StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
            if ('D' == aBlock.header.type) { // if data block
                if (entity.getHash() == aBlock.header.extra) { // if belongs to entity
//            Row * blkRow = new Row(aBlock);
//            KeyValues columns = blkRow->setColumns();
                    KeyValues columns;
                    entity.decodeData(aBlock, columns, fields);

                    if (filters.matches(columns)) { // if filters match the block
                        collection.add(new Row(columns, aBlockNum));
                    }
                }
            }
            return StatusResult{noError};
        }

        const Entity &entity;
        const Filters &filters;
        RowCollection &collection;
        const StringList *fields;
    };

}

#endif /* BlockVisitor_h */
