#pragma once
#include"Core/buffer.hpp"
#include"job.hpp"

class RequestHeader
{
public:
	RequestHeader() = default;

	~RequestHeader() = default;
	buffer Serialise();

	void Deserialise(const buffer& raw);

	void SetCommand(const Commands& command);
	void SetSize(const uint32_t& bytes);


private:

	char m_Header[2 * sizeof(uint32_t)];
	uint32_t m_bytes;
	Commands m_command;
};