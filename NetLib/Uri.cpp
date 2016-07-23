#include "Uri.h"

#include "Utils.h"


namespace NetLib
{

	Uri::Uri()
		: Uri("", "", "", "", "", "")
	{
	}

	Uri::Uri(
		const std::string &pProtocol, const std::string &pCredentials,
		const std::string &pHost, const std::string &pPath,
		const std::string &pParameters, const std::string &pFragment)
		: m_protocolStr(pProtocol), m_credentialsStr(pCredentials), m_host(pHost), m_path(pPath), m_parameters(pParameters), m_fragment(pFragment)
	{
		CopyProtocolFromStr();
		CopyCredentialsFromStr();
	}

	Uri::Uri(const std::string &pUriString)
	{
		ParseFromString(pUriString);
	}


	Uri::~Uri()
	{
	}


	void Uri::Parse(const std::string &pUriString)
	{
		ParseFromString(pUriString);
	}



	void Uri::ParseFromString(const std::string &pUriString)
	{
	}


	void Uri::CopyProtocolToStr()
	{
		m_protocolStr = g_ProtocolStrings[m_protocol];
	}

	void Uri::CopyProtocolFromStr()
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

	void Uri::CopyCredentialsToStr()
	{
		if ((!m_credentials.UserName.empty()) || (!m_credentials.Password.empty()))
			m_credentialsStr = m_credentials.UserName + ":" + m_credentials.Password;
		else
			m_credentialsStr = "";
	}

	void Uri::CopyCredentialsFromStr()
	{
		auto tokens = SplitString(m_credentialsStr, ':');

		m_credentials.UserName = (tokens.size() > 0) ? tokens[0] : "";
		m_credentials.Password = (tokens.size() > 1) ? tokens[1] : "";
	}

} // ns NetLib
