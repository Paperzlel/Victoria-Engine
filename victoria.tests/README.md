# victoria.tests
This is a binary executable that contains all unit tests for the Victoria Engine. Currently, the tests are a work-in-progress as not every template is represented and any contributions would be welcome (see below for the tests paradigm).

Generally, this binary is not included in the release builds of the engine as it's an optional final executable. Its purpose is to provide us programmers with useful insights into how the engine performs and whether modifications to a class have unexpected results. When modifying a class in any way, it's usually a good idea to run this just before running the engine entirely, as the lab test can ensure typical conditions have been checked for.

## Layout
*NOTE: Only core is tested at the moment, so the layout could change in future.*

Each test is located at the filepath it can be found at within the respective library's path (e.g. `Variant` tests are found in `core/variant/test_variant.cpp`). The name of the test file must be `test_<filename>.cpp` and must include a corresponding header, which is included in `test_manager.h`.

## Test paradigm
Tests should be written in two ways: pedantic (all possible use cases one could think of covered) and exhaustive (all externally-callable class methods must be covered). It should be noted that internal functions (e.g. `_size_changed` in `UIObject`) are not intended to be called by somebody else and therefore are exempt from testing. Additionally, basic getter and setter methods are exempt from testing, since their implementation is moreso dependent on how their class reacts to copy-from or reference operations than their written result.

With this clarified, how a test file is structured is largely dependent on the type itself. Generally, it show follow the below advice:
- Begin with the most basic construction and destruction. Check for the expected starting conditions and don't call the destructor manually - it's implied by the compiler in pretty much every case.
- Always check for the conditions that are expected to change, but never test every possible changed value: some of these changed values are guaranteed based on previous tests to be true and therefore any evaluation is wasted time.
- Never try to test more than one function in a file, unless that function has already been tested beforehand and proves to be reliable. (a TODO: for developers - tests ought to be rewritten to be more modular, so a test failure results in us skipping the rest of the test features)
- You may have more complex use-cases once their requirements have been proven to run. These should generally represent common use-cases of a class (e.g. checking for if a class returns a valid reference count when returning from a stack function) that may wind up in multiple places across a codebase.
- Each test function may have a comment above it explaining what situation the test is looking to check for. This is not a requirement and is mostly useful for tests where the checks seem obscure without the information.
