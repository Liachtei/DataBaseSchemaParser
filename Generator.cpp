/*

 * Generator.cpp
 *
 *  Created on: 22.10.2015
 *      Author: liachtei
 */

#include "Generator.h"
#include <fstream>
#include <iostream>

using namespace std;

Generator::Generator(Schema& schema, std::string outputFileName) {
	this->schema = schema;
	this->outputFileName = outputFileName;
	this->generateFile();
}

Generator::~Generator() {
	/* Nothing to do here */
}

string Generator::generateAttributes(Schema::Relation& relation) {
	stringstream out;
	for (auto& attr : relation.attributes) {
		switch (attr.type) {
		case Types::Tag::Char:
			out << "\tChar<" << attr.len << "> " << attr.name << ";" << endl;
			break;
		case Types::Tag::Varchar:
			out << "\tVarchar<" << attr.len << "> " << attr.name << ";" << endl;
			break;
		case Types::Tag::Integer:
			out << "\tInteger " << attr.name << ";" << endl;
			break;
		case Types::Tag::Numeric:
			out << "\tNumeric<" << attr.len << ", " << attr.len2 << "> "
					<< attr.name << ";" << endl;
			break;
		case Types::Tag::Timestamp:
			out << "\tTimestamp " << attr.name << ";" << endl;
			break;
		default:
			cout << "WARNING DEFAULT CASE STATEMENT!!" << endl;
		}
	}
	return out.str();
}

