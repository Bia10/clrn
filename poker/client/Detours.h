#pragma once

namespace Detours
{
 	enum DETOUR_RESULT
	{
		DETOURED_OK,
		ERROR_TRAMPOLINE_MISSING,
		ERROR_INVALID_FUNCTION_BODY,
		ERROR_INSUFFICIENT_MEMORY,
		ERROR_UNEXPECTED_FAILURE,
		ERROR_CODE_INJECTION_FAILED
	};

	/* Set hook to function pointed with lpFunc, so all requests for it will be redirected to function pointed with lpDetour.
	 * To call original function should use functon pointer in lppOriginalFunctionTrampoline. 
	 * T is hooked function type. */
	DETOUR_RESULT HookFunction(void* lpFunc, void* lpDetour, void** lppOriginalFunctionTrampoline, int size = 0);

	/* Removes hook from the function pointed with lpFunc. This address is same that use in HookFunction
	 * T is Unhooked function type. */
	void UnhookFunction(void* lpFunc);
}