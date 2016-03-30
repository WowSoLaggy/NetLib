// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		Server
// Author:		Anton Egorov
// Description:	Class that starts server on the given port,
//				accepts pending connections, sends and
//				receives data, and controls connected clients
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef NETLIB_SERVER_H
#define NETLIB_SERVER_H


#include "Logger.h"
#include "NetErrCodes.h"
#include "Net.h"
#include "ClientData.h"


namespace NetLib
{

	// Typedef for the client accepted callback
	typedef std::function<void(unsigned int pClientId, std::string pClientAddress, int pClientPort)> ClientAcceptCallback;

	// Typedef for the client disconnected callback
	typedef std::function<void(unsigned int pClientId)> ClientDisconnectCallback;

	// Typedef for the data received callback
	typedef std::function<void(unsigned int pClientId, char *pData, int pDataLength)> ReceiveFromClientCallback;


	// Class that starts server on the given port,
	// accepts pending connections, sends and
	// receives data, and controls connected clients
	class Server
	{
	public:

		// Creates a new server instance
		// Params:
		// [in] ClientAcceptCallback pOnClientAccepted			- callback that is called when the new client is accepted
		// [in] ClientDisconnectCallback pOnClientDisconnected	- callback that is called when the client is disconnected
		// [in] ReceiveFromClientCallback pOnClientDataReceived	- callback taht is called when the new data is received from the client
		Server(
			ClientAcceptCallback pOnClientAccepted,
			ClientDisconnectCallback pOnClientDisconnected,
			ReceiveFromClientCallback pOnReceiveFromClient);
		
		// Default virtual dtor
		virtual ~Server();


		// Returns whether the server is listening for the incoming connections
		bool IsListening();

		// Checks whether the client with the given Id exists
		// Params:
		// [in] unsigned int pClient	- Id of the client to check existance for
		bool CheckClientExists(unsigned int pClientId);


		// Starts the server on the given port
		// Params:
		// [in] int pPort	- port to start server on
		NetErrCode Start(int pPort);

		// Stops the server and disconnects all clients
		NetErrCode Stop();


		// Sends the given data to the client with the given Id
		// Params:
		// [in] unsigned int pClientId	- Id of the client to send data to
		// [in] const char * pData		- pointer to the data to send
		// [in] int pDataLength			- length of the data to send
		NetErrCode SendToClient(unsigned int pClientId, const char *pData, int pDataLength);

		// Disconnects the client with the given Id
		// Params:
		// [in] unsigned int pClientId	- Id of the client to be disconnected
		NetErrCode DisconnectClient(unsigned int pClientId);

	protected:

		volatile bool m_isRunning;							// Variable used to stop server
		std::mutex m_runningLock;							// Mutex to lock access to the m_isRunning variable. Used to stop server
		sockaddr_in m_addrListen;							// Address to listen on
		SOCKET m_sockListen;								// Socket that is used to listen for the pending connections
		ClientAcceptCallback m_onClientAccepted;			// Callback that is called when the new client is accepted
		ClientDisconnectCallback m_onClientDisconnected;	// Callback that is called when the client is disconnected
		ReceiveFromClientCallback m_onReceiveFromClient;	// Callback taht is called when the new data is received from the client

		std::mutex m_clientsLock;							// Mutex to lock access to the m_clients vector
		std::vector<ClientData> m_clients;					// Vector of connected clients
		std::vector<char> m_receiveBuffer;					// Buffer to receive data to


		// Returns whether the server is listening for the incoming connections
		bool CheckIsListening(SOCKET pSocket);

		// Disconnects the client with the given Id (no mutex lock)
		// Params:
		// [in] unsigned int pClientId	- Id of the client to be disconnected
		NetErrCode DisconnectClient_Internal(unsigned int pClientId);


		// Static main loop starter
		// Params:
		// [in] LPVOID pParam	- pointer to the Server class instance
		static DWORD WINAPI MainLoopThreadStarter(LPVOID pParam);


		// Main loop that repeats until the server is stopped
		void MainLoop();

		// Accepts pending connections if any and calls the OnClientAccepted callback
		void TryAccept();

		// Checks all client's sockets if they have something to receive
		void TryReceive();

		// Disconnects clients with the closed connection
		void DisconnectClosed();


		// Disconnects all clients and releases used memory
		NetErrCode Cleanup();
	};

} // NetLib


#endif // NETLIB_SERVER_H
