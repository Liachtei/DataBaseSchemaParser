#include <iostream>
#include <memory>
#include "Parser.hpp"
#include "Generator.h"
#include "Schema.hpp"

int main(int argc, char* argv[]) {
   if (argc != 2) {
      std::cerr << "usage: " << argv[0] << " <schema file>" << std::endl;
      return -1;
   }

   Parser p(argv[1]);
   try {
      std::unique_ptr<Schema> schema = p.parse();
      std::cout << schema->toString() << std::endl;
      Generator g = Generator(*schema, "GeneratedSchema.cpp");
   } catch (ParserError& e) {
      std::cerr << e.what() << std::endl;
   }

   return 0;
}
