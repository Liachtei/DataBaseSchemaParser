#ifndef H_Schema_hpp
#define H_Schema_hpp

#include <vector>
#include <string>
#include "Types.hpp"

struct Schema {
   struct Relation {
      struct Attribute {
         std::string name;
         Types::Tag type;
         unsigned len;
         signed len2;
         bool notNull;
         Attribute() : len(~0), notNull(true) {len2=-1;}
      };
      std::string name;
      std::vector<Schema::Relation::Attribute> attributes;
      std::vector<unsigned> primaryKey;
      Relation(const std::string& name) : name(name) {}
   };
   struct Index {
	   std::string name;
	   std::string ofTable;
	   std::vector<std::string> attributeNames;
	   std::vector<unsigned> index;
	   Index(const std::string& name) : name(name) {}
   };
   std::vector<Schema::Relation> relations;
   std::vector<Schema::Index> indexes;
   std::string toString() const;
};
#endif
