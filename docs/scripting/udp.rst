===========
UDP Sockets
===========

KatieLib exposes a very basic, async implementation of UDP.

.. class:: KnUdpSocket([address: string, port: integer]): KnUdpSocket
      
   Create a new UDP socket which is optionally bound to the given address and
   port.
   
   Binding a UDP socket to an address and port is required for reciving
   messages, but not for sending them.
   
   .. method:: send(data: string, address: string, port: integer): boolean
      
      Send a datagram with the given data to the address and port. Returns a
      boolean indicating success.
   
   .. method:: recieve(): string, string, integer | nil, nil, nil
      
      Get the next datagram in the queue, if there is one.
      
      If a datagram is available, this returns three values: the data of the
      datagram itself, the address that sent the datagram, and the port it came
      from.
      
      If there are no more datagrams or there is an error, this returns three
      nil values.
   
   .. method:: close()
      
      Closes the UDP socket and frees resources
   
   .. data:: fd: integer
      
      The raw file descriptor associated with the UDP socket
