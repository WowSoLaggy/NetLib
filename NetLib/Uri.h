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

		Uri()
			: Uri("", "", "", "", "", "")
		{
		}

		Uri(const std::string &pProtocol, const std::string &pCredentials,
			const std::string &pHost, const std::string &pPath,
			const std::string &pParameters, const std::string &pFragment)
			: m_protocolStr(pProtocol), m_credentialsStr(pCredentials), m_host(pHost), m_path(pPath), m_parameters(pParameters), m_fragment(pFragment)
		{
		}

		Uri(const std::string &pUriString)
		{
			ParseFromString(pUriString);
		}
		

		virtual ~Uri()
		{
		}



		const Protocol & GetProtocol()			const { return m_protocol; }
		const std::string & GetProtocolStr()	const { return m_protocolStr; }

		const Credentials & GetCredentials()	const { return m_credentials; }
		const std::string & GetCredentialsStr()	const { return m_credentialsStr; }

		const std::string & GetHostStr()		const { return m_host; }
		const std::string & GetPathStr()		const { return m_path; }
		const std::string & GetParametersStr()	const { return m_parameters; }
		const std::string & GetFragmentStr()	const { return m_fragment; }



		void Parse(const std::string &pUriString)
		{
			ParseFromString(pUriString);
		}


	private:

		Protocol m_protocol;
		std::string m_protocolStr;

		Credentials m_credentials;
		std::string m_credentialsStr;

		std::string m_host;
		std::string m_path;
		std::string m_parameters;
		std::string m_fragment;


		void ParseFromString(const std::string &pUriString)
		{
		}


		void CopyProtocolToStr()
		{
			m_protocolStr = g_ProtocolStrings[m_protocol];
		}

		void CopyProtocolFromStr()
		{
			m_protocol = (Protocol)0;

			for (int i = 1; i < prot_end; ++i)
			{
				if (m_protocolStr.compare(g_ProtocolStrings[i]) == 0)
				{
					m_protocol = (Protocol)i;
					return;
				}
			}
		}

		void CopyCredentialsToStr()
		{
			m_credentialsStr = m_credentials.UserName + ":" + m_credentials.Password;
		}

	};

} // ns NetLib


#endif // NETLIB_URI_H
