===========
UDP Sockets
===========

KatieLib exposes a very basic, async implementation of UDP.

.. class:: KnUdpSocket
   
   .. function:: KnUdpSocket(address: string, port: integer, server: boolean): KnUdpSocket
      
      Create a new UDP socket which is associated with the given address and
      port. The address must currently be an IP address (no hostname).
      The port can be any valid UDP port the game has permission to open.
      
      If ``server`` is **true**, then the :c:expr:`bind()` syscall will be
      used instead of :c:expr:`connect()` when setting up the socket. You
      probably want to use :c:expr:`connect()`. Note that when running in server
      mode it's not currently possible to reply as the API lacks a way to send
      to a specific address currently.
   
   .. method:: send(data: string): boolean
      
      Send a datagram with the given data to the address associated with this
      port. Returns a boolean indicating success.
   
   .. method:: recieve(): string | nil
      
      Recieve a datagram from the associated address. Returns the data as a
      string or **nil** if there are no more datagrams.
   
   .. data:: fd: integer
      
      The raw file descriptor associated with the UDP socket
