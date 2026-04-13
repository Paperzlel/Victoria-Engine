#pragma once

typedef bool (*PFN_test)();

/**
 * @brief Registers a test to be ran. Adds it to the end of an array from which it can then be called in order,
 * allowing for test order to be assigned properly.
 * @param p_test The function pointer of the test to register.
 * @param p_desc A description of what the test does. Context is not preserved so the tested datatype should be
 * included in the description.
 */
void register_test(PFN_test p_test, const char *p_desc);
