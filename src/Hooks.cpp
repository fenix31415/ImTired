#include "Hooks.h"

namespace Hooks
{
	void apply_melee(std::uintptr_t is_strong)
	{
		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr)
			{
				Xbyak::Label still_strong;

				// rcx = rbx = actor
				push(rdx);
				push(r8);
				push(rbx);
				sub(rsp, 48);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 48);
				pop(rbx);
				pop(r8);
				pop(rdx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm6, xmm6);

				L(still_strong);
				mov(rcx, rdx);
				mov(rsi, r8);
				ret();
			}
		};
		constexpr REL::ID funcOffset(49748);
		Code xbyakCode{ is_strong };

		auto& trampoline = SKSE::GetTrampoline();
		auto result = trampoline.allocate(xbyakCode.getSize());
		std::memcpy(result, xbyakCode.getCode(), xbyakCode.getSize());
		trampoline.write_call<6>(funcOffset.address() + 0x1D, (std::uintptr_t)result);
	}

	template <size_t BRANCH_TYPE, uint64_t ID, size_t offset = 0>
	void add_trampoline(Xbyak::CodeGenerator* xbyakCode)
	{
		constexpr REL::ID funcOffset = REL::ID(ID);
		auto funcAddr = funcOffset.address();
		auto size = xbyakCode->getSize();
		auto& trampoline = SKSE::GetTrampoline();
		auto result = trampoline.allocate(size);
		std::memcpy(result, xbyakCode->getCode(), size);
		trampoline.write_branch<BRANCH_TYPE>(funcAddr + offset, (std::uintptr_t)result);
	}

	void apply_another_get_chance(std::uintptr_t is_strong)
	{
		const int ID = 49747, BRANCH_TYPE = 5;
		constexpr REL::ID funcOffset = REL::ID(ID);
		auto funcAddr = funcOffset.address();

		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr, uintptr_t ret_addr)
			{
				Xbyak::Label still_strong;

				push(rcx);
				push(rdx);
				sub(rsp, 40);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 40);
				pop(rdx);
				pop(rcx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm0, xmm0);
				ret();

				L(still_strong);
				mov(ptr[rsp + 8], rbx);  // recover
				mov(rax, ret_addr);
				jmp(rax);
			}
		} xbyakCode{ is_strong, funcAddr + BRANCH_TYPE };
		add_trampoline<BRANCH_TYPE, ID>(static_cast<Xbyak::CodeGenerator*>(&xbyakCode));
	}

	void apply_get_CombatSpecialAttackChance(std::uintptr_t is_strong)
	{
		const int ID = 49749, BRANCH_TYPE = 5;
		constexpr REL::ID funcOffset(ID);
		auto funcAddr = funcOffset.address();
		constexpr REL::ID subOffset(37586);
		auto subAddr = subOffset.address();

		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr, uintptr_t ret_addr, uintptr_t sub_addr)
			{
				Xbyak::Label still_strong;

				push(rcx);
				push(rdx);
				sub(rsp, 48);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 48);
				pop(rdx);
				pop(rcx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm0, xmm0);
				add(rsp, 0x28);  // recover
				ret();

				L(still_strong);
				mov(rax, ret_addr);
				push(rax);
				mov(rax, sub_addr);
				jmp(rax);
			}
		} xbyakCode{ is_strong, funcAddr + 0x4 + BRANCH_TYPE, subAddr };
		add_trampoline<BRANCH_TYPE, ID, 0x4>(static_cast<Xbyak::CodeGenerator*>(&xbyakCode));
	}

	void apply_get_CombatRangedAttackChance(std::uintptr_t is_strong)
	{
		const int ID = 49772, BRANCH_TYPE = 6;
		constexpr REL::ID funcOffset = REL::ID(ID);
		auto funcAddr = funcOffset.address();

		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr, uintptr_t ret_addr)
			{
				Xbyak::Label still_strong;

				push(rcx);
				push(rdx);
				sub(rsp, 40);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 40);
				pop(rdx);
				pop(rcx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm0, xmm0);
				ret();

				L(still_strong);
				push(rbx);       // recover
				sub(rsp, 0x40);  // recover
				mov(rax, ret_addr);
				jmp(rax);
			}
		} xbyakCode{ is_strong, funcAddr + BRANCH_TYPE };
		add_trampoline<BRANCH_TYPE, ID>(static_cast<Xbyak::CodeGenerator*>(&xbyakCode));
	}

	void apply_get_CombatBashChance(std::uintptr_t is_strong)
	{
		const int ID = 49755, BRANCH_TYPE = 5;
		constexpr REL::ID funcOffset = REL::ID(ID);
		auto funcAddr = funcOffset.address();

		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr, uintptr_t ret_addr)
			{
				Xbyak::Label still_strong;

				push(rcx);
				push(rdx);
				sub(rsp, 40);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 40);
				pop(rdx);
				pop(rcx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm0, xmm0);
				ret();

				L(still_strong);
				mov(ptr[rsp + 8], rbx);	 // recover
				mov(rax, ret_addr);
				jmp(rax);
			}
		} xbyakCode{ is_strong, funcAddr + BRANCH_TYPE };
		add_trampoline<BRANCH_TYPE, ID>(static_cast<Xbyak::CodeGenerator*>(&xbyakCode));
	}

	void apply_get_CombatBlockChance(std::uintptr_t is_strong)
	{
		const int ID = 49751, BRANCH_TYPE = 6;
		constexpr REL::ID funcOffset = REL::ID(ID);
		auto funcAddr = funcOffset.address();

		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr, uintptr_t ret_addr)
			{
				Xbyak::Label still_strong;

				push(rcx);
				push(rdx);
				sub(rsp, 40);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 40);
				pop(rdx);
				pop(rcx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm0, xmm0);
				ret();

				L(still_strong);
				push(rbx);  // recover
				sub(rsp, 0x30);  // recover
				mov(rax, ret_addr);
				jmp(rax);
			}
		} xbyakCode{ is_strong, funcAddr + BRANCH_TYPE };
		add_trampoline<BRANCH_TYPE, ID>(static_cast<Xbyak::CodeGenerator*>(&xbyakCode));
	}

	void apply_get_OffensiveBashChance(std::uintptr_t is_strong)
	{
		const int ID = 49756, BRANCH_TYPE = 5;
		constexpr REL::ID funcOffset = REL::ID(ID);
		auto funcAddr = funcOffset.address();

		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr, uintptr_t ret_addr)
			{
				Xbyak::Label still_strong;

				push(rcx);
				push(rdx);
				sub(rsp, 40);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 40);
				pop(rdx);
				pop(rcx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm0, xmm0);
				ret();

				L(still_strong);
				mov(ptr[rsp + 8], rbx);	 // recover
				mov(rax, ret_addr);
				jmp(rax);
			}
		} xbyakCode{ is_strong, funcAddr + BRANCH_TYPE };
		add_trampoline<BRANCH_TYPE, ID>(static_cast<Xbyak::CodeGenerator*>(&xbyakCode));
	}

	void apply_get_CombatBlockAttackChance(std::uintptr_t is_strong)
	{
		const int ID = 49750, BRANCH_TYPE = 5;
		constexpr REL::ID funcOffset(ID);
		auto funcAddr = funcOffset.address();
		constexpr REL::ID subOffset(49711);
		auto subAddr = subOffset.address();

		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t is_strong_addr, uintptr_t ret_addr, uintptr_t sub_addr)
			{
				Xbyak::Label still_strong;

				push(rcx);
				push(rdx);
				sub(rsp, 48);
				mov(rax, is_strong_addr);
				call(rax);
				add(rsp, 48);
				pop(rdx);
				pop(rcx);

				test(al, al);
				jnz(still_strong);
				xorps(xmm0, xmm0);
				add(rsp, 0x28);	 // recover
				ret();

				L(still_strong);
				mov(rax, ret_addr);
				push(rax);
				mov(rax, sub_addr);
				jmp(rax);
			}
		} xbyakCode{ is_strong, funcAddr + 0x4 + BRANCH_TYPE, subAddr };
		add_trampoline<BRANCH_TYPE, ID, 0x4>(static_cast<Xbyak::CodeGenerator*>(&xbyakCode));
	}

	void apply_hooks(std::uintptr_t is_strong)
	{
		SKSE::AllocTrampoline(1 << 10);
		apply_another_get_chance(is_strong);
		apply_get_CombatSpecialAttackChance(is_strong);
		apply_get_CombatRangedAttackChance(is_strong);
		apply_get_CombatBashChance(is_strong);
		apply_get_CombatBlockChance(is_strong);
		apply_get_OffensiveBashChance(is_strong);
		apply_get_CombatBlockAttackChance(is_strong);
	}
}