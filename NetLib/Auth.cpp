#include "Auth.h"

#include "Log.h"
#include "Config.h"
#include "Utils.h"
#include "Base64.h"
#include "Md5.h"


namespace NetLib
{
	using namespace std::experimental::filesystem::v1;


	NetErrCode Auth::UpdatePrivileges()
	{
		LOG("Auth::UpdatePrivileges()");
		NetErrCode err;

		// Parse auth type

		std::string authTypeString = Config::GetAuthType();
		for (auto & authType : g_authTypesMap)
		{
			if (authType.first == auth_unknown)
			{
				// Skip the first element of the enumeration
				continue;
			}
			if (authType.first == auth_end)
			{
				// It is already the last element of the enumeration and
				// we still don't have any match
				echo("ERROR: Can't recognize the Authorization type: \"", authTypeString, "\".");
				return neterr_cantParseAuth;
			}

			if (authType.second.compare(authTypeString) == 0)
			{
				m_authType = authType.first;
				break;
			}
		}

		// Parse auth file

		err = ReadPrivilegesFromFile();
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't parse auth file.");
			return err;
		}

		return neterr_noErr;
	}


	NetErrCode Auth::CanAccess(const HttpConnectionInfo &pHttpConnectionInfo, const path &pResourceUri, AccessType pAccessType,
		std::string &pAuthorizationHeader)
	{
		path dir;
		pAuthorizationHeader = "";

		// Get parent path

		if (is_directory(pResourceUri))
			dir = pResourceUri;
		else if (is_regular_file(pResourceUri))
		{
			if (pResourceUri.has_filename() && (pResourceUri.has_parent_path()))
				dir = pResourceUri.parent_path();
			else
				return neterr_authError;
		}
		else if (pResourceUri.has_parent_path())
			dir = pResourceUri.parent_path();
		else
			return neterr_authError;

		// Check for files starting with the '.' symbol (including .htaccess files)

		if (
			(Config::GetForbidDot()) &&
			(is_regular_file(pResourceUri)) &&
			(pResourceUri.filename().string()[0] == '.')
			)
			return neterr_forbidden;
		if (
			(Config::GetForbidHtAccess()) &&
			(is_regular_file(pResourceUri)) &&
			(pResourceUri.filename().compare(".htaccess") == 0)
			)
			return neterr_forbidden;

		// Check if the dir is the server's root dir or one of it's subdirs

		if (
			!IsSameDir(dir, Config::GetRootFolder()) &&
			!IsSubDir(dir, Config::GetRootFolder())
			)
			return neterr_forbidden;

		// Check whether there is any auth
		
		if (m_authType == auth_none)
		{
			// If no auth, then the request should only be under the root dir, no other restrictions
			return neterr_noErr;
		}

		// Find neccessary .htaccess file

		std::string htaccessFilePath = FindHtAccessFile(dir);
		if (!CheckFileExists(htaccessFilePath))
		{
			// No .htaccess file found - no restrictions
			return neterr_noErr;
		}

		// Read privileges for this dir

		auto privileges = ReadHtAccessFile(htaccessFilePath);
		auto it = privileges.find(pAccessType);
		if (it == privileges.end())
		{
			// No restrictions for this access type
			return neterr_noErr;
		}

		std::string permittedRealm = it->second;
		if (permittedRealm.empty())
		{
			// Empty realm specified (no restrictions)
			return neterr_noErr;
		}

		// Authenticate current connection

		if (!Authorize(pHttpConnectionInfo.AuthorizationString, permittedRealm))
		{
			// Credentials are invalid
			pAuthorizationHeader = GetAuthorizationString(permittedRealm);
			return neterr_authRequired;
		}

		// Get realm for the current connection

		return neterr_noErr;
	}


	NetErrCode Auth::ReadPrivilegesFromFile()
	{
		m_realms.clear();
		m_users.clear();

		if (!CheckFileExists(Config::GetAuthPath()))
			return neterr_noErr;

		int mode = 0; // 1 - reading realms, 2 - reading users

		std::fstream f(Config::GetAuthPath(), std::ios::in);
		std::string line;
		std::vector<std::string> tokens;
		while (std::getline(f, line))
		{
			line = NetLib::TrimString(line);
			if (line[0] == ';')
				continue;

			if (line.compare(c_realmsOpenStr) == 0)
			{
				mode = 1;
				continue;
			}
			else if (line.compare(c_realmsCloseStr) == 0)
			{
				mode = 0;
				continue;
			}
			else if (line.compare(c_usersOpenStr) == 0)
			{
				mode = 2;
				continue;
			}
			else if (line.compare(c_usersCloseStr) == 0)
			{
				mode = 0;
				continue;
			}

			if (mode == 1)
				m_realms.emplace_back(line);
			else if (mode == 2)
			{
				tokens = NetLib::SplitString(line, ':');
				if (tokens.size() < 2)
					continue;

				std::vector<std::string> realms;
				if (tokens.size() > 2)
					realms = std::vector<std::string>(tokens.begin() + 2, tokens.end());

				m_users.push_back({ tokens[0], tokens[1], realms });
			}
		}

		return neterr_noErr;
	}

	std::map<AccessType, std::string> Auth::ReadHtAccessFile(const std::string &pHtAccessFilePath)
	{
		std::map<AccessType, std::string> res;

		if (!CheckFileExists(pHtAccessFilePath))
			return res;

		std::fstream f(pHtAccessFilePath, std::ios::in);
		std::string line;
		std::vector<std::string> tokens;
		while (std::getline(f, line))
		{
			line = NetLib::TrimString(line);
			if (line[0] == ';')
				continue;

			tokens = NetLib::SplitString(line, '=');
			if (tokens.size() != 2)
				continue;

			// Access type

			AccessType accessType;
			tokens[0] = TrimString(tokens[0]);
			for (auto & access : g_accessTypesMap)
			{
				if (access.first == access_unknown)
				{
					// Skip the first element of the enumeration
					continue;
				}
				if (access.first == access_end)
				{
					// It is already the last element of the enumeration and
					// we still don't have any match. Ignore this line
					break;
				}

				if (tokens[0].compare(access.second) == 0)
				{
					accessType = access.first;
					break;
				}
			}
			if (accessType == access_end)
				continue;

			tokens[1] = TrimString(tokens[1]);

			res.insert({ accessType, tokens[1] });
		}

		return res;
	}

	std::string Auth::FindHtAccessFile(const path &pDir)
	{
		std::string htaccessFilePath = Config::GetDefaultHtAccess();

		path curDir = pDir;
		std::string curHtAccessFile = curDir.string() + "/.htaccess";
		if (CheckFileExists(curHtAccessFile))
			htaccessFilePath = curHtAccessFile;
		while (!IsSameDir(Config::GetRootFolder(), curDir))
		{
			if (CheckFileExists(curHtAccessFile))
			{
				htaccessFilePath = curHtAccessFile;
				break;
			}
			if (!curDir.has_parent_path())
				break;
			curDir = curDir.parent_path();
			curHtAccessFile = curDir.string() + "/.htaccess";
		}

		return htaccessFilePath;
	}

	std::string Auth::GetAuthorizationString(const std::string &pRealm)
	{
		if (m_authType == auth_none)
			return "";
		else if (m_authType == auth_basic)
			return "Basic realm=\"" + pRealm + "\"";
		else if (m_authType == auth_digest)
		{
			// TODO:
			return "";
		}
		else
			return "";
	}

	bool Auth::Authorize(const std::string &pAuthString, const std::string &pRequiredRealm)
	{
		if (m_authType == auth_none)
			return true;
		else if (m_authType == auth_basic)
			return AuthorizeBasic(pAuthString, pRequiredRealm);
		else if (m_authType == auth_digest)
			return AuthorizeDigest(pAuthString, pRequiredRealm);

		return false;
	}

	bool Auth::AuthorizeBasic(const std::string &pAuthString, const std::string &pRequiredRealm)
	{
		// Remove leading "Basic "
		if (pAuthString.size() < 6)
			return false;

		std::string authString = std::string(pAuthString.begin() + 6, pAuthString.end());
		authString = Base64::Base64_Decode(authString);

		std::vector<std::string> tokens = SplitString(authString, ':');
		if (tokens.size() != 2)
			return false;

		if (tokens[0].empty() || tokens[1].empty())
			return false;

		auto it = std::find_if(m_users.begin(), m_users.end(), [&](auto & user) { return user.Name.compare(tokens[0]) == 0; });
		if (it == m_users.end())
			return false;

		if (it->Password.compare(Md5::Md5_Encode(std::vector<uint8_t>(tokens[1].begin(), tokens[1].end()))) != 0)
			return false;

		return std::find_if(it->Realms.begin(), it->Realms.end(), [&](auto & realm) { return realm.compare(pRequiredRealm) == 0; }) != it->Realms.end();
	}

	bool Auth::AuthorizeDigest(const std::string &pAuthString, const std::string &pRequiredRealm)
	{
		return false;
	}

} // ns NetLib
