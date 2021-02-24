#include "CharacterHandler.h"

namespace CharHandler
{
	bool is_init_char(RE::TESObjectREFR* actor)  // we just skip handling if false
	{
		if (actor->formType.get() != RE::FormType::ActorCharacter)
			return false;
		if (actor->pad94 != 0xFE314159) {
			info("is_init_char, Got new actor ({}), was = {}. Initialising", (uint64_t)actor, actor->pad94);
			actor->pad94 = 0xFE314159;
		}
		return true;
	}

	namespace DistanceManaging
	{
		constexpr REL::ID get_dist_ID(19396);
		REL::Relocation<float __fastcall(RE::TESObjectREFR* to, RE::TESObjectREFR* from, char a3, char is_someone_tmp)> get_dist_(get_dist_ID);

		enum class DistanceType
		{
			close,
			mid,
			tofar
		};

		DistanceType get_dist_type(RE::Actor* actor, RE::Actor* target)
		{
			float dist = get_dist_(actor, target, (char)1, (char)0);
			if (dist > 512.0f) {
				info("get_dist_type, ({}). Dist={}, so FAR", (uint64_t)actor, dist);
				return DistanceType::tofar;
			}
			if (dist > 150.0f) {
				info("get_dist_type, ({}). Dist={}, so MID", (uint64_t)actor, dist);
				return DistanceType::mid;
			}
			info("get_dist_type, ({}). Dist={}, so CLOSE", (uint64_t)actor, dist);
			return DistanceType::close;
		}
	}
	using DistanceType = DistanceManaging::DistanceType;

	RE::WEAPON_TYPE get_weapon_type(RE::Actor* actor)
	{
		if (check_null(actor->currentProcess, "!! get_tired_type, actor stupid. Assuming it's arms"))
			return RE::WEAPON_TYPE::kHandToHandMelee;
		if (check_null(actor->currentProcess->middleHigh, "!! get_tired_type, actor middle stupid. Assuming it's arms"))
			return RE::WEAPON_TYPE::kHandToHandMelee;
		if (check_null(actor->currentProcess->middleHigh->rightHand, "!! get_tired_type, actor hasnt right item. Assuming it's arms"))
			return RE::WEAPON_TYPE::kHandToHandMelee;

		const auto object = actor->currentProcess->middleHigh->rightHand->object;
		if (check_null(object, "!! get_tired_type, actor hasnt right object"))
			return RE::WEAPON_TYPE::kHandToHandMelee;
		info("get_tired_type, ({}) = {}", (uint64_t)actor, static_cast<RE::TESObjectWEAP*>(object)->weaponData.animationType.underlying());
		return static_cast<RE::TESObjectWEAP*>(object)->weaponData.animationType.get();
	}

	enum class TiredType
	{
		Err,
		Light,
		HeavyClose,
		HeavyFar,
		None
	};

	TiredType get_tired_type(RE::Actor* actor)
	{
		if (check_null(actor->currentCombatTarget, "!! get_tired_type, empty targetHandle"))
			return TiredType::Err;
		if (check_null(actor->currentCombatTarget.get(), "!! get_tired_type, empty targetPtr"))
			return TiredType::Err;
		RE::Actor* target = actor->currentCombatTarget.get().get();
		if (check_null(target, "!! get_tired_type, empty target"))
			return TiredType::Err;

		WeaponInfo::WeapType weap_type = WeaponInfo::get_weap_type(get_weapon_type(actor));
		DistanceType dist_type = DistanceManaging::get_dist_type(actor, target);
		if (dist_type == DistanceType::mid && weap_type == WeaponInfo::WeapType::Heavy) {
			info("get_tired_type, mid & Heavy ({}). so HeavyClose", (uint64_t)actor);
			return TiredType::HeavyClose;
		}
		if (dist_type == DistanceType::tofar) {
			info("get_tired_type, tofar({}). so HeavyFar|Light", (uint64_t)actor);
			return weap_type == WeaponInfo::WeapType::Heavy ? TiredType::HeavyFar : TiredType::Light;
		}
		info("get_tired_type, light & mid || close({}). so None", (uint64_t)actor);
		return TiredType::None;
	}

