//

#include "Index.hpp"
#include "Entity.hpp"
#include "Value.hpp"
#include "MemoryStream.hpp"


namespace ECE141 {

    Index &Index::addKeyValue(const Value &aKey, uint32_t aValue) {
        //  Insert the keyValue to the index;
        index.insert(std::pair<Value, uint32_t>(aKey, aValue));
        return *this;
    }

    Index &Index::removeKeyValue(const Value &aKey) {
        //  Removes the keyValue from the Index;
        if (contains(aKey)) {
            index.erase(aKey);
        }
        return *this;
    }

    bool Index::contains(const Value &aKey) {
        //  Check for whether the map has aKey!
        for (auto &it : index) {
            if (it.first == aKey) {
                return true;
            }
        }
        return false;
    }

    uint32_t Index::getValue(const Value &aKey) {
        //  Obtain an object of Value class!
        if (contains(aKey)) {
            return index[aKey];
        }
        return 0;
    }

    StatusResult Index::encode(ECE141::Block &aBlock, uint32_t nextBlock) {

        StatusResult theResult{noError};

        BufferWriter aWriter(aBlock.data, kPayloadSize);

        aWriter << field << static_cast<char>(keyType) << tableId << (uint32_t) index.size();

        for (auto &it : index) {
            aWriter << it.first << it.second;
        }

        aWriter << nextBlock;

        return theResult;
    }

    StatusResult Index::decode(const ECE141::Block &aBlock) {


        Block temp(aBlock);
        BufferReader aReader(temp.data, kPayloadSize, kPayloadSize);

        char tempType;
        uint32_t tempSize;

        aReader >> field >> tempType >> tableId >> tempSize;

        for (size_t i = 0; i < tempSize; ++i) {
            Value tempValue;
            uint32_t tempId;
            aReader >> tempValue >> tempId;
            addKeyValue(tempValue, tempId);
        }

        uint32_t nextBlock;
        aReader >> nextBlock;
        StatusResult theResult{noError, nextBlock};
        return theResult;
    }

}
