#ifndef CYREY_NETWORKING_HPP
#define CYREY_NETWORKING_HPP

#include <functional>
#include <future>
#include <map>
#include <string>
#include <string_view>

#ifdef WIN32
#include "raylib_win32.h"
#endif
#ifndef EMSCRIPTEN
#include "cpr/cpr.h"
#endif

namespace Cyrey
{
	struct CaseInsensitiveString
	{
		bool operator()(const std::string& a, const std::string& b) const noexcept {
			return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
				[](unsigned char ac, unsigned char bc) { return std::tolower(ac) < std::tolower(bc); });
		}
	};
	class Response
	{
	public:
		long mCode {};
#ifdef EMSCRIPTEN
		std::map<std::string, std::string, CaseInsensitiveString> mHeaders {};
#else
		cpr::Header mHeaders {};
#endif
		std::string mBody;

#ifndef EMSCRIPTEN
		// Response() = default;
		// Response(long statusCode, const cpr::Header& map, std::string body);
#endif
	};

	namespace Networking
	{
		std::future<Response> Get(const std::string& url,
		                                 const std::function<void(const Response&)>& callback = nullptr,
		                                 const std::function<void(const Response&)>& errorCallback = nullptr);
		std::future<Response> PostBuffer(const std::string& url,
		                                        const std::vector<std::uint8_t>& data,
		                                        const std::function<void(const Response&)>& callback = nullptr,
		                                        const std::function<void(const Response&)>& errorCallback = nullptr);

		void SetBaseUrl(std::string_view url);
		void SetTimeout(uint32_t timeout);
		void SetConnectTimeout(uint32_t timeout);
		constexpr std::string_view cDefaultBaseAddress =
			"https://cyrey-fmfgcufdg3czhjbf.germanywestcentral-01.azurewebsites.net/api/";
	};
} // namespace Cyrey

#endif //CYREY_NETWORKING_HPP
