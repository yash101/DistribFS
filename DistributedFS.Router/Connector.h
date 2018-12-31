#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#include "platformindependence.h"
#include "packetstructure.h"
#include <atomic>

namespace Connector {
	class ServerConnection;
	class ClientConnection;
	class AddressPortTuple;
}

namespace Connector {

	typedef Protocol::Control::ResponsePacket
		(*ControlPacketReceiveHandlerCB)(
		 Protocol::Control::ControlPacket);
	typedef bool(*AddressFilterHandlerCB)
		(uint32_t ip_address,
		 uint16_t local_port,
		 uint16_t remote_port);

	class BasicServerConnection {
		virtual Protocol::Control::ResponsePacket TransmitControlPacket(
			Protocol::Control::ControlPacket tx
		) = 0;

		virtual uint32_t Start(
		) = 0;

		virtual uint32_t StartAsync(
		) = 0;
	};

	class ServerConnection : public BasicServerConnection {

	private:

		class ListeningPort4 {

		public:

			CrossPlatform::FileDescriptor_t FileDescriptor;
			struct sockaddr_in AddressStructure;

			ListeningPort4();
		};

		class ListeningPort6 {

		public:

			CrossPlatform::FileDescriptor_t FileDescriptor;
			struct sockaddr_in6 AddressStructure;

			ListeningPort6();
		};

		int64_t tv_sec;
		int64_t tv_usec;

		std::vector<ListeningPort4> Ports4;
		std::vector<ListeningPort6> Ports6;

		Connector::ControlPacketReceiveHandlerCB ctrl_recv_cb;
		Connector::AddressFilterHandlerCB address_filter_cb;
		int queueLength;
		std::atomic<bool> listening;

		void initialize(
		);

		uint32_t InitializeListeningSockets(
			fd_set* fds,
			int* nfds
		);
		uint32_t SelectLoop(
			fd_set& readfds,
			int nfds
		);
		uint32_t ShutDownCleanUp(
		);

	public:

		ServerConnection(
		);
		~ServerConnection(
		);

		Protocol::Control::ResponsePacket TransmitControlPacket(
			Protocol::Control::ControlPacket&
		);

		void RegisterControlPacketReceiveHandler(
			Connector::ControlPacketReceiveHandlerCB callback
		);

		void RegisterAddressFilterHandler(
			Connector::AddressFilterHandlerCB
		);

		void AddListeningAddress4(
			struct sockaddr_in
		);

		void AddListeningAddress6(
			struct sockaddr_in6
		);

		uint32_t SetTimeout(
			int64_t sec,
			int64_t usec
		);

		uint32_t Start(
		);

		uint32_t Stop(
		);

		uint32_t SetTCPQueueLength(
			int n
		);

	};

	class ClientConnection {

	private:

		CrossPlatform::FileDescriptor_t FileDescriptor;
		Connector::ControlPacketReceiveHandlerCB ctrl_recv_cb;

		void initialize();

	public:

		ClientConnection();
		~ClientConnection();

		Protocol::Control::ResponsePacket TransmitControlPacket(
			Protocol::Control::ControlPacket
		);

		Protocol::Control::ControlPacket WaitForControlPacket(
		);

		void RegisterControlPacketReceiveHandler(
			Connector::ControlPacketReceiveHandlerCB callback
		);

		void Connect(
			uint32_t address,
			uint16_t port
		);

	};

	class AddressPortTuple {

	public:

		uint32_t Address;
		uint16_t Port;

	};
}

#endif