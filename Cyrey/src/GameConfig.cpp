#include "GameConfig.hpp"

std::optional<Cyrey::GameConfig> Cyrey::GameConfig::ParseConfig(const std::string& cfg)
{
	try
	{
		return nlohmann::json::parse(cfg);
	}
	catch (nlohmann::json::exception&)
	{
		return std::nullopt;
	}
}
