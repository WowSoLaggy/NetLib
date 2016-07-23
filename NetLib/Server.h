// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		Server
// Author:		Anton Egorov
// Description:	Class that starts the server on the given port,
//				accepts pending connections, sends and
//				receives data, and controls connected clients
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef NETLIB_SERVER_H
#define NETLIB_SERVER_H


#include <Windows.h>
#include <mutex>
#include <string>
#include <functional>
#include <atomic>
#include <vector>

#include "NetErrCodes.h"
#include "ClientData.h"


namespace NetLib
{
	using namespace NetErrCodes;


	// Typedef for the client accepted callback
	typedef std::function<void(CLIENTID pClientId, const std::string &pClientAddress, int pClientPort)> ServerCb_ClientAccepted;

	// Typedef for the client disconnected callback
	typedef std::function<void(CLIENTID pClientId)> ServerCb_ClientDiconnected;

	// Typedef for the data received callback
	typedef std::function<void(CLIENTID pClientId, char *pData, int pDataLength)> ServerCb_ReceivedFromClient;


	// Class that starts server on the given port,
	// accepts pending connections, sends and
	// receives data, and controls connected clients
	class Server
	{
	public:

		// Creates a new server instance
		// Params:
		// [in] ServerCb_ClientAccepted pOnClientAccepted			- callback that is called when the new client is accepted
		// [in] ServerCb_ClientDiconnected pOnClientDisconnected	- callback that is called when the client is disconnected
		// [in] ServerCb_ReceivedFromClient pOnReceivedFromClient	- callback that is called when the new data is received from the client
		Server(
			ServerCb_ClientAccepted pOnClientAccepted,
			ServerCb_ClientDiconnected pOnClientDisconnected,
			ServerCb_ReceivedFromClient pOnReceivedFromClient);
		
		//
		virtual ~Server();


		// Returns whether the server is listening for incoming connections
		bool IsListening() const;

		// Checks whether the client with the given Id exists
		// Params:
		// [in] CLIENTID pClient	- Id of the client to check existance for
		bool CheckClientExists(CLIENTID pClientId) const;


		// Starts the server on the given port
		// Params:
		// [in] int pPort	- port to start server on
		NetErrCode Start(int pPort);

		// Stops the server and disconnects all clients
		NetErrCode Stop();


		// Sends the given data to the client with the given Id
		// Params:
		// [in] CLIENTID pClientId	- Id of the client to send data to
		// [in] const char * pData	- pointer to the data to send
		// [in] int pDataLength		- length of the data to send
		NetErrCode SendToClient(CLIENTID pClientId, const char *pData, int pDataLength);

		// Disconnects the client with the given Id
		// Params:
		// [in] CLIENTID pClientId	- Id of the client to be disconnected
		NetErrCode DisconnectClient(CLIENTID pClientId);


	protected:


		std::atomic_bool m_isRunning;						// Variable used to stop the server
		sockaddr_in m_addrListen;							// Address to listen on
		SOCKET m_sockListen;								// Socket that is used to listen for the pending connections

		ServerCb_ClientAccepted m_onClientAccepted;			// Callback that is called when the new client is accepted
		ServerCb_ClientDiconnected m_onClientDisconnected;	// Callback that is called when the client is disconnected
		ServerCb_ReceivedFromClient m_onReceivedFromClient;	// Callback that is called when the new data is received from the client

		mutable std::recursive_mutex m_clientsLock;			// Mutex to lock access to the m_clients vector
		std::vector<ClientData> m_clients;					// Vector of connected clients
		std::vector<char> m_receiveBuffer;					// Buffer to receive data to


		// Returns whether the given socket is listening for the incoming connections
		bool IsListening(SOCKET pSocket) const;


		// Disconnects the client with the given Id (no mutex lock)
		// Params:
		// [in] CLIENTID pClientId	- Id of the client to be disconnected
		NetErrCode DisconnectClient_internal(CLIENTID pClientId);


		// Main loop that repeats until the server is stopped
		void MainLoop();

		// Accepts pending connections if any and calls the OnClientAccepted callback
		void TryAccept();

		// Checks all client's sockets if they have something to receive
		void TryReceive();


		// Disconnects all clients and releases used memory
		NetErrCode Cleanup();

	};

} // ns NetLib


#endif // NETLIB_SERVER_H
