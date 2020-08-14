#pragma once
/// @brief The Buffer class is a container to hold serialised data.
/// 
/// The primary use of the Buffer object is to hold serialised data which can be transfered by connectionObjects maintained between the connectionManager and clientManager. Buffer objects are useful for containing
/// large volumes of machiene readable only data.  Utility functions have been included to allow the developer to compare the contents of Buffer objects and determing if they are the same or different.
/// 
/// Manipulation methods to append, prepend, process chunks and print to console beyond that which a std::string would permite by ignoring null termination convention.  
/// 
/// 
/// ###Example
/// ~~~~~~~~~~.cpp
/// 
/// Buffer buff1("test1");
/// Buffer buff2("test2");
/// 
/// if(buff1 == buff2){
///		std::cout << "This cannot happen" << std::endl;
/// } else {
///		std::cout << "This will always happen" << std::endl;
/// }
/// 
/// DataElement someData;
/// Buffer serialised;
/// 
/// serialised = someData.deserialise();
/// 
/// //print the binary equivalent of a DataElement including and nulls that may or may not exist.
/// serialised.fullPrint();
/// ~~~~~~~~~~

class Buffer
{

public:
	
	/// @brief A method to return a const pointer to the Buffers contents
	/// @return Buffers raw contents
	const char* GetContents() const { return contents; }

	/// @brief A method to get the size of the data contained within the Buffer object
	/// @return int size of contents
	const int GetSize() const { return size; }

	/// @brief A method to store primitive types within a buffer object (int, float, double etc)
	/// 
	/// ###Example
	/// ~~~~~~~~~.cpp
	/// double myDouble = 3.14159;
	/// 
	/// Buffer serialisedDouble;
	/// serialisedDouble.set(&myDouble,sizeof(double));
	/// 
	/// //Print the binary of a double.
	/// serialisedDouble.fullPrint();
	/// 
	/// ~~~~~~~~~
	/// @param src pointer to primitive
	/// @param iSize size of the primitive
	void set(void* src, int iSize);

	/// @brief A method to store a const char* source into a Buffer object
	/// 
	/// This is primarily used to store R-value reference strings
    /// ###Example
	/// ~~~~~~~~~.cpp
	/// double myDouble = 3.14159;
	/// 
	/// Buffer serialisedRvalueString;
	/// serialisedRvalueString.set("my const char star");
	/// 
	/// //Print the binary of a double.
	/// serialisedRvalueString.fullPrint();
	/// 
	/// ~~~~~~~~~
	/// @param src 
	void set(const char* src);

	/// @brief Method to pass data held within a char array into a Buffer object
	/// @warning depricated, use set(void* src, int iSize)
	/// @param src source
	/// @param iSize size of source array
	void set(char* src, int iSize);

	/// @brief A static method to determin the number of differences beween two buffers
	/// 
	/// A a difference is given if the lengeth of the buffers are difference, the differences are then counted over the range of the shortest buffer.
	/// @param buf1 left hand side 
	/// @param buf2 right hand side
	/// @return the integer number of differences found
	static int diff(const Buffer& buf1, const Buffer& buf2);

	/// @brief Equality operator overload
	/// @param buf1 right hand side of equality
	/// @return status of equality true if equal.
	bool operator==(const Buffer& buf1);

	/// @brief Inequality operator overload
	/// @param buf1 right hand side of inequality
	/// @return status of equality true if not equal.
	bool operator!=(const Buffer& buf1);



	/// @brief A method to append a buffer to this buffer
	/// @param tail buffer data to append
	void append(const Buffer& tail);

	/// @brief A method to append a R value char array to this buffer
	/// @param tail append source
	void append(const char* tail);

	/// @brief A method to prepent this buffer with another
	/// @param head source data
	void prepend(const Buffer& head);

	/// @brief A method to prepend this buffer with an char array
	/// @param head source data
	void prepend(const char* head);

	/// @brief A method to null terminate the buffer, required for use with std::string like behaviour
	void nullTerminate();

	/// @brief Method to print the contents of a buffer to std::cout.
	/// 
	/// Unlike a std::string, this will not stop printing if a null char is encountered, instead it will print to the end of its buffer.
	void fullPrint() const;

	/// @brief Method to remove N byts of data from the Buffers head
	/// @param N Number of bytes
	void stripHead(int N);

	/// @brief A method to remove up to and including a char, for example a '{' char.
	/// @param seperator delimiter
	void stripHead(char seperator);

	/// @brief A method to remove N bytes from the end of the Buffer object
	/// @param N Number of bytes
	void stripTail(int N);

	/// @brief A method of striping data from the end of the Buffer object up to and including a delimiter, for example a '}' char.
	/// @param seperator 
	void stripTail(char seperator);


	/// @brief A method of converting a string like Buffer contents to a std::string. Should only be used with human readable contents.
	/// @return 
	std::string ToString() const;

	// Author: Guy Collinso
	// Date: 20/01/2020
	// Description: Constructors and destructors.
	Buffer();
	Buffer(const char* string);
	Buffer(char* src, int sSize);

	/// @brief Copy Constructor
	/// @param other 
	Buffer(const Buffer& other);

	/// @brief copy assignent overload
	/// @param other RHS
	/// @return 
	Buffer& operator=(const Buffer& other);

	/// @brief move assignment overload
	/// @param  RHS
	/// @return 
	Buffer& operator=(Buffer&&) noexcept;

	/// @brief std::ostream operator overload
	/// @param out reference to the outstream
	/// @param c const reference of Buffer to be streamed
	/// @return ostream reference
	friend std::ostream& operator<< (std::ostream& out, const Buffer& c);

	/// @brief std::istream operator overload
	/// @param in reference to the outstream
	/// @param buf const reference of Buffer to be streamed
	/// @return istream reference
	friend std::istream& operator>> (std::istream& in, Buffer& buf);

	/// @brief A method to return part of a serialised array of Buffer objects.
	/// @warning This method is destructive, it 'pops' the Buffer part from the full serialised data
	/// ###Example
	/// ~~~~~~~~~~.cpp
	/// Buffer array("{part1}{part2}");
	/// 
	/// buffer part1;
	/// buffer part2;
	/// 
	/// part1 = array.PassChunk('{','}');
	/// part2 = array.PassChunk('{','}');
	/// 
	/// //part1 contains "part1"
	/// //part2 contains "part2"
	/// //array contains ""
	/// ~~~~~~~~~~
	/// @param first 
	/// @param second 
	/// @return 
	Buffer PassChunk(char first, char second);


	~Buffer();

private:
	char* contents;
	int size;
private:
	void resizeContents(size_t newSize);
};
