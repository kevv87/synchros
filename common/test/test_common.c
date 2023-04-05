#include "test_common.h"

void call_test_setup_teardown(
    void (*test_function)(), void (*setup)(),
    void (*teardown)()
) {
    setup();
    test_function();
    teardown();
}

void call_test_teardown(
    void (*test_function)(), void (*teardown)()
) {
    test_function();
    teardown();
}

void call_test_setup(
    void (*test_function)(), void (*setup)()
) {
    setup();
    test_function();
}

void call_test(
    void (*test_function)()
) {
    test_function();
}
