#include "REL/Relocation.h"

#include "Log.h"
#include "Hooks.h"
#include "Events.h"
#include "CharacterHandler.h"

#include <ShObjIdl.h>

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef DEBUG_ON
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= "MyFirstPlugin.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef DEBUG_ON
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	//logger::info("ImTired v1.0.0");

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "ImTired";
	a_info->version = 1;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

/*void ApplyNPCRecords()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
			if (actorbase && !actorbase->IsPlayer()) {
				//logger::info("NPC {} loaded, his name: ", uint64_t(actorbase));
			} else {
				//logger::info("Player {} loaded, his name: ", uint64_t(actorbase));
			}
		}
	}
}*/

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		// ApplyNPCRecords();
		Events::Register();
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", OnInit)) {
		logger::critical("Failed to register messaging listener!\n");
		return false;
	}
	Hooks::apply_hooks(std::uintptr_t(CharHandler::is_strong));

	return true;
}