	void on_tired(RE::Actor* actor, TiredType type)
	{
		switch (type) {
		case TiredType::Light:
		case TiredType::HeavyClose:
			if (actor->NotifyAnimationGraph("mctStaminaRecovery1Start")) {
				auto wait_time = actor->currentProcess->high->exclusiveTimer;
				*(float*)((char*)actor + 0x1C) = wait_time;
				actor->pad1B = (uint8_t)type;
				info("on_tired, run 1 anim");
			} else {
				error("!! on_tired, cant play anim");
			}
			break;
		case TiredType::HeavyFar:
			if (actor->NotifyAnimationGraph("mctStaminaRecovery2Start")) {
				auto wait_time = actor->currentProcess->high->exclusiveTimer;
				*(float*)((char*)actor + 0x1C) = wait_time;
				actor->pad1B = (uint8_t)type;
				info("on_tired, run 2 anim");
			} else {
				error("!! on_tired, cant play anim");
			}
			break;
		case TiredType::None:
			info("on_tired, stand! atamanom budesh");
			break;
		default:
			error("!! on_tired, unknown TiredType");
			break;
		}
	}

	void on_relaxed(RE::Actor* actor, float cost)
	{
		actor->pad1C = 0;
		actor->pad1B = 0;
		actor->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, cost);
		info("on_relaxed, ({}), adding {} stamina", (uint64_t)actor, cost);
	}

	float ieee_float(uint32_t f)
	{
		static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
		float ret;
		std::memcpy(&ret, &f, sizeof(float));
		return ret;
	}

	constexpr float get_anim_duration(TiredType type) {
		switch (type) {
		case CharHandler::TiredType::HeavyFar:
			return 5.0f;
		case CharHandler::TiredType::Light:
		case CharHandler::TiredType::HeavyClose:
			return 3.0f;
		default:
			error("!! get_anim_duration unexpected TiredType");
			return 0.0f;
		}
	}

	bool is_relaxing(RE::Actor* actor, float cost)
	{
		auto high = actor->currentProcess->high;
		auto mem = actor->pad1C;
		if (!mem) {
			info("is_relaxing, NOrelaxing. Exiting...");
			return false;
		}
		float start_time = ieee_float(mem);
		if (start_time == 0.0f) {
			error("!! is_relaxing, pad1C=0.0f. Exiting...");
			return false;
		}
		if (isnan(start_time)) {
			error("!! is_relaxing, isnan. Exiting...");
			return false;
		}
		float cur_time = high->exclusiveTimer;
		float delta = abs(start_time - cur_time);
		info("is_relaxing, start_time={}, cur_time={}, delta={}", start_time, cur_time, delta);
		if (delta > 10.0f) {
			error("!! is_relaxing, toobig delta");
			return false;
		}

		TiredType tired_type = (TiredType)actor->pad1B;
		float duration = get_anim_duration(tired_type);
		if (delta > duration) {
			on_relaxed(actor, cost);
			info("on_relaxed");
			return true;
		}
		return true;
	}

	float get_cost(RE::Actor* actor)
	{
		float cost = 20;
		auto item = actor->GetEquippedEntryData(false);
		if (item) {
			cost = WeaponInfo::weap_cost(get_weapon_type(actor));
		} else {
			error("get_cost, ({}) hasnt item", (uint64_t)actor);
		}
		return cost;
	}

	bool is_strong(RE::Actor* actor)
	{
		if (!actor || !actor->IsInCombat() || !is_init_char(actor))
			return true;

		info("================================================================");
		info("is_strong start: ({})", (uint64_t)actor);

		auto cur_stamina = actor->GetActorValue(RE::ActorValue::kStamina);

		float cost = 100.0f;
		cost = get_cost(actor);

		if (is_relaxing(actor, cost))
			return false;

		if (cur_stamina >= cost) {
			info("is_strong=TRUE ({}), {}>={}", (uint64_t)actor, cur_stamina, cost);
			return true;
		} else {
			info("is_strong=FALSE ({}), {}<{}", (uint64_t)actor, cur_stamina, cost);
			if (is_human_race(actor->GetRace()->GetFormEditorID()))
				on_tired(actor, get_tired_type(actor));
			return false;
		}
	}
}