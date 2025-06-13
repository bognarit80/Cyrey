#include "Networking.hpp"

#ifdef EMSCRIPTEN
#include <emscripten/fetch.h>
#else
#include "cpr/cpr.h"
#endif

namespace
{
#ifdef EMSCRIPTEN
	auto parseHeaders(emscripten_fetch_t* fetch)
	{
		size_t bufferSize = emscripten_fetch_get_response_headers_length(fetch);
		std::string buffer(bufferSize + 1, '\0');
		emscripten_fetch_get_response_headers(fetch, buffer.data(), bufferSize + 1);
		// fill headers
		std::map<std::string, std::string, Cyrey::CaseInsensitiveString> headers;
		size_t pos = 0;
		while (pos < bufferSize)
		{
			size_t nextPos = buffer.find_first_of("\r\n", pos);
			std::string_view line {buffer.data() + pos, nextPos - pos};
			size_t subPos = line.find_first_of(':');
			headers.emplace(std::string { line.substr(0, subPos) },
							std::string { line.substr(subPos + 1, line.size() - (subPos + 1)) });
			pos = nextPos == std::string_view::npos ? buffer.size() : nextPos + 1;
		}
		return headers;
	}
#else
	cpr::Session session;
#endif
	std::string base { Cyrey::Networking::cDefaultBaseAddress };
	uint32_t connectTimeout { 10000 };
	uint32_t timeout { 5000 };
} // namespace

std::future<Cyrey::Response> Cyrey::Networking::Get(const std::string& url,
                                                    const std::function<void(const Response&)>& callback,
                                                    const std::function<void(const Response&)>& errorCallback)
{
	return std::async(std::launch::async, [url, callback]
	{
#ifdef EMSCRIPTEN
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		std::strcpy(attr.requestMethod, "GET");
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
		attr.timeoutMSecs = connectTimeout;
		auto fetch = emscripten_fetch(&attr, (base + url).c_str());

		Response r { fetch->status, parseHeaders(fetch), std::string(fetch->data, fetch->numBytes) };
		if (callback)
			callback(r);
		emscripten_fetch_close(fetch);
		return r;
#else
		session.SetUrl(base + url);
		session.SetTimeout(timeout);
		session.SetConnectTimeout(connectTimeout);
		auto r = session.Get();
		auto resp = Response { r.status_code, r.header, r.text };
		if (callback)
		    callback(resp);
		return resp;
#endif
	});
}

std::future<Cyrey::Response> Cyrey::Networking::PostBuffer(const std::string& url,
                                                           const std::vector<std::uint8_t>& data,
                                                           const std::function<void(const Response&)>& callback,
                                                           const std::function<void(const Response&)>& errorCallback)
{
	return std::async(std::launch::async, [url, data, callback]
	{
#ifdef EMSCRIPTEN
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		std::strcpy(attr.requestMethod, "POST");
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
		attr.timeoutMSecs = connectTimeout;
		attr.requestData = reinterpret_cast<const char*>(data.data());
		attr.requestDataSize = data.size();
		const char* arr[] = { "Content-Type", "application/octet-stream", nullptr };
		attr.requestHeaders = arr;
		auto fetch = emscripten_fetch(&attr, (base + url).c_str());

		Response r {fetch->status, parseHeaders(fetch), std::string(fetch->data, fetch->numBytes)};
		if (callback)
			callback(r);
		emscripten_fetch_close(fetch);
		return r;
#else
		session.SetUrl(base + url);
		session.SetHeader( { { "Content-Type", "application/octet-stream" } });
		session.SetBody(cpr::Buffer { data.begin(), data.end(), "" });
		session.SetTimeout(timeout);
		session.SetConnectTimeout(connectTimeout);

		auto r = session.Post();
		auto resp = Response { r.status_code, r.header, r.text };
		if (callback)
			callback(resp);
		return resp;
#endif
	});
}

void Cyrey::Networking::SetBaseUrl(std::string_view url)
{
	base = url;
}

void Cyrey::Networking::SetTimeout(uint32_t timeoutMs)
{
	timeout = timeoutMs;
}

void Cyrey::Networking::SetConnectTimeout(uint32_t timeoutMs)
{
	connectTimeout = timeoutMs;
}
