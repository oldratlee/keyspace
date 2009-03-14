#include "System/Buffer.h"
#include "TransportUDPWriter.h"

TransportUDPWriter::TransportUDPWriter()
{
	ioproc = NULL;
}

void TransportUDPWriter::Init(IOProcessor* ioproc_, Scheduler*, Endpoint &ep)
{
	ioproc = ioproc_;
	endpoint = ep;
}

void TransportUDPWriter::Write(ByteString &bs)
{
	int ret;
	
	socket.Create(UDP);
	socket.SetNonblocking();

	// We use direct sendto here because otherwise
	//  we should buffer the packets here.
	ret = sendto(socket.fd, bs.buffer, bs.length, 0,
				 (const sockaddr *) &endpoint.sa,
				 sizeof(sockaddr));
	
	if (ret < 0)
		Log_Errno();
	
	socket.Close();
}

