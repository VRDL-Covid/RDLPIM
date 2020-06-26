#include "buffer.hpp"

buffer::~buffer()
{
	//free memory allocated to the contents buffer
	if (contents != nullptr) {
		free(contents);
	}

}

buffer::buffer()
{
	//initialise a null buffer
	contents = nullptr;
	size = 0;
}

buffer::buffer(const char* src)
{
	//find size of src, treated as a null terminated string.
	size = 0;

	while (src[size] != '\0') {
		size++;
	}

	//allocate memory to the contents buffer
	contents = (char*)malloc(sizeof(char)*size);

	//copy the source to the contents buffer except the final null terminator.
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}
}

buffer::buffer(char* src, int sSize)
{
	//initialise the size
	size = sSize;

	//allocate memore to the contents buffer
	contents = (char*)malloc(sizeof(char)*size);

	//copy the source to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}

}

buffer::buffer(const buffer& other)
{
	size = other.size;
	contents = (char*)malloc(size * sizeof(char));

	for (int i = 0; i < size; i++) {
		contents[i] = other.contents[i];
	}
}

//public Methods

void buffer::set(void* src, int iSize)
{
	//redefine the size of the buffer
	size = iSize;

	char* temp = (char*)malloc(size);
	memcpy(temp, src, size);


	//resize the memory allocated to the contents buffer
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char)*size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char)*size);
	}

	//copy the contents of the src to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = temp[i];
	}

	free(temp);
}

void buffer::set(const char* src)
{
	//find the size of the source, treated as a null terminated string.
	size = 0;
	while (src[size] != '\0') {
		size++;
	}

	//resize the memory allocated to the contents buffer
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char)*size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char)*size);
	}

	//copy the contents of the src to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}
}

void buffer::set(char* src, int iSize)
{
	//redefine the size of the buffer
	size = iSize;

	//resize the memory allocated to the contents buffer
	if (contents == nullptr) {
		contents = (char*)malloc(sizeof(char)*size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char)*size);
	}

	//copy the contents of the src to the contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = src[i];
	}

}

//TODO:GWC - This is broken, needs to be derefferenced
int buffer::diff(buffer* buf1, buffer* buf2)
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

int buffer::diff(buffer* buf1, const buffer* buf2)
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

bool buffer::operator==(const buffer &buf1)
{
	bool state1, state2;
	state1 = size == buf1.size;
	state2 = diff(this, &buf1) == 0;
	return((state1) && (state2));

}

bool buffer::operator!=(const buffer &buf1)
{
	bool state1, state2;
	state1 = size == buf1.size;
	state2 = diff(this, &buf1) == 0;
	return!((state1) && (state2));
}

std::ostream& operator<< (std::ostream&out, const buffer&c)
{
	buffer temp = c;
	temp.nullTerminate();
	
	out << temp.contents;
	return out;
}

void buffer::append(const buffer &tail)
{
	//calculate new size
	int newSize = size + tail.size;

	//copy head packet to temporary buffer
	char* tempBuffer = (char*)malloc(sizeof(char)*newSize);
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
		contents = (char*)malloc(sizeof(char)*size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char)*size);
	}

	//temp buffer to contents.

	for (int i = 0; i < size; i++) {
		contents[i] = tempBuffer[i];
	}

	free(tempBuffer);
}

void buffer::append(const char* tail)
{
	//get size of src without null termination
	int srcSize = 0;
	while (tail[srcSize] != '\0') {
		srcSize++;
	}

	//get new size
	int newSize = size + srcSize;

	//copy head packet to temporary buffer
	char* tempBuffer = (char*)malloc(sizeof(char)*newSize);
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
		contents = (char*)malloc(sizeof(char)*size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char)*size);
	}

	//temp buffer to contents.

	for (int i = 0; i < size; i++) {
		contents[i] = tempBuffer[i];
	}

	free(tempBuffer);
}

void buffer::prepend(const buffer &head)
{
	//Calculate new size
	int newSize = head.size + size;

	//copy head to temporary buffer
	char* temporaryBuff = (char*)malloc(sizeof(char)*newSize);

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
		contents = (char*)malloc(sizeof(char)*size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char)*size);
	}

	//copy temporary buffer to contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = temporaryBuff[i];
	}

	//clean up
	free(temporaryBuff);
}

void buffer::prepend(const char* head)
{
	//get head size without null termination
	int headSize = 0;
	while (head[headSize] != '\0') {
		headSize++;
	}

	//calculate newSize of contents buffer
	int newSize = headSize + size;

	//copy head buffer to temporary buffer
	char* tempBuff = (char*)malloc(sizeof(char)*newSize);
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
		contents = (char*)malloc(sizeof(char)*size);
	}
	else {
		contents = (char*)realloc(contents, sizeof(char)*size);
	}

	//copy temporary buffer to contents buffer
	for (int i = 0; i < size; i++) {
		contents[i] = tempBuff[i];
	}

	//clean up
	free(tempBuff);
}

void buffer::nullTerminate()
{
	char* temp = (char*)malloc(1 + sizeof(char) * size);
	int newSize = size + 1;

	for (int i = 0; i < size; i++) {
		temp[i] = contents[i];
	}

	temp[newSize - 1] = '\0';

	size = newSize;

	contents = (char*)realloc(contents, sizeof(char)*size);

	for (int i = 0; i < size; i++) {
		contents[i] = temp[i];
	}

	free(temp);
}

void buffer::fullPrint()
{
	for (int i = 0; i < size; i++) {
		std::cout << contents[i];
	}

	std::cout << std::endl;
}

void buffer::stripHead(int N)
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
}

void buffer::stripHead(char seperator)
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

void buffer::stripTail(int N)
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
}

void buffer::stripTail(char seperator)
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


buffer& buffer::operator=(const buffer& other)
{
	size = other.size;

	contents = (char*)malloc(size);

	for (int i = 0; i < size; i++) {
		contents[i] = other.contents[i];
	}

	return *this;
}