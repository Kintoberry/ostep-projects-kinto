#define UNITY_FAIL_AND_BAIL
#include "wish-utilities.h"
#include "unity.h"
#include <string.h>


// void test_get_token_nums(void) {
//     char *arguments = "cp src1 src2 src3 dest";
//     char *delimter = " ";
//     size_t token_num = get_token_nums(arguments, delimter);
//     TEST_ASSERT_EQUAL_INT(6, token_num);

//     arguments = "ls -a";
//     token_num = get_token_nums(arguments, delimter);
//     TEST_ASSERT_EQUAL_INT(2, token_num);
// }

// void test_takeout_all_arguments(void) {
//     char *arguments = "cp src1 src2 src3 dest";
//     char *delimiter = " ";
//     char** tokens = takeout_all_arguments(arguments, delimiter);
//     TEST_ASSERT_EQUAL_STRING("cp", tokens[0]);
//     TEST_ASSERT_EQUAL_STRING("src1", tokens[1]);
//     TEST_ASSERT_EQUAL_STRING("src2", tokens[2]);
//     TEST_ASSERT_EQUAL_STRING("src3", tokens[3]);
//     TEST_ASSERT_EQUAL_STRING("dest", tokens[4]);

//     const char *expected_string_array[] = {"ls", "-a"};
//     tokens = takeout_all_arguments("ls -a", delimiter);
//     TEST_ASSERT_EQUAL_STRING_ARRAY(expected_string_array, tokens, 2);
    
// }

void test_strip(void) {
    char input[] = "   hello, world!   ";
    char *stripped_input = strip(input);
    TEST_ASSERT_EQUAL_STRING("hello, world!", stripped_input);
    TEST_ASSERT_EQUAL_INT(13, strlen(stripped_input));
}

void setUp(void) {
    // Empty function, no setup needed for tests
}

void tearDown(void) {
    // Empty function, no teardown needed for tests
}

int main(void) {
    UNITY_BEGIN(); // Initialize the Unity testing framework

    // RUN_TEST(test_get_token_nums); 
    // RUN_TEST(test_takeout_all_arguments);
    RUN_TEST(test_strip);

    return UNITY_END(); // Close the Unity testing framework and return the test results
}
