#include "Detours.h"
#include <windows.h>
#include <assert.h>

namespace Detours
{
	namespace Internals
	{
		const unsigned char opcode_jmp_near = 0xE9;
		const size_t szJumpNear = 0x05;

		struct _hook
		{
			void* lpFunc;
			void* lpTrampoline;
			size_t szData;

			_hook()
				: lpFunc(NULL),
				  lpTrampoline(NULL)
			{ }
		};

		_hook* _hooks;
		size_t _sz_hooks;
		size_t _pt_hooks;

		bool AddHook(void* lpFunc, void* lpTrampoline)
		{
			if(!_hooks)
			{
				_sz_hooks = 16;
				_pt_hooks = 0;
				_hooks = new _hook[_sz_hooks];
				if(!_hooks)
					return false;
			}

			if(_pt_hooks == _sz_hooks)
			{
				_hook* new_hooks = new _hook[_sz_hooks * 2];
				if(!new_hooks)
					return false;

				memcpy(new_hooks, _hooks, _sz_hooks * sizeof(_hook));
				_sz_hooks *= 2;
				delete [] _hooks;
				_hooks = new_hooks;
			}

			_hooks[_pt_hooks].lpFunc = lpFunc;
			_hooks[_pt_hooks].lpTrampoline = lpTrampoline;
			_pt_hooks += 1;

			return true;
		}

		#define FIND_BY_FUNCTION 1
		#define FIND_BY_TRAMPOLINE 2

		_hook* _FindHookByFuncAddr(void* lpFunc, size_t* szOutIndex, unsigned char ucFindType)
		{
			if(szOutIndex != NULL)
				*szOutIndex = static_cast<size_t>(-1);
			for(size_t i = 0; i < _pt_hooks; i++)
				if((ucFindType == FIND_BY_FUNCTION && _hooks[i].lpFunc == lpFunc)
				|| (ucFindType == FIND_BY_TRAMPOLINE && _hooks[i].lpTrampoline == lpFunc))
				{
					if(szOutIndex != NULL)
						*szOutIndex = i;
					return &_hooks[i];
				}

			return NULL;
		}

		void RemoveHook(void* lpFunc)
		{
			size_t current_hook_index = 0;
			_hook* current_hook = _FindHookByFuncAddr(lpFunc, &current_hook_index, FIND_BY_FUNCTION);

			if(!current_hook)
				return;

			errno_t err = memmove_s(current_hook, _pt_hooks - current_hook_index, 
				      				current_hook + 1, _sz_hooks - current_hook_index + 1);
			if(err != 0)
				return;

			_pt_hooks -= 1;
		}

		void GetTrampoline(void* lpFunc, void** lppTrampoline)
		{
			if(!lppTrampoline)
				return;
			*lppTrampoline = NULL;

			_hook* hook = _FindHookByFuncAddr(lpFunc, NULL, FIND_BY_FUNCTION);
			if(hook)
				*lppTrampoline = hook->lpTrampoline;
		}

		void SetHookSize(void* lpFunc, size_t szData)
		{
			_hook* hook = _FindHookByFuncAddr(lpFunc, NULL, FIND_BY_FUNCTION);
			if(hook)
				hook->szData = szData;
		}

		size_t GetHookSize(void* lpTrampoline)
		{
			_hook* hook = _FindHookByFuncAddr(lpTrampoline, NULL, FIND_BY_TRAMPOLINE);
			if(hook)
				return hook->szData;
			assert(false);
			return 0;
		}

		size_t _CalculateDispacement(void* lpFirst, void* lpSecond)
		{
			return reinterpret_cast<char*>(lpSecond) - (reinterpret_cast<char*>(lpFirst) + 5);
		}

		/* Analyses function code and returns the size in bytes which consists a whole instruction set - minimal greater 
		 * than 5 bytes for jump near (0xe9)
		 * If function body is less than 5 bytes or current instruction set prevents detour the return value is 0 */
		size_t _AnalyzeFunctionCode(void* lpFunc)
		{
			unsigned char prologue[] = { 0x8b, 0xff, 0x55, 0x8b, 0xec };
			if(!memcmp(prologue, lpFunc, sizeof(prologue)))
				return 5;
			return 0;
		}

