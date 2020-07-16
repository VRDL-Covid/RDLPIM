#include "rdlpch.h"
#include "buffer.hpp"

Buffer Buffer::PassChunk(char first, char second)
{
	uint32_t start = 0;
	uint32_t end = 0;
	uint32_t bytes = 0;
	Buffer out;

	//find firt delimiter
	for (int i = 0; i < size; i++) {
		if (contents[i] != first)
			++start;
		else
			break;
	}

	//find second delimiter
	for (int i = start + 1; i < size; i++) {
		if (contents[i] == second) {
			end = i;
			break;
		}
			
	}

	bytes = end - start + 1;

	char* temp = (char*)malloc(bytes);

	for (int i = 0; i < bytes; i++) {
		temp[i] = contents[i + start];
	}

	out.set(temp, bytes);

	free(temp);

	stripHead((int)bytes);
	out.stripHead('{');
	out.stripTail('}');
	return out;

}

Buffer::~Buffer()
{
	//free memory allocated to the contents buffer
	if (contents != nullptr) {
		free(contents);
	}

}

Buffer::Buffer()
{
	//initialise a null buffer
	contents = nullptr;
	size = 0;
}

Buffer::Buffer(const char* src)
{
	//find size of src, treated as a null terminated string.
	size = 0;

	while (src[size] != '\0') {
		size++;
	}

	//allocate memory to the contents buffer
	contents = (char*)malloc(sizeof(char) * size);

	//copy the source to the contents buffer except the final null terminator.
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}
}

Buffer::Buffer(char* src, int sSize)
{
	//initialise the size
	size = sSize;

	//allocate memore to the contents buffer
	contents = (char*)malloc(sizeof(char) * size);

	//copy the source to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}

}

Buffer::Buffer(const Buffer& other)
{
	size = other.size;
	contents = (char*)malloc(size * sizeof(char));

	for (int i = 0; i < size; i++) {
		contents[i] = other.contents[i];
	}
}

//public Methods

void Buffer::set(void* src, int iSize)
{
	//redefine the size of the buffer
	size = iSize;

	char* temp = (char*)malloc(size);
	memcpy(temp, src, size);


	//resize the memory allocated to the contents buffer
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char) * size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char) * size);
	}

	//copy the contents of the src to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = temp[i];
	}

	free(temp);
}

void Buffer::set(const char* src)
{
	//find the size of the source, treated as a null terminated string.
	size = 0;
	while (src[size] != '\0') {
		size++;
	}

	//resize the memory allocated to the contents buffer
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char) * size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char) * size);
	}

	//copy the contents of the src to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}
}

void Buffer::set(char* src, int iSize)
{
	//redefine the size of the buffer
	size = iSize;

	//resize the memory allocated to the contents buffer
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char) * size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char) * size);
	}

	//copy the contents of the src to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}

}

//TODO:GWC - This is broken, needs to be derefferenced
int Buffer::diff(Buffer* buf1, Buffer* buf2)
{
	int size = 0;
	int dif = 0;

	if (buf1->size > buf2->size) {
		size = buf2->size;
		dif = buf1->size - buf2->size;
	}
	else {
		size = buf1->size;
		dif = -buf1->size + buf2->size;
	}

	int i = 0;

	for (int i = 0; i < size; i++) {
		if (buf1->contents[i] != '\0' && buf2->contents[i] != '\0') {
			if (buf1->contents[i] != buf2->contents[i]) {
				dif++;
			}
		}
	}

	return dif;
}

int Buffer::diff(Buffer* buf1, const Buffer* buf2)
{
	int size = 0;
	int dif = 0;

	if (buf1->size > buf2->size) {
		size = buf2->size;
		dif = buf1->size - buf2->size;
	}
	else {
		size = buf1->size;
		dif = -buf1->size + buf2->size;
	}

	int i = 0;

	for (int i = 0; i < size; i++) {
		if (buf1->contents[i] != '\0' && buf2->contents[i] != '\0') {
			if (buf1->contents[i] != buf2->contents[i]) {
				dif++;
			}
		}
	}

	return dif;
}

bool Buffer::operator==(const Buffer& buf1)
{
	if (size != buf1.size)
		return false;

	return(!diff(this, &buf1));
}

bool Buffer::operator!=(const Buffer& buf1)
{
	if (size != buf1.size)
		return true;

	return(diff(this, &buf1));
}

std::ostream& operator<< (std::ostream& out, const Buffer& c)
{
	Buffer temp = c;
	temp.nullTerminate();

	out << temp.contents;
	return out;
}

void Buffer::append(const Buffer& tail)
{
	//calculate new size
	int newSize = size + tail.size;

	//copy head packet to temporary buffer
	char* tempBuffer = (char*)malloc(sizeof(char) * newSize);
	for (int i = 0; i < size; i++) {
		tempBuffer[i] = contents[i];
	}

	//append tail to temporary buffer
	for (int i = size; i < newSize; i++) {
		tempBuffer[i] = tail.contents[i + tail.size - newSize];
	}

	//realocate contents size
	size = newSize;
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char) * size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char) * size);
	}

	//temp buffer to contents.

	for (int i = 0; i < size; i++) {
		contents[i] = tempBuffer[i];
	}

	free(tempBuffer);
}

