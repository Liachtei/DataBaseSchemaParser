#include "Schema.hpp"

#include <sstream>

static std::string type(const Schema::Relation::Attribute& attr) {
   Types::Tag type = attr.type;
   switch(type) {
      case Types::Tag::Integer:
         return "Integer";
      case Types::Tag::Timestamp:
    	  return "Timestamp";
      case Types::Tag::Numeric: {
         std::stringstream ss;
         //TODO: Check about length
         ss << "Numeric(" << attr.len << ", " << attr.len2 << ")";
         return ss.str();
      }
      case Types::Tag::Char: {
         std::stringstream ss;
         ss << "Char(" << attr.len << ")";
         return ss.str();
      }
      case Types::Tag::Varchar: {
    	  std::stringstream ss;
    	  ss << "Varchar(" <<attr.len << ")";
    	  return ss.str();
      }
   }
   throw;
}


std::string Schema::toString() const {
   std::stringstream out;
   out << "RELATIONS:" << std::endl;
   for (const Schema::Relation& rel : relations) {
      out << rel.name << std::endl;
      out << "\tPrimary Key:";
      for (unsigned keyId : rel.primaryKey)
         out << ' ' << rel.attributes[keyId].name;
      out << std::endl;
      for (const auto& attr : rel.attributes)
         out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << std::endl;
   }
   out << std::endl;
   out << "INDEXES:" << std::endl;
   for(const Schema::Index& ind : indexes)
   {
	   out << ind.name <<std::endl;
	   out << "\ton table: " << ind.ofTable << std::endl;
	   out << "\ton attributes: ";
	   for(const auto& attr : ind.attributeNames)
	   {
		   out << attr << " ";
	   }
	   out << std::endl;
   }
   return out.str();
}
