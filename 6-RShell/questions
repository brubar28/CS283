1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_The remote client determines when a command's output is fully received from the server by using a delimiter to display the transmission's end. In this case the EOF character is sent to signal the end of the output._

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_A networked shell protocol should be defined using a special character such as \0 to detect the end or beginning of a command as well as another character for the whole output. You could also send the length of the output before sending the actual command, in which the receiver could read the exact number of bytes specified. If this is not handled correctly, the TCP may split or combine messages resulting in the receiver getting partial or mutiple messages in a single call._

3. Describe the general differences between stateful and stateless protocols.

_Stateful protocols maintain and keep tract of the clinets state, having things like FTP which remembers the clients current directory and authentication status. This is more complex to implement and has less scalabiltiy resulting from the multiple requests and the resoruces needed to be allocated to maintatin these states. Stateless protocols do not maintain any information about the client, resulting in a simple and isolated implementation that is much more scalable do to the lack of resources needed for allocation._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_UDP is good in some circumstances as it provides a low overhead, as it is efficient when managing small and frequent messages. It also provides low latency, as it does not guarantee delivery or order, avoiding delays associated with transmissions. It is also relatively simple to implement specifically in applications that do not require reliability._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_Socket API is used as the primary interface/abstraction for network communications, allowing applications to send and recieve data over a network using sys calls._
