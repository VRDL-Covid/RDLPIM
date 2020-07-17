#pragma once
// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to contain buffer data to be sent via sockets
class Buffer
{
private:


public:
	//members
	char* contents;
	int size;

	//methods

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: A setter to set the contents of the buffer

	void set(void* src, int iSize);
	void set(const char* src);
	void set(char* src, int iSize);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: methods to compare the contents of two buffers, returns the number of differences found.
	static int diff(Buffer* buf1, Buffer* buf2);
	static int diff(Buffer* buf1, const Buffer* buf2);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Overloaded equality and inequality operators to allow inline boolean evaluation
	bool operator==(const Buffer& buf1);
	bool operator!=(const Buffer& buf1);
	friend std::ostream& operator<< (std::ostream& out, const Buffer& c);

	//Author: Guy Collins
	// Date: 28/01/2020
	// Description: A method to append one buffer to another
	void append(const Buffer& tail);
	void append(const char* tail);

	//Author: Guy Collins
	// Date: 28/01/2020
	// Description: A method to append one buffer to another
	void prepend(const Buffer& head);
	void prepend(const char* head);

	//Author: Guy Collins
	//Date: 04/02/2020
	//Method to null terminate the contents of a buffer.
	void nullTerminate();

	//Author:Guy Collins
	//date: 04/02/2020
	//method to fully print the contents of a buffer to the console for debugging, ignores null termination.
	void fullPrint() const;

	//Author:Guy Collins
	//date: 07/02/2020
	//Methods to remove characters from buffer arrays (number of bytes or upto and including a speration character.
	void stripHead(int N);
	void stripHead(char seperator);
	void stripTail(int N);
	void stripTail(char seperator);


	//Author:Guy Collins
	//14/05/2020
	//Method to convert buffer to a string
	std::string ToString() const;

	// Author: Guy Collinso
	// Date: 20/01/2020
	// Description: Constructors and destructors.
	Buffer();
	Buffer(const char* string);
	Buffer(char* src, int sSize);

	//copy constructor
	Buffer(const Buffer& other);

	//copy assignment overload
	Buffer& operator=(const Buffer& other);

	//move operator
	Buffer& operator=(Buffer&&);

	//input overload
	friend std::istream& operator>> (std::istream& in, Buffer& buf);
	friend std::ostream& operator<< (std::ostream& out, Buffer& buf);

	Buffer PassChunk(char first, char second);
	Buffer PassChunk(char delimiter);

	~Buffer();
};
