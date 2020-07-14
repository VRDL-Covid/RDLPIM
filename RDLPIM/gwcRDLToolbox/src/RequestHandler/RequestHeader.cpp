#include "rdlpch.h"
#include "RequestHeader.h"



buffer RequestHeader::Serialise()
{
	return buffer(&m_Header[0], 8);
}

void RequestHeader::Deserialise(const buffer& raw)
{
	memcpy(&m_command, raw.contents, 4);
	memcpy(&m_bytes, raw.contents+4, 4);
}

void RequestHeader::SetCommand(const Commands& command)
{
	m_command = command;
}

void RequestHeader::SetSize(const uint32_t& bytes)
{
	m_bytes = bytes;
}
