#include "rdlpch.h"
#include "reqElement.hpp"

// type to convert types prior to sending over TCP/IP using char array buffer.
union convertable {
	char Char[8];
	double Double = 0.0;
	float Float;
	int Int;
	bool Bool;
};

responceElement::responceElement()
{
	//varName.set("");
	//type.set("");
	bytes = 0;
	data = nullptr;
}

responceElement::responceElement(const char* var)
{
	buffer temp(var);
	temp.nullTerminate();
	varName.set(var);
	type.set("");
	bytes = 0;
	data = nullptr;
}

responceElement::responceElement(std::string var)
{
	varName.set(var.c_str());
	type.set("");
	bytes = 0;
	data = nullptr;
}

responceElement::responceElement(const buffer& var)
{
	varName = var;
	type.set("");
	bytes = 0;
	data = nullptr;
}

responceElement::responceElement(buffer* var)
{
	varName = *var;
	type.set("");
	bytes = 0;
	data = nullptr;
}



responceElement::~responceElement()
{
	if (data != nullptr) {
		free(data);
	}
}

int responceElement::appendData(char* dst, int dSize, char* src, int sSize)
{
	int end = dSize - 1;
	int i = 0;
	while (dst[end] != '=') {
		end--;
	}
	end++;

	if (sSize > (dSize - end)) {
		return 1;
	}

	while (i < sSize) {
		dst[end + i] = src[i];
		i++;
	}

	dst[end + i] = '=';
	return 0;
}

int responceElement::appendValueData(char* dst, int dSize, char* src, int sSize)
{
	int end = dSize - 1;
	int i = 0;
	while (dst[end] != '=') {
		end--;
	}
	end++;

	if (sSize > (dSize - end)) {
		return 1;
	}

	while (i < sSize) {
		dst[end + i] = src[i];
		i++;
	}
	return 0;
}

void responceElement::set(bool in)
{
	sizeData(sizeof(bool));
	memcpy(data, &in, sizeof(bool));
	bytes = sizeof(bool);
	type.set("bool");
}
void responceElement::set(int in)
{
	sizeData(sizeof(int));
	memcpy(data, &in, sizeof(int));
	bytes = sizeof(int);
	type.set("int");
}
void responceElement::set(float in)
{
	sizeData(sizeof(float));
	memcpy(data, &in, sizeof(float));
	bytes = sizeof(float);
	type.set("float");
}
void responceElement::set(double in)
{
	sizeData(sizeof(double));
	memcpy(data, &in, sizeof(double));
	bytes = sizeof(double);
	type.set("double");
}
int responceElement::set(char* in, int size)
{
	sizeData(size);
	memset(data, '\0', size);

	for (int i = 0; i < size; i++) {
		data[i] = in[i];
	}
	type.set("blob");
	bytes = size;
	return 0;
}
int responceElement::set(buffer* in)
{
	sizeData(in->size);
	memset(data, '\0', in->size);

	for (int i = 0; i < in->size; i++) {
		data[i] = in->contents[i];
	}
	type.set("blob");
	bytes = in->size;
	return 0;
}
int responceElement::set(const buffer &in)
{
	sizeData(in.size);
	memset(data, '\0', in.size);

	for (int i = 0; i < in.size; i++) {
		data[i] = in.contents[i];
	}
	type.set("blob");
	bytes = in.size;
	return 0;
}
void responceElement::set(const rdlData& rdldata)
{
	//set name
	varName.set(rdldata.name);

	//set bytes
	bytes = rdldata.bytes;
	
	//set type
	type.set(rdldata.ctype);

	if (!strncmp(rdldata.ctype,"L",1) && bytes == 1) {
		type.set("bool");
	}

	if (!strncmp(rdldata.ctype, "I", 1) &&bytes  == 4) {
		type.set("int");
	}

	if (!strncmp(rdldata.ctype, "R", 1)&& bytes == 8) {
		type.set("double");
	}



	//set data
	sizeData(bytes);

	for (int i = 0; i < bytes; i++) {
		data[i] = rdldata.data[i];
	}
}

int responceElement::deserialise(buffer* in)
{
	int nameSize = 0;
	int typeSize = 0;
	int itt = 0;
	char* temp;

	//if no data to serialise return 0 elements deserialised
	if (in->size == 0) {
		return -1;
	}

	//get variable name
	while (in->contents[itt] != '=' && itt < in->size) {
		itt++;
	}

	//if no '=' found its not properly formatted
	if (itt == 0 || itt >= in->size) {
		return -1;
	}

	else {
		nameSize = itt;
		itt = 0;
	}

	temp = (char*)malloc(nameSize * sizeof(char));
	
	for (int i = 0; i < nameSize; i++) {
		temp[i] = in->contents[i];
	}

	varName.set(temp,nameSize);


	//get type data
	nameSize++;
	while (in->contents[nameSize + itt] != '=' && (itt + nameSize) < in->size) {
		itt++;
	}

	if (itt == 0 || itt >= in->size) {
		return 1;
	}
	else {
		typeSize = itt;
		itt = 0;
	}

	for (int i = 0; i < nameSize+typeSize; i++) {
		temp[i] = in->contents[i+nameSize];
	}

	// copy data over.
	type.set(temp,typeSize);

	

	//get number of bytes
	typeSize++;
	memcpy(&bytes, &(in->contents[nameSize + typeSize]), sizeof(int));
	typeSize += 5;

	sizeData(bytes);
	memcpy(data, &(in->contents[nameSize + typeSize]), bytes);



	return 4;


}

void responceElement::serialise(buffer* output)
{
	int tempSize;
	if (bytes < 4) {
		tempSize = 8;
	}
	else {
		tempSize = bytes;
	}

	char* temp = (char*)malloc(sizeof(char)*tempSize);

	//build varname
	output->set(varName.contents,varName.size);

	//build type
	output->append("=");
	output->append(type);

	//build bytes
	std::memcpy(temp, &bytes, sizeof(int));

	output->append("=");
	buffer bytesData(temp, 4);
	output->append(bytesData);

	//build data
	buffer dataData(data, bytes);
	output->append(dataData);
	output->prepend("{");
	output->append("}");
	free(temp);
}


buffer responceElement::serialise()
{
	buffer output;
	int tempSize;
	if (bytes < 4) {
		tempSize = 8;
	}
	else {
		tempSize = bytes;
	}

	char* temp = (char*)malloc(sizeof(char) * tempSize);

	//build varname
	output.set(varName.contents, varName.size);

	//build type
	output.append("=");
	output.append(type);

	//build bytes
	std::memcpy(temp, &bytes, sizeof(int));

	output.append("=");
	buffer bytesData(temp, 4);
	output.append(bytesData);

	//build data
	buffer dataData(data, bytes);
	output.append("=");
	output.append(dataData);

	output.prepend("{");
	output.append("}");

	free(temp);

	return output;
}


void responceElement::sizeData(int sBytes){
	if (data == nullptr) {
		data = (char*)malloc(sBytes);
	}
	else {
		data = (char*)realloc(data, sBytes);
	}
}

//copy constructors
responceElement::responceElement(const responceElement& other)
{
	varName = other.varName;
	type = other.type;
	bytes = other.bytes;

	data = (char*) malloc(bytes);

	for (int i = 0; i < bytes; i++) {
		data[i] = other.data[i];
	}
}


responceElement& responceElement::operator=(const responceElement& other)
{
	varName = other.varName;
	type = other.type;
	bytes = other.bytes;

	data = (char*)malloc(bytes);

	for (int i = 0; i < bytes; i++) {
		data[i] = other.data[i];
	}

	return *this;
}