#pragma once

#include "Utils.h"
#include "Log.h"

namespace Events
{
	void log_event(const RE::TESCombatEvent* a_event);
	void init_actor(RE::TESObjectREFR* actor);

	class CombatManager :
		public RE::BSTEventSink<RE::TESCombatEvent>
	{
	public:
		static CombatManager* GetSingleton()
		{
			static CombatManager singleton;
			return std::addressof(singleton);
		}

		static void Register()
		{
			auto scripts = RE::ScriptEventSourceHolder::GetSingleton();
			if (scripts) {
				scripts->AddEventSink(GetSingleton());
			}
		}

	protected:
		using EventResult = RE::BSEventNotifyControl;

		EventResult ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*) override
		{
			if (!a_event) {
				return EventResult::kContinue;
			}
			if (!a_event->actor.get()) {
				return EventResult::kContinue;
			}

#ifdef DEBUG_EVENTS
			log_event(a_event);
#endif	// DEBUG_EVENTS

			init_actor(a_event->actor.get());
			return EventResult::kContinue;
		}

	private:
		CombatManager() = default;
		CombatManager(const CombatManager&) = delete;
		CombatManager(CombatManager&&) = delete;

		~CombatManager() = default;

		CombatManager& operator=(const CombatManager&) = delete;
		CombatManager& operator=(CombatManager&&) = delete;
	};

	inline void Register()
	{
		CombatManager::Register();
	}
}