#pragma once
// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to contain buffer data to be sent via sockets
class buffer
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
	static int diff(buffer* buf1, buffer* buf2);
	static int diff(buffer* buf1, const buffer* buf2);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Overloaded equality and inequality operators to allow inline boolean evaluation
	bool operator==(const buffer& buf1);
	bool operator!=(const buffer& buf1);
	friend std::ostream& operator<< (std::ostream& out, const buffer& c);

	//Author: Guy Collins
	// Date: 28/01/2020
	// Description: A method to append one buffer to another
	void append(const buffer& tail);
	void append(const char* tail);

	//Author: Guy Collins
	// Date: 28/01/2020
	// Description: A method to append one buffer to another
	void prepend(const buffer& head);
	void prepend(const char* head);

	//Author: Guy Collins
	//Date: 04/02/2020
	//Method to null terminate the contents of a buffer.
	void nullTerminate();

	//Author:Guy Collins
	//date: 04/02/2020
	//method to fully print the contents of a buffer to the console for debugging, ignores null termination.
	void fullPrint();

	//Author:Guy Collins
	//date: 07/02/2020
	//Methods to remove characters from buffer arrays (number of bytes or upto and including a speration character.
	void stripHead(int N);
	void stripHead(char seperator);
	void stripTail(int N);
	void stripTail(char seperator);

	// Author: Guy Collinso
	// Date: 20/01/2020
	// Description: Constructors and destructors.
	buffer();
	buffer(const char* string);
	buffer(char* src, int sSize);

	//copy constructor
	buffer(const buffer& other);

	//copy assignment overload
	buffer& operator=(const buffer& other);

	//move operator
	buffer& operator=(buffer&&);

	//input overload
	friend std::istream& operator >>(std::istream& in, buffer& buf);

	~buffer();
};
