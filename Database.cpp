//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Database.hpp"
#include "View.hpp"
#include "BlockVisitor.hpp"
#include "Entity.hpp"
#include "Row.hpp"

namespace ECE141 {

    bool describeBlock(const Block &aBlock, uint32_t aBlockNum) {

        return true;
    }

    //==================================================================

    // USE: This view is used to do a debug dump of db/storage container...
    class DescribeDatabaseView : public View {
    public:
        DescribeDatabaseView(Storage &aStorage) : storage{aStorage}, stream(nullptr) {}

        StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
            //STUDENT: Write code here to display information about the given block to your output stream.
            //         Make this as informative as you can.
            (*stream) << aBlockNum << ". block " << aBlock.header.type << std::endl;

            return StatusResult{noError};
        }

        bool show(std::ostream &anOutput) {
            stream = &anOutput;
            storage.eachBlock(*this);
            return true;
        }

    protected:
        Storage &storage;
        std::ostream *stream;
    };

    //==================================================================

    // USE: This view is used to show list of entities in TOC....
    class ShowTablesView : public View {
    public:
        ShowTablesView(Block &aTOC) : toc{aTOC} {}

        bool show(std::ostream &anOutput) {
            anOutput << "Showing tables: \n";
            for (int i = 0; i < toc.entities.header.count; i++) {
                anOutput << toc.entities.items[i].name << std::endl;
            }
            return true;
        }

    protected:
        Block &toc;
    };

    //==================================================================

    Database::Database(const std::string aName, CreateNewStorage)
            : name(aName), storage(aName, CreateNewStorage{}) {
        //we've created storage, but haven't loaded any entities yet...
    }

    Database::Database(const std::string aName, OpenExistingStorage)
            : name(aName), storage(aName, OpenExistingStorage{}) {
        //we've opened storage, but haven't loaded any entities yet...
    }

    Database::~Database() {

//        StatusResult theResult = saveIndices();
//        uint32_t indexBlockNum = theResult.value;

//        saveEntities(indexBlockNum);
        saveEntities();

        for (auto &item : entities) {
            delete item.second;
        }
//deleting the entities pointer
    }

    // USE: a child object needs a named entity for processing...
    Entity *Database::getEntity(const std::string &aName) {

        //STUDENT: implement this method to retrieve entity from storage...
        if (entities.count(aName)) {
            return entities[aName];
        } else {
            //check unloaded schema from storage...
            PersistEntity *thePE = storage.findEntityInTOC(aName);
            if (thePE && thePE->blocknum > 0) {
                Block theBlock;
                StatusResult theResult = storage.readBlock(thePE->blocknum, theBlock, sizeof(theBlock));
                if (theResult) {
                    Entity *theEntity = new Entity(theBlock, thePE->blocknum);
                    entities[aName] = theEntity;
                    return theEntity;
                }
            }
        }
        return nullptr;
    }

    // USE:   Add a new table (entity);
    // NOTE:  The DB assumes ownership of the entity object...
    StatusResult Database::createTable(Entity *anEntity) {
        std::string &theName = anEntity->getName();
        entities[theName] = anEntity;

        Block theBlock;
        theBlock = (*anEntity); //convert from entity...
        StatusResult theResult = storage.addEntity(theName, theBlock);
        Entity *theEntity = entities[theName];
        theEntity->blockNum = theResult.value; //hang on to the blocknum...

//        Index *theIndex = new Index(theEntity->getPrimaryKey(), theEntity->blockNum, theEntity->getPrimaryKeyType());

//        indices[theName] = theIndex;

        return theResult;
    }

    // USE: called to actually delete rows from storage for table (via entity) with matching filters...
    StatusResult Database::deleteRows(const Entity &anEntity, const Filters &aFilters) {
        RowCollection theCollection;
        if (StatusResult theResult = selectRows(theCollection, anEntity, aFilters)) {
            RowList &theRows = theCollection.getRows();
            for (auto &theRow : theRows) {
                storage.releaseBlock(theRow->getBlockNumber());
            }
        }
        return StatusResult{noError};
    }

    // USE: Call this to dump the db for debug purposes...
    StatusResult Database::describe(std::ostream &anOutput) {
        if (View *theView = new DescribeDatabaseView(storage)) {
            theView->show(anOutput);  //STUDENT you need to implement the view...
            delete theView;
        }
        return StatusResult{noError};
    }

    // USE: call this to actually remove a table from the DB...
    StatusResult Database::dropTable(const std::string &aName) {

        //STUDENT: Implement this method:
        //         1. find the Entity in storage TOC
        //         2  if found, ask storage to drop it
        //         3. if you cache entities on DB, remove it there too...
//    ...
        if (Entity *theEntity = getEntity(aName)) {
            storage.dropEntity(aName);
//                std::cout<<"Dropping table from storage"<<std::endl;
            // drop from cache on DB
            if (entities.count(aName)) {
                entities.erase(aName);
//                    return entities[aName];
//                    std::cout<<"Clearing table from entity cache"<<std::endl;
            }
            return StatusResult{noError};
        }
//    ...
        return StatusResult{unknownTable};
    }


    // USE: call this to add a row into the given database...
    StatusResult Database::insertRow(const Row &aRow, const std::string &aTableName) {
        if (Entity *theEntity = getEntity(aTableName)) {

            // auto increment the primary key
            Row theRow = aRow;

            if (uint32_t autoincr = theEntity->getNextAutoIncrementValue()) {
                Value tempAuto(autoincr, DataType::int_type);
                theRow.columns[theEntity->getPrimaryKey()] = tempAuto;
            }

            // validation of data
            if (theEntity->validate(theRow.columns)) {
                Block theBlock(kDataBlockType);
                StatusResult theResult = theEntity->encodeData(theRow.columns, theBlock);
                if (theResult) {
                    theResult = storage.addBlock(theBlock);
//                    if (Index *theIndex = getIndex(aTableName, theEntity->getPrimaryKey())) { //see step 2d
//                        theIndex->addKeyValue(theRow.columns[theEntity->getPrimaryKey()], theBlock.header.id);
//                    }
                }

                return StatusResult{noError};
            }
            return StatusResult{invalidAttribute};
        }
        return StatusResult{unknownTable};
    }

    // USE: select a set of rows that match given filters (or all rows if filters are empty)...
    StatusResult Database::selectRows(RowCollection &aCollection, const Entity &anEntity, const Filters &aFilters,
                                      const StringList *aFields) {
        //STUDENT: Get blocks for this entity from storage, decode into rows, and add to collection
        //         NOTE:  aFields (if not empty) tells you which fields to load per row;
        //                otherwise load all fields (*)
        if (Index *theIndex = getIndex(anEntity.name, anEntity.getPrimaryKey())) {
            StatusResult theResult;
            for (auto &thePair : *theIndex) {
                uint32_t theBlockNum = thePair.second;
                Block theBlock;
                if ((theResult = storage.readBlock(theBlockNum, theBlock))) {
                    KeyValues theValues;
                    theResult = anEntity.decodeData(theBlock, theValues, aFields);
                    if (aFilters.matches(theValues)) {
                        aCollection.add(new Row(theValues, theBlockNum));
                    }
                }
            }
            return theResult;
        } else {
            BlockVisitor *blkVisitor = new BlockVisitor(aCollection, anEntity, aFilters, aFields);
            storage.eachBlock(*blkVisitor);
            delete blkVisitor;
            return StatusResult{noError};
        }
    }

    //USE: resave entities that were in memory and changed...
    StatusResult Database::saveEntities(uint32_t indexBlockNum) {
        StatusResult theResult{noError};
        for (auto &thePair : entities) {
            if (thePair.second->isDirty()) {
                thePair.second->indexBlockNum = indexBlockNum;
                Block theBlock = (*thePair.second);
                theResult = storage.writeBlock(thePair.second->blockNum, theBlock);
            }
        }
        return theResult;
    }

    StatusResult Database::saveEntities() {
        StatusResult theResult{noError};
        for (auto &thePair : entities) {
            if (thePair.second->isDirty()) {
                Block theBlock = (*thePair.second);
                theResult = storage.writeBlock(thePair.second->blockNum, theBlock);
            }
        }
        return theResult;
    }

    StatusResult Database::saveIndices() {

        StatusResult theResult = storage.findFreeBlockNum();

        for (auto &thePair : indices) {
            size_t indicesSize = thePair.second->size();
            uint32_t numBlocks = ceil(((indicesSize * 8) / kPayloadSize));
            for (int i = 0; i < numBlocks; ++i) {
                Block theBlock(kIndexBlockType);
                uint32_t nextBlockNum = -1;
                if (i == numBlocks - 1)
                    nextBlockNum = storage.findFreeBlockNum().value;
                if (thePair.second->encode(theBlock, nextBlockNum))
                    storage.addBlock(theBlock);
            }


        }
        return theResult;
    }

    //USE: show the list of tables in this db...
    StatusResult Database::showTables(std::ostream &anOutput) {

        //STUDENT: create a ShowTablesView object, and call the show() method...
        //         That view is declared at the top of this file.

        if (View *theView = new ShowTablesView(storage.toc)) {
            theView->show(anOutput);

            delete theView;

            return StatusResult{noError};
        }

        return StatusResult{readError};


    }

    // USE: called to update records already in db (future assignment)...
    StatusResult Database::updateRow(const Row &aRow, const KeyValues &aKVList, const Entity &anEntity) {
        //STUDENT: Implement this...
        Row theRow = aRow;
        Entity theEntity = anEntity;
        KeyValues &newKV = theRow.setColumns();
        for (auto &aKV: aKVList) {
            Value tempValue = aKV.second;
            if (tempValue.become(newKV[aKV.first].getType())) {
                newKV[aKV.first] = tempValue;
            }
        }
        Block theBlock;
        storage.readBlock(theRow.getBlockNumber(), theBlock, sizeof(Block));
        theEntity.encodeData(newKV, theBlock);
        storage.writeBlock(theRow.getBlockNumber(), theBlock);

        return StatusResult{noError};
    }

    // USE: asks the DB if a table exists in storage...
    bool Database::tableExists(const std::string aName) {
        //STUDENT: implement this if you need it...
        return false;
    }

    Index *Database::getIndex(const std::string &aTableName, const std::string &aFieldName) {

        if (indices.count(aFieldName)) {
            return indices[aFieldName];
        } else {
            // read Index block
            uint32_t blocknum = entities.begin()->second->getIndexBlockNum();
            while (blocknum > 0) {
                Index *theIndex = new Index();
                Block theBlock;
                storage.readBlock(blocknum, theBlock, sizeof(theBlock));
                StatusResult theResult = theIndex->decode(theBlock);
                blocknum = theResult.value;
                indices[aFieldName] = theIndex;
            }

            return indices[aFieldName];
        }

        return nullptr;
    }

    StatusResult Database::alterTable(Entity &anEntity, const Attribute &anAttribute, std::string action) {


        // changing the entity
        // altering the entity in memory
        anEntity.addAttribute(anAttribute);
        // marking it dirty would force the database to save
        // the new entity
        anEntity.dirty = true; //

        // modifying rowcollection?

        RowCollection aCollection;

        Filters aFilter;

        selectRows(aCollection, anEntity, aFilter);

        for (auto &row : aCollection.getRows()) {

            Value tempValue = Value::getDefault(anAttribute.getType());
            row->addColumn(anAttribute.getName(), tempValue);

            Block theBlock;
            storage.readBlock(row->getBlockNumber(), theBlock, sizeof(Block));
            anEntity.encodeData(row->setColumns(), theBlock);
            storage.writeBlock(row->getBlockNumber(), theBlock);
        }

        return StatusResult{noError};
    }

}

