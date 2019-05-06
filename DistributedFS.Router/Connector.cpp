#include "Connector.h"
#include "errors.h"

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

uint32_t Connector::ServerConnection::SetTimeout(
	int64_t sec,
	int64_t usec
) {
	if (listening)
		return ERR_ALREADY_LISTENING;

	tv_sec = sec;
	tv_usec = usec;

	return ERR_SUCCESS;
}

#define FAIL_CLOSE_SOCKETS(i, x, a, b) \
	for(size_t j = 0; j < i; j++) \
		close(a[i].FileDescriptor); \
	for(size_t j = 0; j < x; j++) \
		close(b[i].FileDescriptor) \

uint32_t Connector::ServerConnection::InitializeListeningSockets(
	fd_set* fds,
	int* nfds
) {
	CrossPlatform::FileDescriptor_t largest;
	bool flg_largest_init;

	// Set up all the ipv4 ports
	for (size_t i = 0; i < Ports4.size(); i++) {
		Ports4[i].FileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
		if (Ports4[i].FileDescriptor < 0) {
			FAIL_CLOSE_SOCKETS(i - 1, 0, Ports4, Ports6);
			return ERR_UNABLE_TO_CREATE_SOCKET;
		}

		static const int yes = 1;

		if (setsockopt(
			Ports4[i].FileDescriptor,
			SOL_SOCKET,
			SO_REUSEADDR,
			&yes,
			sizeof(yes)
		) == -1) {
			FAIL_CLOSE_SOCKETS(i, 0, Ports4, Ports6);
			return ERR_SOCKOPT_FAILED;
		}

		if (bind(
			Ports4[i].FileDescriptor,
			reinterpret_cast<struct sockaddr*>(&Ports4[i].AddressStructure),
			sizeof(Ports4[i].AddressStructure)) < 0
			) {
			FAIL_CLOSE_SOCKETS(i, 0, Ports4, Ports6);
			return ERR_BIND_FAILED;
		}

		listen(
			Ports4[i].FileDescriptor,
			queueLength
		);

		FD_SET(Ports4[i].FileDescriptor, fds);

		if (flg_largest_init || Ports4[i].FileDescriptor > largest) {
			flg_largest_init = true;
			largest = Ports4[i].FileDescriptor;
		}
	}

	// Set up all the ipv6 ports
	for (size_t i = 0; i < Ports6.size(); i++) {
		Ports6[i].FileDescriptor = socket(AF_INET6, SOCK_STREAM, 0);
		if (Ports6[i].FileDescriptor < 0) {
			FAIL_CLOSE_SOCKETS(Ports4.size(), i, Ports4, Ports6);
			return ERR_UNABLE_TO_CREATE_SOCKET;
		}

		if (bind(
			Ports6[i].FileDescriptor,
			reinterpret_cast<struct sockaddr*>(&Ports4[i].AddressStructure),
			sizeof(Ports4[i].AddressStructure) < 0)
			) {
			FAIL_CLOSE_SOCKETS(Ports4.size(), i, Ports4, Ports6);
			return ERR_BIND_FAILED;
		}

		listen(
			Ports6[i].FileDescriptor,
			queueLength
		);

		FD_SET(Ports6[i].FileDescriptor, fds);

		if (flg_largest_init || Ports6[i].FileDescriptor > largest) {
			flg_largest_init = true;
			largest = Ports6[i].FileDescriptor;
		}
	}

	*nfds = largest + 1;

	return ERR_SUCCESS;
}

uint32_t Connector::ServerConnection::SelectLoop(
	fd_set& readfds,
	int nfds
) {
	int ret;
	fd_set fds;
	while (true) {
		fds = readfds;
		ret = select(nfds, &fds, NULL, NULL, NULL);

		if (ret < 0) {
			int error = errno;

			if (error == EBADF ||
				error == EINVAL ||
				error == ENOMEM
			) {
				ShutDownCleanUp();
				break;
			}
		}

		if (!listening) {
			ShutDownCleanUp();
			break;
		}

		// find the ready file descriptor, and accept
		for (size_t i = 0; i < Ports4.size(); i++) {
			if (ret == 0) break;
			if (FD_ISSET(Ports4[i].FileDescriptor, &fds)) {
				ret--;

				// accept the connection
				Connector::ServerConnection::Ipv4ListeningWorker* worker = new Connector::ServerConnection::Ipv4ListeningWorker;

				static int AddrStructSz = sizeof(worker->AddressStructure);
				worker->FileDescriptor = accept(
					Ports4[i].FileDescriptor,
					(struct sockaddr*) &worker->AddressStructure,
					(socklen_t*) &AddrStructSz
				);
			}
		}

		for (size_t i = 0; i < Ports6.size(); i++) {
			if (ret == 0) break;
			if (FD_ISSET(Ports6[i].FileDescriptor, &fds)) {
				ret--;

				// accept the connection
				Connector::ServerConnection::Ipv6ListeningWorker* worker = new Connector::ServerConnection::Ipv6ListeningWorker;
				
				static int AddrStructSz = sizeof(worker->AddressStructure);
				worker->FileDescriptor = accept(
					Ports6[i].FileDescriptor,
					(struct sockaddr*) &worker->AddressStructure,
					(socklen_t*)&AddrStructSz
				);
			}
		}
	}
}

#ifdef _WIN32
#define close closesocket
#endif

uint32_t Connector::ServerConnection::ShutDownCleanUp(
) {
	// 
	for (auto it = Ports4.begin(); it != Ports4.end(); ++it) {
		shutdown(it->FileDescriptor, SHUT_RDWR);
		close(it->FileDescriptor);
	}

	for (auto it = Ports6.begin(); it != Ports6.end(); ++it) {
		shutdown(it->FileDescriptor, SHUT_RDWR);
		close(it->FileDescriptor);
	}
}

#ifdef _WIN32
#undef close
#endif

uint32_t Connector::ServerConnection::Start(
) {
	if (listening)
		return ERR_ALREADY_LISTENING;

	fd_set readfds;
	FD_ZERO(&readfds);
	int nfds;

	InitializeListeningSockets(&readfds, &nfds);

	while (true) {
		SelectLoop(readfds, nfds);
	}

	listening = true;
	return ERR_SUCCESS;
}

#undef FAIL_CLOSE_SOCKETS

uint32_t Connector::ServerConnection::Stop(
) {
}

uint32_t Connector::ServerConnection::SetTCPQueueLength(
	int length
) {
	queueLength = length;
}