#include "test_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void assertm(int condition, char *message, ...) {
    if (!condition) {
        va_list args;
        va_start(args, message);
        vprintf(message, args);
        va_end(args);
        exit(1);
    }
}

void expect_equal(int expected, int actual, char *message, ...) {
    if (expected != actual) {
        va_list args;
        va_start(args, message);
        vprintf(message, args);
        va_end(args);
        printf("Expected: %d, Actual: %d\n" , expected, actual);
        exit(1);
    } else {
        printf("Ok.\n");
    }
}

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
