#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"


TEST(TrimSpace, TMessage, 5000)
{
	char res[32];
	TrimSpace("  asdf   ", res);

	ZSTR_EQUAL("Trim failed", res, "asdf");
}
