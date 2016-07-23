#pragma once

#ifndef NETLIB_URI_H
#define NETLIB_URI_H


#include <string>

#include "NetErrCodes.h"
#include "Protocols.h"
#include "Credentials.h"


namespace NetLib
{

	class Uri
	{
	public:

		Uri();

		Uri(const std::string &pProtocol, const std::string &pCredentials,
			const std::string &pHost, const std::string &pPath,
			const std::string &pParameters, const std::string &pFragment);

		Uri(const std::string &pUriString);


		virtual ~Uri();



		const Protocol & GetProtocol()			const { return m_protocol; }
		const std::string & GetProtocolStr()	const { return m_protocolStr; }

		const Credentials & GetCredentials()	const { return m_credentials; }
		const std::string & GetCredentialsStr()	const { return m_credentialsStr; }

		const std::string & GetHostStr()		const { return m_host; }
		const std::string & GetPathStr()		const { return m_path; }
		const std::string & GetParametersStr()	const { return m_parameters; }
		const std::string & GetFragmentStr()	const { return m_fragment; }


		void Parse(const std::string &pUriString);


	private:

		Protocol m_protocol;
		std::string m_protocolStr;

		Credentials m_credentials;
		std::string m_credentialsStr;

		std::string m_host;
		std::string m_path;
		std::string m_parameters;
		std::string m_fragment;


		void ParseFromString(const std::string &pUriString);


		void CopyProtocolToStr();
		void CopyProtocolFromStr();
		void CopyCredentialsToStr();
		void CopyCredentialsFromStr();

	};

} // ns NetLib


#endif // NETLIB_URI_H
