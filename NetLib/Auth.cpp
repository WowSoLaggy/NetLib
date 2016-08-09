#include "Auth.h"

#include "Log.h"
#include "Config.h"


namespace NetLib
{

	NetErrCode Auth::UpdatePrivileges()
	{
		LOG("Auth::UpdatePrivileges()");

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

		return neterr_noErr;
	}

	bool Auth::Authorize()
	{
		return true;
	}

} // ns NetLib
