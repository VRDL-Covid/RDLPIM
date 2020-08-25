#pragma once
#include"Core/buffer.hpp"
#include"job.hpp"

class RequestHeader
{
public:
	RequestHeader() = default;

	~RequestHeader() = default;
	Buffer Serialise();

	void Deserialise(const Buffer& raw);

	void SetCommand(const Commands& command);
	void SetSize(const uint32_t& bytes);
	Buffer SetData(const Buffer& data);

	void ProcessHeader(Buffer& data);

	const Commands& GetCommand() const { return m_command; }

private:

	char m_Header[2 * sizeof(uint32_t)];
	uint32_t m_bytes;
	Commands m_command;
};