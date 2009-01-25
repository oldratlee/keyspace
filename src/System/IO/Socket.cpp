#include "Socket.h"
#include "System/Log.h"
#include <machine/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

bool Socket::Create(int type_)
{
	int ret;
	int	sockopt;
	
	if (fd >= 0)
	{
		Log_Message("Called Create() on existing socket");
		return false;
	}
	
	fd = socket(AF_INET, type_, 0);
	if (fd < 0)
	{
		Log_Errno();
		return false;
	}
	
	sockopt = 1;
	ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
	if (ret < 0)
	{
		Log_Errno();
		Close();
		return false;
	}
	
	type = type_;
	listening = false;
	
	return true;
}

bool Socket::SetNonblocking()
{
	int ret;
	
	if (fd < 0)
	{
		Log_Message("SetNonblocking on invalid file descriptor");
		return false;
	}
	
	ret = fcntl(fd, F_SETFL, FNONBLOCK | FASYNC);
	if (ret < 0)
	{
		Log_Errno();
		return false;
	}
	
	return true;
}

bool Socket::Bind(int port)
{
	int					ret;
	struct sockaddr_in	sa;
	
	memset(&sa, 0, sizeof(sa));
	sa.sin_port = htons((uint16_t)port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
	ret = bind(fd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		Log_Errno();
		Close();
		return false;
	}
	
	return true;
}

bool Socket::Listen(int port, int backlog)
{
	int	ret;
	
	if (!Bind(port))
		return false;
	
	ret = listen(fd, backlog);
	if (ret < 0)
	{
		Log_Errno();
		Close();
		return false;
	}
	
	listening = true;
	
	return true;
}

bool Socket::Accept(Socket *newSocket)
{
	newSocket->fd = accept(fd, NULL, NULL);
	if (newSocket->fd < 0)
	{
		Log_Errno();
		Close();
		return false;
	}
	
	return true;
}

void Socket::Close()
{
	if (fd != -1)
	{
		close(fd);
		fd = -1;
	}
}
