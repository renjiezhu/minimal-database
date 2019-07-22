//
//  RecordsView.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/26/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef RecordsView_h
#define RecordsView_h

#include <iostream>
#include <iomanip>
#include "View.hpp"
#include "Entity.hpp"
#include "Storage.hpp"

namespace ECE141 {

    // USE: This view is used to show a record set in tabular format...
    class RecordsView : public View {
    public:

        RecordsView(const Entity &anEntity, RowCollection &aCollection)
                : collection{aCollection}, entity(anEntity) {}


        // USE: this is the main show() for the view, where it presents all the rows...
        bool show(std::ostream &anOutput) {
            //STUDENT: Implement this, to show each row (from aCollection) like this example:
            // RowCollection = Class for storing RowList, where RowList = std::vector<Row *>;
            // We have the rowCollection, for each row, just print the values
            // > select * from Names
            // +-----------+----------------------+
            // | id        | first_name           |
            // +-----------+----------------------+
            // | 1         | rickg                |
            // +-----------+----------------------+
            // | 2         | ramsin               |
            // +-----------+-------------------+
            // print columns names
            // display each row
            // Obtaining the rows of the object! How do I obtain the rows of the class?
            RowList rows = collection.getRows();
            // First print the keys in the first row. Then, print the values for all the rows!
            if (rows.size()) {
                Row *firstRow = rows[0];
                // Print the rowSeparators and attributes!
                displaySeparators(firstRow->getColumns(), anOutput);
                displayAttrNames(firstRow->getColumns(), anOutput);
                // Print the contents of all the rows!
                for (auto &row : rows) {
                    displaySeparators(row->getColumns(), anOutput);
                    displayValues(row->getColumns(), anOutput);
                }
                displaySeparators(firstRow->getColumns(), anOutput);
            } else {
                std::cout << "No records found. \n";
            }
            return true; //or false ,if your view fails...
        }

        static void displaySeparators(const KeyValues &columns, std::ostream &anOutput) {
            const char *theSep = "+-------------------";
            for (size_t i = 0; i < columns.size(); ++i) {
                anOutput << theSep;
            }
            anOutput << "+\n";
        }

        static void displayAttrNames(const KeyValues &columns, std::ostream &anOutput) {
            for (auto &elem : columns) {
                anOutput << "| ";
                size_t lenSpace = 18 - elem.first.size();
                anOutput << elem.first;
                anOutput << std::string(lenSpace, ' ');
            }
            anOutput << "|\n";
        }

        static void displayValues(const KeyValues &columns, std::ostream &anOutput) {
            //  Value is used to switch between printing attributes and values of the rows!
            for (auto &elem : columns) {
                anOutput << "| ";
                std::string val;
                switch (elem.second.getType()) {
                    case (DataType::int_type)   : val = std::to_string(elem.second.getIntVal());
                        break;
                    case (DataType::float_type) : val = std::to_string(elem.second.getFloatVal());
                        break;
                    case (DataType::timestamp_type)   : val = std::to_string(elem.second.getTimeVal());
                        break;
                    case (DataType::bool_type)   : val = std::to_string(elem.second.getBoolVal());
                        break;
                    default: val = elem.second.getVarcharVal();
                        break;
                }
                size_t lenSpace = 18 - val.size();
                anOutput << val;
                anOutput << std::string(lenSpace, ' ');
            }
            anOutput << "|\n";
        }

    protected:
        RowCollection &collection;
        const Entity &entity;
    };

}

#endif /* RecordsView_h */
