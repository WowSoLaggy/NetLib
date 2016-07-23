#pragma once

#ifndef NETLIB_URI_H
#define NETLIB_URI_H


#include <string>

#include "NetErrCodes.h"


namespace NetLib
{

	enum Protocol : int
	{
		prot_unknown = 0,

		prot_http,
		prot_ftp,
		prot_rtsp,

		prot_end
	};

	
	struct Credentials
	{
		Credentials()
			: UserName(""), Password("")
		{
		}

		Credentials(const std::string &pUserName, const std::string &pPassword)
			: UserName(pUserName), Password(pPassword)
		{
		}

		std::string UserName;
		std::string Password;
	};


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
			neterr_noErr;
		}


		void CopyProtocolToStr()
		{
			m_protocolStr = c_ProtocolStrings[m_protocol];
		}

		void CopyProtocolFromStr()
		{
			m_protocol = (Protocol)0;

			for (int i = 1; i < prot_end; ++i)
			{
				if (m_protocolStr.compare(c_ProtocolStrings[i]) == 0)
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

	private:

		// Global const data

		const std::vector<std::string> c_ProtocolStrings = {
			"",
			"http",
			"ftp",
			"rtsp",

			""
		};

	};

} // ns NetLib


#endif // NETLIB_URI_H