		/* Creates a trampoline to the function lpFunc in virtual process memory by copying first szDispacement function
		 * data and set a relative jump to the skipped part at the end */
		void* _CreateTrampoline(void* lpFunc, size_t szDispacement)
		{
			void* lpPage = VirtualAlloc(NULL, szDispacement + szJumpNear, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if(!lpPage)
				return NULL;

			size_t szRelativeAddr = _CalculateDispacement(lpPage, lpFunc);
			try
			{
				errno_t err = memcpy_s(lpPage, szDispacement, lpFunc, szDispacement);
				if(err)
					throw err;
				err = memcpy_s(reinterpret_cast<char*>(lpPage) + szDispacement, sizeof(opcode_jmp_near), &opcode_jmp_near, sizeof(opcode_jmp_near));
				if(err)
					throw err;
				err = memcpy_s(reinterpret_cast<char*>(lpPage) + szDispacement + sizeof(opcode_jmp_near), sizeof(size_t), &szRelativeAddr, sizeof(size_t));
				if(err)
					throw err;
			}
			catch(errno_t)
			{
				VirtualFree(lpPage, szDispacement + szJumpNear, 0);
				return NULL;
			}

			SetHookSize(lpFunc, szDispacement + szJumpNear);
			return lpPage;
		}

		/* Restores original function data from the trampoline */
		void _RestoreFunction(void* lpFunc, void* lpTrampoline)
		{
			size_t szHook = GetHookSize(lpTrampoline) - szJumpNear;
			DWORD dwProtect = PAGE_READWRITE;

			VirtualProtect(lpFunc, szHook, dwProtect, &dwProtect);
			memcpy_s(lpFunc, szHook, lpTrampoline, szHook);
			VirtualProtect(lpFunc, szHook, dwProtect, &dwProtect);
		}

		/* Destroys a trampoline in virtual memory */
		void _DestroyTrampoline(void* lpTrampoline)
		{
			size_t szData = GetHookSize(lpTrampoline);
			VirtualFree(lpTrampoline, szData, 0);
		}
	
		/* Set a relative long jump to the start an lpFunc which targets to lpTarget */
		bool _SetLongJumpTo(void* lpFunc, void* lpTarget)
		{
			size_t szDispacement = _CalculateDispacement(lpFunc, lpTarget);
			DWORD dwProtect = PAGE_READWRITE;

			VirtualProtect(lpFunc, szJumpNear, dwProtect, &dwProtect);
			*reinterpret_cast<char*>(lpFunc) = 0xE9;
			errno_t err = memcpy_s(reinterpret_cast<char*>(lpFunc) + 1, sizeof(size_t), &szDispacement, sizeof(size_t));
			VirtualProtect(lpFunc, szJumpNear, dwProtect, &dwProtect);

			if(!err)
				return true;

			void* lpTrampoline = NULL;
			GetTrampoline(lpFunc, &lpTrampoline);
			if(lpTrampoline)
				_RestoreFunction(lpFunc, lpTrampoline);

			return false;
		}
	}

	DETOUR_RESULT HookFunction(void* lpFunc, void* lpDetour, void** lppOriginalFunctionTrampoline, int size)
	{
		assert(lpFunc);
		if(lppOriginalFunctionTrampoline == NULL)
			return ERROR_TRAMPOLINE_MISSING;

		size_t szCopiedFuncBody = size ? size : Internals::_AnalyzeFunctionCode(lpFunc);
		if(szCopiedFuncBody == 0)
			return ERROR_INVALID_FUNCTION_BODY;

		void* lpFunctionTrampoline = Internals::_CreateTrampoline(lpFunc, szCopiedFuncBody);
		if(!lpFunctionTrampoline)
			return ERROR_INSUFFICIENT_MEMORY;

		try
		{
			if(!Internals::AddHook(lpFunc, lpFunctionTrampoline))
				throw ERROR_INSUFFICIENT_MEMORY;

			if(!Internals::_SetLongJumpTo(lpFunc, lpDetour))
				throw ERROR_CODE_INJECTION_FAILED;
		}
		catch(DETOUR_RESULT result)
		{
			Internals::RemoveHook(lpFunc);
			Internals::_DestroyTrampoline(lpFunctionTrampoline);
			return result;
		}

		*lppOriginalFunctionTrampoline = lpFunctionTrampoline;
		return DETOURED_OK;
	}

	void UnhookFunction(void* lpFunc)
	{
		void* lpTrampoline;
		Internals::GetTrampoline(lpFunc, &lpTrampoline);
		if(!lpTrampoline)
			return;

		Internals::RemoveHook(lpFunc);
		Internals::_RestoreFunction(lpFunc, lpTrampoline);
	}
}