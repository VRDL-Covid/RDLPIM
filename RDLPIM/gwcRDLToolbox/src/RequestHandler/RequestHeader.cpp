#include "rdlpch.h"
#include "RequestHeader.h"



Buffer RequestHeader::Serialise()
{
	memcpy(&m_Header[0], &m_command, 4);
	memcpy(&m_Header[4], &m_bytes, 4);

	return Buffer(&m_Header[0], 8);
}

void RequestHeader::Deserialise(const Buffer& raw)
{
	memcpy(&m_command, &raw.GetContents()[0], 4);
	memcpy(&m_bytes, &raw.GetContents()[4], 4);
}

void RequestHeader::SetCommand(const enum Commands& command)
{
	m_command = command;
}

void RequestHeader::SetSize(const uint32_t& bytes)
{
	m_bytes = bytes;
}

Buffer RequestHeader::SetData(const Buffer& data)
{
	Buffer ret;

	m_bytes = data.GetSize();

	ret = Serialise();
	ret.append(data);
	return ret;
}

void RequestHeader::ProcessHeader(Buffer& data)
{
	Deserialise(data);
	data.stripHead(8);
}
