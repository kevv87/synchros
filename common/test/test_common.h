#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#define TEST_BUFFER_SIZE 10
#define TEST_FILE_LENGTH 20
#define PROJECT_ID 42
#define TEST_SHM_NAME "/test_mem"

void assertm(
    int condition, char *message, ...
);

void expect_equal(int expected, int actual, char *message, ...);

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