#include <vector>
#include <tuple>
void Generator::generateFile() {
	ofstream outputFile;
	outputFile.open(outputFileName.c_str());
	ostringstream out;
	out << "#include \"Types.hpp\"" << endl;
	out << "#include <vector>" << endl;
	out << "#include <tuple>" << endl;
	out << "#include <algorithm>" << endl;
	out << "#include <fstream>" << endl;
	out << "using namespace std;" << endl;

	vector<string> keys;

	//Define relations
	for (auto& relation : schema.relations) {
		out << "struct " << relation.name << " {" << endl;

		//Define attributes of relation
		out << "\t// Attributes" << endl;
		out << generateAttributes(relation);
		out << "\t// Primary key" << endl;

		//Define primary key
		string primaryKeyString = "tuple<Integer";
		for (uint i = 1; i < relation.primaryKey.size(); i++) {
			primaryKeyString.append(",Integer");
		}
		primaryKeyString.append(">");
		out << "\t" << primaryKeyString << " primaryKey;" << endl;
		keys.push_back(primaryKeyString);

		out << "};" << endl;
	}
	//End of Relations

	//Generate Vectors of Relations
	for (uint index = 0; index < schema.relations.size(); index++) {
		Schema::Relation relation = schema.relations[index];
		string pks = keys[index];
		//TABLES
		out << "vector<" << relation.name << "> " << relation.name << "Table;"
				<< endl;
		out << "vector<tuple<" << keys[index] << ", int>> " << relation.name
				<< "IndexTable;" << endl;
		out << endl;
	}
	out << endl;

	out << "// Find functions" << endl;

	//Generate "find" functions
	for (uint index = 0; index < schema.relations.size(); index++) {
		Schema::Relation relation = schema.relations[index];
		if (relation.name == "history") {
			continue;
		}
		out << "int find" << relation.name << "(" << keys[index] << " key) {"
				<< endl;
		out << "\tfor(auto entry : " << relation.name << "IndexTable){" << endl;
		out << "\tif(get<0>(entry) == key){" << endl;
		out << "\t\treturn get<1>(entry);" << endl;
		out << "\t\t}" << endl;
		out << "\t}" << endl;
		out << "\treturn -1;" << endl;
		out << "}" << endl;
		out << endl;
	}

	out << "// Delete functions" << endl;
	//Generate "delete" functions
	for (uint index = 0; index < schema.relations.size(); index++) {
		Schema::Relation relation = schema.relations[index];
		if (relation.name == "history") {
			continue;
		}
		out << "bool delete" << relation.name << "(" << keys[index] << " key) {"
				<< endl;
		out << "\tfor(auto indexEntry : " << relation.name << "IndexTable){"
				<< endl;
		out << "\t\tif(get<0>(indexEntry) == key){" << endl;
		out << "\t\t\tint indexToDelete = get<1>(indexEntry);" << endl;
		out << "\t\t\tif(indexToDelete < 0)" << endl;
		out << "\t\t\t\treturn false;" << endl;
		out << "\t\t\titer_swap(" << relation.name
				<< "Table.begin() + indexToDelete, " << relation.name
				<< "Table.end());" << endl;
		out << "\t\t\tget<1>(indexEntry) = -1;" << endl;
		out << "\t\t\treturn true;}}" << endl;
		out << "\treturn false;" << endl;
		out << "}" << endl << endl;
	}
	out << endl;

	out << "// Insert functions" << endl;
	//Generate "insert" functions
	for (uint index = 0; index < schema.relations.size(); index++) {
		Schema::Relation relation = schema.relations[index];
		if (relation.name == "history") {
			continue;
		}
		out << "bool insert" << relation.name << "(" << keys[index] << "key, "
				<< relation.name << " value) {" << endl;
		out << "\tif (find" << relation.name << "(key) != -1)" << endl;
		out << "\t\treturn false;" << endl;
		out << "\t" << relation.name << "Table.push_back(value);" << endl;
		out << "\t" << relation.name << "IndexTable.push_back(make_tuple("
				<< "value.primaryKey, " << relation.name << "Table.size()));"
				<< endl;
		out << "\tsort(" << relation.name << "IndexTable.begin(), "
				<< relation.name << "IndexTable.end());" << endl;
		out << "\treturn true;" << endl;
		out << "}" << endl << endl;
	}

	for (uint index = 0; index < schema.relations.size(); index++) {
		Schema::Relation relation = schema.relations[index];
		out << "void " << relation.name << "Init() {" << endl;
		out << "\tifstream file;" << endl;
		out << "\tfile.open(\"tpcc_" << relation.name << ".tbl\");" << endl;
		out << "\tstring line;" << endl;
		out << "\twhile (getline(file, line)) {" << endl;
		out << "\t\t" << relation.name << " rel;" << endl;
		vector<Schema::Relation::Attribute> attributes;
		string formatString = "";
		vector<string> strings;
		for (Schema::Relation::Attribute attr : relation.attributes) {

			switch (attr.type) {
			case Types::Tag::Char:
			case Types::Tag::Varchar: {
				formatString.append("%s|");
				break;
			}
			case Types::Tag::Integer:
			case Types::Tag::Numeric:
			case Types::Tag::Timestamp: {
				formatString.append("%d|");
				break;
			}
			}
			string tmp = "&rel.";
			tmp.append(attr.name);
			tmp.append(".value");
			strings.push_back(tmp);
		}
		out << "\t\tsscanf(line.c_str(), \"" << formatString << "\", ";
		for (auto s : strings) {
			out << s;
			if (s != strings.back()) {
				out << ",";
			}
		}
		out << ");" << endl;

		out << "\t\t" << relation.name << "Table.push_back(rel);" << endl;
		if(relation.name == "history"){
			out << "}" << endl;
			out << "\tfile.close();" << endl;
			out << "}" << endl;
			continue;
		}
		out << "\trel.primaryKey = make_tuple(";
		for (auto i : relation.primaryKey) {
			out << "rel." << relation.attributes[i].name;
			if (i != relation.primaryKey.back()) {
				out << ",";
			}
		}
		out << ");" << endl;
		out << "\t\t" <<relation.name
				<< "IndexTable.push_back(make_tuple(rel.primaryKey, "
				<< relation.name << "Table.size()));" << endl;
		out << "\t}" << endl;
		out << "\tfile.close();" << endl;
		out << "}" << endl;
		out << endl;
		//TODO: insert elements into indextable

	}

	/*
	 * void warehouseInit() {
	 ifstream warehouseFile;
	 warehouseFile.open("tpcc_warehouse.tbl");
	 string line;
	 while (getline(warehouseFile, line)) {
	 warehouse w;
	 sscanf(line.c_str(), "%d|%s|%s|%s|%s|%s|%s|%d|%d|", &w.w_id, &w.w_name,
	 &w.w_street_1, &w.w_street_2, &w.w_city, &w.w_state, &w.w_zip,
	 &w.w_tax, &w.w_ytd);
	 w.primaryKey = tuple<w.w_id>;
	 warehouseTable.push_back(w);
	 warehouseIndexTable.push_back(make_tuple(w.primaryKey, warehouseTable.size()));
	 }
	 sort(warehouseIndexTable.begin(), warehouseIndexTable.end());
	 warehouseFile.close();
	 }
	 */

//TODO: Generate additional Indices
	outputFile.write(out.str().c_str(), out.str().size());
	outputFile.close();
	cout << "File GeneratedSchema.cpp written." << endl;

}

