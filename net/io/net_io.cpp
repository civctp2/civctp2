




#include "c3.h"

#include "net_types.h"
#include "net_io.h"

NetIO::~NetIO()
{
}

NET_ERR
NetIO::Init(NetIOResponse* response)
{
	m_response = response;
	return NET_ERR_OK;
}

NET_ERR
NetIO::EnumTransports()
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::Host(char* sessionName)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::EnumSessions()
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::Join(sint32 index)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::GetMyId(uint16 & id)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::GetHostId(uint16& id)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::EnumPlayers()
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::Send(uint16 id, sint32 flags, uint8* buf, sint32 len)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::SendCompressed(uint16 id, sint32 flags, uint8* buf, sint32 len)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::SetName(char* name)
{
	return NET_ERR_NOTIMPLEMENTED;
}


NET_ERR
NetIO::SetTransport(sint32 idx)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR
NetIO::SetLobby(char*)
{
	return NET_ERR_NOTIMPLEMENTED;
}
