#include "Events.h"

namespace Events
{
	void init_actor(RE::TESObjectREFR* actor)
	{
		if (actor->formType.get() != RE::FormType::ActorCharacter)	 // handle only character events
			return;

		if (!is_human_race(static_cast<RE::Actor*>(actor)->GetRace()->GetFormEditorID())) {
			info("Got non-human {}, his race is {}", (uint64_t)actor, static_cast<RE::Actor*>(actor)->GetRace()->GetFormEditorID());
		} else {
			info("Got HUMAN {}, his race is {}", (uint64_t)actor, static_cast<RE::Actor*>(actor)->GetRace()->GetFormEditorID());
		}

		if (actor->pad94 != 0xFE314159) {
#ifdef DEBUG_EVENTS
			if (actor->pad94)
				logger::info("XAXA, was {}", actor->pad94);
#endif	// DEBUG_EVENTS
			actor->pad94 = 0xFE314159;
			actor->pad1C = 0;
			actor->pad1B = 0;
		}
	}

	void log_event(const RE::TESCombatEvent* a_event)
	{
		if (!a_event->actor.get()->GetPlayable())
			return;
		if (!a_event->targetActor.get()) {
			logger::info("TESCombatEvent {}! actor={}({}), target=empty", (uint64_t)a_event->newState.get(),
				a_event->actor.get()->GetName(), (uint64_t)a_event->actor.get());
			return;
		}
		logger::info("TESCombatEvent {}! actor={}({}), target={}({})", (uint64_t)a_event->newState.get(),
			a_event->actor.get()->GetName(), (uint64_t)a_event->actor.get(),
			a_event->targetActor.get()->GetName(), (uint64_t)a_event->targetActor.get());

	}
}
