#include "rdlpch.h"
#include "DataElement.hpp"

// type to convert types prior to sending over TCP/IP using char array buffer.
union convertable {
	char Char[8];
	double Double = 0.0;
	float Float;
	int Int;
	bool Bool;
};

DataElement::DataElement()
{
	//varName.set("");
	//type.set("");
	m_Bytes = 0;
	m_data = nullptr;
}

DataElement::DataElement(const char* var)
{
	Buffer temp(var);
	temp.nullTerminate();
	m_VarName.set(var);
	m_Type.set("");
	m_Bytes = 0;
	m_data = nullptr;
}

DataElement::DataElement(std::string var)
{
	m_VarName.set(var.c_str());
	m_Type.set("");
	m_Bytes = 0;
	m_data = nullptr;
}

DataElement::DataElement(const Buffer& var)
{
	m_VarName = var;
	m_Type.set("");
	m_Bytes = 0;
	m_data = nullptr;
}

DataElement::DataElement(Buffer* var)
{
	m_VarName = *var;
	m_Type.set("");
	m_Bytes = 0;
	m_data = nullptr;
}



DataElement::~DataElement()
{
	if (m_data != nullptr) {
		free(m_data);
	}
}


void DataElement::set(bool in)
{
	sizeData(sizeof(bool));
	memcpy(m_data, &in, sizeof(bool));
	m_Bytes = sizeof(bool);
	m_Type.set("bool");
}
void DataElement::set(int in)
{
	sizeData(sizeof(int));
	memcpy(m_data, &in, sizeof(int));
	m_Bytes = sizeof(int);
	m_Type.set("int");
}
void DataElement::set(float in)
{
	sizeData(sizeof(float));
	memcpy(m_data, &in, sizeof(float));
	m_Bytes = sizeof(float);
	m_Type.set("float");
}
void DataElement::set(double in)
{
	sizeData(sizeof(double));
	memcpy(m_data, &in, sizeof(double));
	m_Bytes = sizeof(double);
	m_Type.set("double");
}
int DataElement::set(char* in, int size)
{
	sizeData(size);
	memset(m_data, '\0', size);

	for (int i = 0; i < size; i++) {
		m_data[i] = in[i];
	}
	m_Type.set("blob");
	m_Bytes = size;
	return 0;
}
int DataElement::set(Buffer* in)
{
	sizeData(in->size);
	memset(m_data, '\0', in->size);

	for (int i = 0; i < in->size; i++) {
		m_data[i] = in->contents[i];
	}
	m_Type.set("blob");
	m_Bytes = in->size;
	return 0;
}
int DataElement::set(const Buffer &in)
{
	sizeData(in.size);
	memset(m_data, '\0', in.size);

	for (int i = 0; i < in.size; i++) {
		m_data[i] = in.contents[i];
	}
	m_Type.set("blob");
	m_Bytes = in.size;
	return 0;
}
void DataElement::set(const rdlData& rdldata)
{
	//set name
	m_VarName.set(rdldata.name);

	//set bytes
	m_Bytes = rdldata.bytes;
	
	//set type
	m_Type.set(rdldata.ctype);

	if (!strncmp(rdldata.ctype,"L",1) && m_Bytes == 1) {
		m_Type.set("bool");
	}

	if (!strncmp(rdldata.ctype, "I", 1) &&m_Bytes  == 4) {
		m_Type.set("int");
	}

	if (!strncmp(rdldata.ctype, "R", 1)&& m_Bytes == 8) {
		m_Type.set("double");
	}



	//set data
	sizeData(m_Bytes);

	for (int i = 0; i < m_Bytes; i++) {
		m_data[i] = rdldata.data[i];
	}
}

int DataElement::deserialise(const Buffer& in)
{
	int nameSize = 0;
	int typeSize = 0;
	int itt = 0;
	char* temp;
	Buffer incpy = in;

	//if no data to serialise return 0 elements deserialised
	if (incpy.size == 0) {
		return -1;
	}

	//get variable name
	while (incpy.contents[itt] != '=' && itt < incpy.size) {
		itt++;
	}

	//if no '=' found its not properly formatted
	if (itt == 0 || itt >= incpy.size) {
		return -1;
	}

	else {
		nameSize = itt;
		itt = 0;
	}

	temp = (char*)malloc(nameSize * sizeof(char));
	
	for (int i = 0; i < nameSize; i++) {
		temp[i] = incpy.contents[i];
	}

	m_VarName.set(temp,nameSize);


	//get type data
	nameSize++;
	while (incpy.contents[nameSize + itt] != '=' && (itt + nameSize) < incpy.size) {
		itt++;
	}

	if (itt == 0 || itt >= incpy.size) {
		return 1;
	}
	else {
		typeSize = itt;
		itt = 0;
	}

	for (int i = 0; i < nameSize+typeSize; i++) {
		temp[i] = incpy.contents[i+nameSize];
	}

	// copy data over.
	m_Type.set(temp,typeSize);

	

	//get number of bytes
	typeSize++;
	memcpy(&m_Bytes, &(incpy.contents[nameSize + typeSize]), sizeof(int));
	typeSize += 5;

	sizeData(m_Bytes);
	memcpy(m_data, &(incpy.contents[nameSize + typeSize]), m_Bytes);



	return 4;


}

Buffer DataElement::serialise()
{
	Buffer output;
	int tempSize;
	if (m_Bytes < 4) {
		tempSize = 8;
	}
	else {
		tempSize = m_Bytes;
	}

	char* temp = (char*)malloc(sizeof(char) * tempSize);

	//build varname
	output.set(m_VarName.contents, m_VarName.size);

	//build type
	output.append("=");
	output.append(m_Type);

	//build bytes
	std::memcpy(temp, &m_Bytes, sizeof(int));

	output.append("=");
	Buffer bytesData(temp, 4);
	output.append(bytesData);

	//build data
	Buffer dataData(m_data, m_Bytes);
	output.append("=");
	output.append(dataData);

	output.prepend("{");
	output.append("}");

	free(temp);

	return output;
}


void DataElement::sizeData(int sBytes){
	if (m_data == nullptr) {
		m_data = (char*)malloc(sBytes);
	}
	else {
		m_data = (char*)realloc(m_data, sBytes);
	}
}

//copy constructors
DataElement::DataElement(const DataElement& other)
{
	m_VarName = other.m_VarName;
	m_Type = other.m_Type;
	m_Bytes = other.m_Bytes;

	m_data = (char*) malloc(m_Bytes);

	for (int i = 0; i < m_Bytes; i++) {
		m_data[i] = other.m_data[i];
	}
}

//move constructors
DataElement::DataElement(DataElement&& other)
{
	m_VarName = std::move(other.m_VarName);
	m_Type = std::move(other.m_Type);
	m_Bytes = std::move(other.m_Bytes);

	m_data = std::move(other.m_data);

}


DataElement& DataElement::operator=(const DataElement& other)
{
	m_VarName = other.m_VarName;
	m_Type = other.m_Type;
	m_Bytes = other.m_Bytes;

	m_data = (char*)malloc(m_Bytes);

	for (int i = 0; i < m_Bytes; i++) {
		m_data[i] = other.m_data[i];
	}

	return *this;
}

DataElement& DataElement::operator=(DataElement&& other)
{
	m_VarName = std::move(other.m_VarName);
	m_Type =    std::move(other.m_Type);
	m_Bytes =   std::move(other.m_Bytes);

	m_data = std::move(other.m_data);

	return *this;
}