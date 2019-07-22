//
//  EntityDescriptionView.cpp
//  Datatabase4
//
//  Created by rick gessner on 4/22/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "EntityDescriptionView.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include <iomanip>

//** USE: This view describes an entity (fields), suitable for console display...

namespace ECE141 {

    EntityDescriptionView::EntityDescriptionView(const Entity &anEntity)
            : entity{anEntity}, stream(nullptr) {}


    bool EntityDescriptionView::show(std::ostream &anOutput) {
        stream = &anOutput; //for use in callback...

        const char *theSep = "+-------------------+--------------+------+------+----------------+------------------+";
        const char *theTitle = "| Field             | Type         | Null | Key  | Default        | Extra            |";

        anOutput << theSep << std::endl;
        anOutput << theTitle << std::endl;
        anOutput << theSep;

        //STUDENT: Add your code here to output the list of fields, properly formatted...
        //         Feel free to completely change this class if you have a better approach...
        AttributeList attributes = this->entity.getAttributes();

//      for (auto &attr : attributes) {
//          attr.describeTabular();
//      }
        for (auto &attr : attributes) {
            // field
            anOutput << "\n| " << attr.getName();
            size_t nSpace = 18 - attr.getName().length();
            anOutput << std::string(nSpace, ' ');
            // type
            switch (attr.getType()) {
                case DataType::int_type :anOutput << "| integer      ";
                    break;

                case DataType::bool_type :anOutput << "| boolean      ";
                    break;

                case DataType::varchar_type:anOutput << "| varchar      ";
                    break;

                case DataType::float_type:anOutput << "| float        ";
                    break;

                case DataType::timestamp_type:anOutput << "| timestamp    ";
                    break;

                default:anOutput << "| n/a          ";
                    break;

            }

            // Null
            if (attr.isNullable()) anOutput << "| T    ";
            else anOutput << "| F    ";

            // Key
            if (attr.isPrimaryKey()) anOutput << "| PRI  ";
            else anOutput << "|      ";

            // default
//          attr.
            anOutput << "|                ";

            if (attr.isAutoIncrement()) anOutput << "| auto             |";
            else anOutput << "|                  |";

        }
        anOutput << "\n";
        anOutput << theSep << std::endl;

        return true;

    }

}
