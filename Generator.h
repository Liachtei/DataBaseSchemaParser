/*
 * Generator.h
 *
 *  Created on: 22.10.2015
 *      Author: liachtei
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <string>
#include "Schema.hpp"
#include <sstream>
#include <memory>


class Generator {
public:
	Generator(Schema& schema, std::string outputFileName);
	virtual ~Generator();

private:
	std::string outputFileName;
	Schema schema;
	void generateFile();
	std::string generateAttributes(Schema::Relation& relation);

};

#endif /* GENERATOR_H_ */
