#ifndef TEST_COMMON_H
#define TEST_COMMON_H
#include <assert.h>
#define assertm(exp, msg) assert(((void)msg, exp))

void call_test_setup_teardown(
    void (*test_function)(), void (*setup)(),
    void (*teardown)()
);

void call_test_teardown(
    void (*test_function)(), void (*teardown)()
);

void call_test_setup(
    void (*test_function)(), void (*setup)()
);

void call_test(
    void (*test_function)()
);

#endif