void Buffer::append(const char* tail)
{
	//get size of src without null termination
	int srcSize = 0;
	while (tail[srcSize] != '\0') {
		srcSize++;
	}

	//get new size
	int newSize = size + srcSize;

	//copy head packet to temporary buffer
	char* tempBuffer = (char*)malloc(sizeof(char) * newSize);
	for (int i = 0; i < size; i++) {
		tempBuffer[i] = contents[i];
	}

	//append tail to temporary buffer
	for (int i = size; i < newSize; i++) {
		tempBuffer[i] = tail[i + srcSize - newSize];
	}

	//realocate contents size
	size = newSize;
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char) * size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char) * size);
	}

	//temp buffer to contents.

	for (int i = 0; i < size; i++) {
		contents[i] = tempBuffer[i];
	}

	free(tempBuffer);
}

void Buffer::prepend(const Buffer& head)
{
	//Calculate new size
	int newSize = head.size + size;

	//copy head to temporary buffer
	char* temporaryBuff = (char*)malloc(sizeof(char) * newSize);

	for (int i = 0; i < head.size; i++) {
		temporaryBuff[i] = head.contents[i];
	}

	//append this to temporary buffer
	for (int i = head.size; i < newSize; i++) {
		temporaryBuff[i] = contents[i + size - newSize];
	}

	//reallocate memory assigned to contents buffer
	size = newSize;
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char) * size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char) * size);
	}

	//copy temporary buffer to contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = temporaryBuff[i];
	}

	//clean up
	free(temporaryBuff);
}

void Buffer::prepend(const char* head)
{
	//get head size without null termination
	int headSize = 0;
	while (head[headSize] != '\0') {
		headSize++;
	}

	//calculate newSize of contents buffer
	int newSize = headSize + size;

	//copy head buffer to temporary buffer
	char* tempBuff = (char*)malloc(sizeof(char) * newSize);
	for (int i = 0; i < headSize; i++) {
		tempBuff[i] = head[i];
	}

	//append current buffer to temporary buffer
	for (int i = headSize; i < newSize; i++) {
		tempBuff[i] = contents[i + size - newSize];
	}

	//reallocate memory assigned to contents buffer
	size = newSize;
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char) * size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char) * size);
	}

	//copy temporary buffer to contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = tempBuff[i];
	}

	//clean up
	free(tempBuff);
}

void Buffer::nullTerminate()
{
	char* temp = (char*)malloc(sizeof(char) + sizeof(char) * this->size);
	
	
	int newSize = this->size + 1;

	for (int i = 0; i < this->size; i++) {
		temp[i] = this->contents[i];
	}

	temp[newSize - 1] = '\0';

	this->size = newSize;

	this->contents = (char*)realloc(this->contents, sizeof(char) * this->size);


	for (int i = 0; i < this->size; i++) {
		this->contents[i] = temp[i];
	}

	free(temp);
}

void Buffer::fullPrint() const
{
	for (int i = 0; i < size; i++) {
		std::cout << contents[i];
	}

	std::cout << std::endl;
}

void Buffer::stripHead(int N)
{

	if (N > size) {
		set("");
		return;
	}

	if (N < 0) {
		return;
	}

	char* temp = (char*)malloc(size);

	for (int i = N; i < size; i++) {
		temp[i - N] = contents[i];
	}

	set(temp, size - N);

	free(temp);
}

void Buffer::stripHead(char seperator)
{
	int it = 0;

	while ((contents[it] != seperator) && (it < size)) {
		it++;
	}

	if (it == size) {
		return;
	}

	stripHead((it + 1));
}

void Buffer::stripTail(int N)
{

	if (N > size) {
		set("");
		return;
	}

	if (N < 0) {
		return;
	}

	char* temp = (char*)malloc(size);

	for (int i = 0; i < size - N; i++) {
		temp[i] = contents[i];
	}

	set(temp, size - N);

	free(temp);
}

void Buffer::stripTail(char seperator)
{
	int it = 0;

	while ((contents[size - 1 - it] != seperator) && (it < (size - 1))) {
		it++;
	}

	if (it == (size - 1)) {
		return;
	}

	stripTail(it + 1);
}

std::string Buffer::ToString() const
{
	Buffer temp = *this;
	temp.nullTerminate();
	return std::string(temp.contents);

}


Buffer& Buffer::operator=(const Buffer& other)
{
	this->size = other.size;

	if (this->contents == nullptr) {
		this->contents = (char*)malloc(sizeof(char) * other.size);
	}
	else {
		this->contents = (char*)realloc(this->contents, sizeof(char) * other.size);
	}

	for (int i = 0; i < size; i++) {
		this->contents[i] = other.contents[i];
	}

	return *this;
}

Buffer& Buffer::operator=(Buffer&& other)
{
	size = other.size;
	other.size = 0;

	contents = other.contents;
	other.contents = nullptr;

	return *this;
}

std::istream& operator>> (std::istream& in, Buffer& buf)
{
	std::string temp;
	in >> temp;
	buf.set(temp.c_str());
	return in;
}