# DistributedFS Protocol Definition

The DistributedFS protocol is designed for transfer performance. Defined are the different packet types in this protocol:

## Connection Begins:

When a connection begins, a packet of the type, `Protocol::InitialNegotiationPacket` is sent by the client. The server responds with a uint8_t of 255 if the server does not support the protocol version requested, and then closes the connection. If the server supports the connection, a uint8_t of 0 is returned to the client. The client thereafter negotiates its connection with the server using `Protocol::ControlPacket`s and `Protocol::ResponsePackets`.

## Control Packets:

Control packets are used to transmit information via a key-pair multimap. Below is the data layout:

```
       UNNAMED CONTROL PACKET LAYOUT:
+------+-------------+---------------------------+
| Bits | Name        | Description               |
+======+=============+===========================+
|  8   | packet_type | purpose of the packet     |
|  16  | kv_count    | number of keys and values |
|  8   | key_length  | length of a key           |
|  16  | val_length  | length of a value         |
|      | key         | key_length bytes          |
|      | value       | val_length bytes          |
+------+-------------+---------------------------+

{key_length, val_length, key, value} repeated kv_count times
```

```
       NAMED CONTROL PACKET LAYOUT:
+------+-------------+---------------------------+
| Bits | Name        | Description               |
+======+=============+===========================+
|  8   | packet_type | purpose of the packet     |
|  8   | name_length | Length of packet name     |
|      | name        | name_length bytes         |
|  16  | kv_count    | number of keys and values |
|  8   | key_length  | length of a key           |
|  16  | val_length  | length of a value         |
|      | key         | key_length bytes          |
|      | value       | val_length bytes          |
+------+-------------+---------------------------+

{key_length, val_length, key, value} repeated kv_count times
```

```
       RESPONSE PACKET LAYOUT:
+------+-------------+---------------------------+
| Bits | Name        | Description               |
+======+=============+===========================+
|  16  | kv_count    | number of keys and values |
|  8   | key_length  | length of key             |
|  16  | val_length  | length of value           |
|      | key         | key data                  |
|      | value       | value data                |
+------+-------------+---------------------------+

{key_length, val_length, key, value} repeated kv_count times
```
