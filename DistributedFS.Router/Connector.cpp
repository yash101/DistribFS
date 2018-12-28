#include "Connector.h"
#include "errors.h"

#include <unistd.h>
#include <sys/socket.h>

// Allows all traffic (does nothing)
static bool DefaultAddressFilter(
	uint32_t ip_address,
	uint16_t local_port,
	uint16_t remote_port
) {
	return true;
}

// Initializes the ListeningPort4 class
Connector::ServerConnection::ListeningPort4::ListeningPort4(
) :	FileDescriptor(CrossPlatform::FileDescriptorInitializer())
{}

// Initializes the ListeningPort6 class
Connector::ServerConnection::ListeningPort6::ListeningPort6(
) :	FileDescriptor(CrossPlatform::FileDescriptorInitializer())
{}

// Initializes the ServerConnection class and its variables
Connector::ServerConnection::ServerConnection(
) {
	initialize();
}

// Destructs the ServerConnection class and its variables
Connector::ServerConnection::~ServerConnection(
) {
	queueLength = 5;
	ctrl_recv_cb = nullptr;
	address_filter_cb = DefaultAddressFilter;
}

// Initialzes the variables in ServerConnection
void Connector::ServerConnection::initialize(
) {
	ctrl_recv_cb = nullptr;
	address_filter_cb = nullptr;
}

Protocol::Control::ResponsePacket Connector::ServerConnection::TransmitControlPacket(
	Protocol::Control::ControlPacket& packet
) {
}

void Connector::ServerConnection::RegisterAddressFilterHandler(
	Connector::AddressFilterHandlerCB cb
) {
	address_filter_cb = cb;
}

void Connector::ServerConnection::RegisterControlPacketReceiveHandler(
	Connector::ControlPacketReceiveHandlerCB cb
) {
	ctrl_recv_cb = cb;
}

void Connector::ServerConnection::AddListeningAddress4(
	struct sockaddr_in address
) {
	ListeningPort4 nPort;
	nPort.AddressStructure = address;
	Ports4.push_back(nPort);
}

void Connector::ServerConnection::AddListeningAddress6(
	struct sockaddr_in6 address
) {
	ListeningPort6 nPort;
	nPort.AddressStructure = address;
	Ports6.push_back(nPort);
}

uint32_t Connector::ServerConnection::Start(
) {
	if (listening)
		return ERR_ALREADY_LISTENING;

	// Set up all the ipv4 ports
	for (size_t i = 0; i < Ports4.size(); i++) {
		Ports4[i].FileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
		if (Ports4[i].FileDescriptor < 0) {
			return ERR_UNABLE_TO_CREATE_SOCKET;
		}

		if (bind(
				Ports4[i].FileDescriptor,
				(struct sockaddr*) &Ports4[i].AddressStructure,
				sizeof(Ports4[i].AddressStructure)) < 0
		) {
			return ERR_BIND_FAILED;
		}

		listen(
			Ports4[i].FileDescriptor,
			queueLength
		);
	}

	// Set up all the ipv6 ports
	for (size_t i = 0; i < Ports6.size(); i++) {
		Ports6[i].FileDescriptor = socket(AF_INET6, SOCK_STREAM, 0);
		if (Ports6[i].FileDescriptor < 0) {
			return ERR_UNABLE_TO_CREATE_SOCKET;
		}

		if (bind(
				Ports6[i].FileDescriptor,
				(struct sockaddr*) &Ports4[i].AddressStructure,
				sizeof(Ports4[i].AddressStructure) < 0)
		) {
			return ERR_BIND_FAILED;
		}

		listen(
			Ports6[i].FileDescriptor,
			queueLength
		);
	}

	// Wait for a connection or data
	int nfds = (Ports4.size() + Ports6.size());
	int ret;

	fd_set readfds;
	fd_set writefds;

	while (true) {
	}

	listening = true;
	return ERR_SUCCESS;
}

uint32_t Connector::ServerConnection::Stop(
) {
}

uint32_t Connector::ServerConnection::SetTCPQueueLength(
	int length
) {
	queueLength = length;
}