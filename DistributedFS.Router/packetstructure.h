#ifndef _PACKETSTRUCTURE_H
#define _PACKETSTRUCTURE_H

#include "platformindependence.h"
#include <vector>

namespace Protocol {
	class KeyValuePair;
	class InitialNegotiationPacket;
	namespace Control {
		class ControlPacket;
		class ResponsePacket;
	}
}

namespace Protocol {

	class KeyValuePair {

	public:

		uint8_t key_length;
		uint16_t value_length;
		void* key;
		void* value;
	};

	class InitialNegotiationPacket {

	public:

		uint16_t protocol_version;
	};

	class InitialNegotiationPacketServerResponse {

	public:

		uint8_t status;
	};

	namespace Control {
		/*
		  ControlPacket
		A ControlPacket is used to transfer information between nodes. This is done via a key-value multimap.
		A 16-bit value contains the number of keys that are present in the transmission. 
		*/
		class ControlPacket {
			
		public:

			uint8_t packet_type;

			uint8_t packet_name_length;	// only in named control packets
			unsigned char* packet_name;			// only in named control packets

			uint16_t kv_count;
			Protocol::KeyValuePair* arguments;
		};

		class ResponsePacket {

		public:

			uint16_t kv_count;
			Protocol::KeyValuePair* arguments;
		};

		class RawDataPacket {

		public:

			uint64_t data_offset;
			uint16_t data_length;
			uint8_t* data_buffer;
		};

		class DifferentialDataChunk {
			uint64_t offset;
			uint16_t chunk_length;
			uint8_t flags;
			char* data_buffer;
		};

		class DifferentialDataPacket {
			uint16_t chunk_count;
			DifferentialDataChunk* chunks;
		};

		namespace PacketType {
			const static uint8_t named = 0;
			const static uint8_t chunk_request = 1;
			const static uint8_t fs_action = 2;
			const static uint8_t authentication = 3;
			const static uint8_t file_open = 4;
			const static uint8_t file_close = 5;
			const static uint8_t stat_file = 6;
		}
	}
}

#endif