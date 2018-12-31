# DistributedFS Protocol Definition

The DistributedFS protocol is designed for transfer performance. Defined are the different packet types in this protocol:

## Connection Begins:

When a connection is initiated, the client sends a `Protocol::InitialNegotiationPacket` (currently a 16-bit unsigned integer protocol version). The server responds with a `Protocol::ControlPacket`. The packet_type field of the `Protocol::ControlPacket` will be used as a status code. It will be set as 0 if the connection is being accepted, and non-zero if not. Definitions for these non-zero values are still to be developed. The server will respond with a `Protocol::ResponsePacket` to begin the connection. This can contain client configuration information, etc. Below is a diagram of the communication:

Diagram:

```
+------------+                               +----------+
|            |<--[InitialNegotiationPacket]--|          |
|            |-------[ControlPacket]-------->|          |
|   Router   |<------[ResponsePacket]--------|  Client  |
|  (server)  |                               |  (sink)  |
|            |<--------[ Traffic ]---------->|          |
|            |                               |          |
+------------+                               +----------+
```

## Control Packets:

Control packets are used to transmit information via a key-pair multimap. Below is the data layout:

```
       UNNAMED CONTROL PACKET LAYOUT:
+------+-------------+---------------------------+-----------+
| Bits | Name        | Description               | Data Type |
+======+=============+===========================+===========+
|  8   | packet_type | purpose of the packet     | uint8     |
|  32  | packet_size | number of bytes in packet | uint32    |
|  16  | kv_count    | number of keys and values | uint16    |
|  8   | key_length  | length of a key           | uint8     |
|  16  | val_length  | length of a value         | uint16    |
|      | key         | key_length bytes          | uint8[]   |
|      | value       | val_length bytes          | uint8[]   |
+------+-------------+---------------------------+-----------+

{key_length, val_length, key, value} repeated kv_count times
```

```
       NAMED CONTROL PACKET LAYOUT:
+------+-------------+---------------------------+-----------+
| Bits | Name        | Description               | Data Type |
+======+=============+===========================+===========+
|  8   | packet_type | purpose of the packet     | uint8     |
|  32  | packet_size | number of bytes in packet | uint32    |
|  8   | name_length | Length of packet name     | uint8     |
|      | name        | name_length bytes         | uint8[]   |
|  16  | kv_count    | number of keys and values | uint16    |
|  8   | key_length  | length of a key           | uint8     |
|  16  | val_length  | length of a value         | uint16    |
|      | key         | key_length bytes          | uint8[]   |
|      | value       | val_length bytes          | uint8[]   |
+------+-------------+---------------------------+-----------+

{key_length, val_length, key, value} repeated kv_count times
```

```
       RESPONSE PACKET LAYOUT:
+------+-------------+---------------------------+
| Bits | Name        | Description               |
+======+=============+===========================+
|  8   | packet_type | packet type               |
|  32  | packet_size | packet size               |
|  16  | kv_count    | number of keys and values |
|  8   | key_length  | length of key             |
|  16  | val_length  | length of value           |
|      | key         | key data                  |
|      | value       | value data                |
+------+-------------+---------------------------+

{key_length, val_length, key, value} repeated kv_count times
```
