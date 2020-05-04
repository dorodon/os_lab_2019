#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
    char simple_string[] = "Hello";
    char str_with_spaces[] = "String with spaces";
    char str_with_odd_chars_num[] = "abc";
    char str_with_even_chars_num[] = "abcd";

    RevertString(simple_string);
    assert(0 == strcmp("olleH", simple_string));

    RevertString(str_with_spaces);
    assert(0 == strcmp("secaps htiw gnirtS", str_with_spaces));

    RevertString(str_with_odd_chars_num);
    assert(0 == strcmp("cba", str_with_odd_chars_num));

    RevertString(str_with_even_chars_num);
    assert(0 == strcmp("dcba", str_with_even_chars_num));
    }
