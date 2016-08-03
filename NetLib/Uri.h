#pragma once

#ifndef NETLIB_URI_H
#define NETLIB_URI_H


#include <string>

#include "NetErrCodes.h"
#include "Protocols.h"
#include "Credentials.h"


namespace NetLib
{

	// Class that represents Uri
	class Uri
	{
	public:

		// Creates an empty Uri
		Uri();

		// Creates a Uri and fills it with the given fields
		Uri(const std::string &pProtocol, const std::string &pCredentials,
			const std::string &pHost, const std::string &pPort, const std::string &pPath,
			const std::string &pParameters, const std::string &pFragment);

		// Creates a Uri from the given string
		Uri(const std::string &pUriString);

		virtual ~Uri();


		// Returns the Uri's protocol
		const Protocol & GetProtocol()			const { return m_protocol; }

		// Returns the Uri's protocol as a string
		const std::string & GetProtocolStr()	const { return m_protocolStr; }


		// Returns the Uri's credentials
		const Credentials & GetCredentials()	const { return m_credentials; }

		// Returns the Uri's credentials as a string
		const std::string & GetCredentialsStr()	const { return m_credentialsStr; }


		// Returns the Uri's host
		const std::string & GetHostStr()		const { return m_host; }


		// Returns the Uri's port
		int GetPort()							const { return m_port; }

		// Returns the Uri's port as a string
		const std::string & GetPortStr()		const { return m_portStr; }


		// Returns the Uri's path
		const std::string & GetPathStr()		const { return m_path; }

		// Returns Uri's parameters
		const std::string & GetParametersStr()	const { return m_parameters; }

		// Returns the Uri's fragment
		const std::string & GetFragmentStr()	const { return m_fragment; }


		// Converts Uri to the string with credentials
		std::string ToString() const;

		// Converts Uri to the string without credentials
		std::string ToStringWithCredentials() const;


		// Parses the given string to the Uri
		void Parse(const std::string &pUriString);


	private:

		Protocol m_protocol;			// Uri's protocol
		std::string m_protocolStr;		// Uri's protocol as a string

		Credentials m_credentials;		// Uri's credentials
		std::string m_credentialsStr;	// Uri's credentials as a string

		std::string m_host;				// Uri's host

		int m_port;						// Uri's port
		std::string m_portStr;			// Uri's port as a string

		std::string m_path;				// Uri's path
		std::string m_parameters;		// Uri's parameters
		std::string m_fragment;			// Uri's fragment


		// Parses the given string to the Uri
		void ParseFromString(const std::string &pUriString);


		// Copies the Uri's protocol to the protocol string
		void CopyProtocolToStr();

		// Copies the Uri's protocol from the protocol string
		void CopyProtocolFromStr();
		
		// Copies the Uri's credentials to the credentials string
		void CopyCredentialsToStr();

		// Copies the Uri's credentials from the credentials string
		void CopyCredentialsFromStr();

		// Copies the Uri's port to the port string
		void CopyPortToStr();

		// Copies the Uri's port from the port string
		void CopyPortFromStr();

	};

} // ns NetLib


#endif // NETLIB_URI_H
