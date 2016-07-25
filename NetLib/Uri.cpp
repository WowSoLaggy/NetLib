#include "Uri.h"

#include "Utils.h"


namespace NetLib
{

	Uri::Uri()
		: Uri("", "", "", "", "", "", "")
	{
	}

	Uri::Uri(
		const std::string &pProtocol, const std::string &pCredentials,
		const std::string &pHost, const std::string &pPort, const std::string &pPath,
		const std::string &pParameters, const std::string &pFragment)
		: m_protocolStr(pProtocol), m_credentialsStr(pCredentials), m_host(pHost), m_portStr(pPort), m_path(pPath), m_parameters(pParameters), m_fragment(pFragment)
	{
		CopyProtocolFromStr();
		CopyCredentialsFromStr();
		CopyPortFromStr();
	}

	Uri::Uri(const std::string &pUriString)
	{
		ParseFromString(pUriString);
	}


	Uri::~Uri()
	{
	}


	std::string Uri::ToString() const
	{
		std::string str = "";
		if (!m_protocolStr.empty())
			str.append(m_protocolStr).append("://");
		/*if (!m_credentialsStr.empty())
			str.append(m_credentialsStr).append("@");*/
		str.append(m_host);
		if (!m_portStr.empty())
			str.append(":").append(m_portStr);
		str.append(m_path);
		if (!m_parameters.empty())
			str.append("?").append(m_parameters);
		if (!m_fragment.empty())
			str.append("#").append(m_fragment);

		return str;
	}

	std::string Uri::ToStringWithCredentials() const
	{
		std::string str = "";
		str.append(m_protocolStr).append("://");
		if (!m_credentialsStr.empty())
			str.append(m_credentialsStr).append("@");
		str.append(m_host);
		if (!m_portStr.empty())
			str.append(":").append(m_portStr);
		str.append(m_path);
		if (!m_fragment.empty())
			str.append("#").append(m_fragment);
		if (!m_parameters.empty())
			str.append("?").append(m_parameters);

		return str;
	}


	void Uri::Parse(const std::string &pUriString)
	{
		ParseFromString(pUriString);
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

	void Uri::CopyPortToStr()
	{
		m_portStr = (m_port == 0) ? "" : std::to_string(m_port);
	}

	void Uri::CopyPortFromStr()
	{
		m_port = (m_portStr.empty()) ? 0 : std::stoi(m_portStr);
	}

} // ns NetLib
