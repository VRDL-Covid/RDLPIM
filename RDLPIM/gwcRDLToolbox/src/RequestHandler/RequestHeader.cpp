#include "rdlpch.h"
#include "RequestHeader.h"



buffer RequestHeader::Serialise()
{
	memcpy(&m_Header[0], &m_command, 4);
	memcpy(&m_Header[4], &m_bytes, 4);

	return buffer(&m_Header[0], 8);
}

void RequestHeader::Deserialise(const buffer& raw)
{
	memcpy(&m_command, &raw.contents[0], 4);
	memcpy(&m_bytes, &raw.contents[4], 4);
}

void RequestHeader::SetCommand(const Commands& command)
{
	m_command = command;
}

void RequestHeader::SetSize(const uint32_t& bytes)
{
	m_bytes = bytes;
}

void RequestHeader::ProcessHeader(buffer& data)
{
	Deserialise(data);
	data.stripHead(8);
}
