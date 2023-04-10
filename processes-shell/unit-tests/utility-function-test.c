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

void test_separate_parallel_commands(void) {
    char input[] = "cd & ls&pwd";
    size_t num_items;
    char **cmds = separate_parallel_commands(input, &num_items);
    TEST_ASSERT_EQUAL_STRING("cd", cmds[0]);
    TEST_ASSERT_EQUAL_STRING("ls", cmds[1]);
    TEST_ASSERT_EQUAL_STRING("pwd", cmds[2]);
    TEST_ASSERT_EQUAL_INT(3, num_items);

}

void test_parse_each_parallel_command(void) {
    char *input[] = {
        "cd",
        "pwd",
        "ls -al",
        "some_command -a -b --flag"
    };
    size_t num_item = 4;
    char *** result = parse_each_parallel_command(input, num_item);
    TEST_ASSERT_EQUAL_STRING("cd", result[0][0]);
    TEST_ASSERT_EQUAL_STRING("pwd", result[1][0]);
    TEST_ASSERT_EQUAL_STRING("ls", result[2][0]);
    TEST_ASSERT_EQUAL_STRING("-al", result[2][1]);
    TEST_ASSERT_EQUAL_STRING("some_command", result[3][0]);
    TEST_ASSERT_EQUAL_STRING("-a", result[3][1]);
    TEST_ASSERT_EQUAL_STRING("-b", result[3][2]);
    TEST_ASSERT_EQUAL_STRING("--flag", result[3][3]);
    TEST_ASSERT_EQUAL(result[4], NULL);
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
    RUN_TEST(test_separate_parallel_commands);
    RUN_TEST(test_parse_each_parallel_command);

    return UNITY_END(); // Close the Unity testing framework and return the test results
}
