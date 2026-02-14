#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "json.h"
#include "log.h"

static int is_key = 0;

int is_object_begin(const char ch)
{
    return '{' == ch;
}

int is_object_end(const char ch)
{
    return '}' == ch;
}

int is_float(const char ch)
{
    return ('+' == ch || 
            '-' == ch || 
            'e' == ch || 
            'E' == ch || 
            '.' == ch);
}

int is_number(const char ch)
{
    return (0x30 <= ch && 0x39 >= ch); 
}

int is_list_begin(const char ch)
{
    return '[' == ch;
}

int is_list_end(const char ch)
{
    return ']' == ch;
}

int is_separator(const char ch)
{
    return ':' == ch;
}

int is_quote(const char ch)
{
    return '"' == ch;
}

int is_space(const char ch)
{
    return ' ' == ch;
}

int is_comma(const char ch)
{
    return ',' == ch;
}

int is_true(const char* json)
{
    return 0 == strncmp(json, "true", 4);
}

int is_false(const char* json)
{
    return 0 == strncmp(json, "false", 5);
}

int is_null(const char* json)
{
    return 0 == strncmp(json, "null", 4); 
}

int is_quote_with_escape_sequence(const char* json)
{
    return ('\\' == *(json - 1) && '"' == *json); 
}

int is_item_key(const char ch)
{
    return is_object_begin(ch) || 
        is_comma(ch);
}

int is_item_value(const char ch)
{
    return is_separator(ch) || 
        is_list_begin(ch);
}

size_t json_string(const char* json, size_t json_length)
{
    ++json;
    --json_length;
    const char* value = json;
    size_t value_length = 0;
    while((!is_quote(*json) && 0 < json_length) || is_quote_with_escape_sequence(json))
    {
        ++json;
        ++value_length;
        --json_length;
    }
     ++json;
    --json_length;
    //TODO Handle missing quote
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    LOG(DEBUG, "String length: %zu\n", value_length);
    LOG(INFO, "%s\n", is_key == 1? "Key" : "Value" );
    LOG(INFO, "String: %.*s\n", (int) value_length, value);
    return json_length; 
}

size_t json_number(const char* json, size_t json_length)
{
    const char* value = json;
    size_t value_length = 0;
    while((is_number(*json) || is_float(*json)) && 0 < json_length) 
    {
        ++json;
        ++value_length;
        --json_length;
    }
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    LOG(DEBUG, "Number length: %zu\n", value_length);
    LOG(INFO, "%s\n", is_key == 1? "Key" : "Value" );
    LOG(INFO, "Number: %.*s\n", (int) value_length, value);
    return json_length;
}

size_t json_true(const char* json, size_t json_length)
{
    const char* value = json;
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    LOG(INFO, "Boolean: %.*s\n", (int) 4, value);
    LOG(INFO, "%s\n", is_key == 1? "Key" : "Value" );
    json_length -= 4;
    return json_length;
}

size_t json_false(const char* json, size_t json_length)
{
    const char* value = json;
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    LOG(INFO, "Boolean: %.*s\n", (int) 5, value);    
    LOG(INFO, "%s\n", is_key == 1? "Key" : "Value" );
    json_length -= 5;
    return json_length;
}

size_t json_null(const char* json, size_t json_length)
{
    const char* value = json;
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    LOG(INFO, "%s\n", is_key == 1? "Key" : "Value" );
    LOG(INFO, "Null: %.*s\n", (int) 4, value);
    json_length -= 4;
    return json_length;
}

void toggle_key_or_value(const char ch)
{
    LOG(DEBUG, "Char: %c\n", ch);
    // Key or Value
    if (is_item_key(ch))
        is_key = 1;
    else if (is_item_value(ch))
        is_key = 0;
}

size_t parse_attributes(const char* json, size_t json_length)
{
    // String: Parse and extract string.
    if (is_quote(*json))
        json_length = json_string(json, json_length);
    // Number: Parse and extract number.
    if (is_number(*json))
        json_length = json_number(json, json_length);
    // True: Parse and extract true.
    if (is_true(json))
        json_length = json_true(json, json_length);
    // False: Parse and extract false.
    if (is_false(json))
        json_length = json_false(json, json_length);
    // Null: Parse and extract null.
    if (is_null(json))
        json_length = json_null(json, json_length);
    // Toggle key or value indicator.
    toggle_key_or_value(*json);
    return json_length;
}

void allocate_heap_storage(void** heap_storage, const size_t size, const size_t count)
{
    // Size must be in the range 1 to MAX_HEAP_BYTES.
    if (1 > size || MAX_HEAP_BYTES < size)
    {
        LOG(ERROR, "size must be in the range 1  byte to %zu bytes.\n", 
                MAX_HEAP_BYTES);
        return;
    }
    // Count must be atleast 1.
    if (1 > count)
    {
        LOG(ERROR, "count cannot be less than 1.\n");
        return;
    }
    // Size * Count must be within MAX_HEAP_BYTES, and
    // Ensure the size * count will not result in size_t overflow.
    if (MAX_HEAP_BYTES / count < size)
    {
        LOG(ERROR, "Total heap memory requested (size %zu * count %zu) "
                "exceeds maximum heap memory %zu bytes permitted.\n",
                size, count, MAX_HEAP_BYTES);
        return;
    }
    // Compute bytes to allocate
    const size_t bytes_to_allocate = size * count;
    void* new_heap_storage = realloc(*heap_storage, bytes_to_allocate);
    if (NULL == new_heap_storage)
    {
        LOG(ERROR, "Heap storage allocation failed, requested %zu bytes.\n", bytes_to_allocate);
        return;
    }
    LOG(INFO, "Heap storage allocated for %zu(bytes).\n", bytes_to_allocate);
    *heap_storage = new_heap_storage;;
}

void parse_json_object(const char* json, size_t json_length)
{
    LOG(DEBUG, "JSON: %s\n", json);
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    while (0 < json_length)
    {
        size_t remaining_json_length = parse_attributes(json, json_length);
        size_t tokens_parsed = json_length - remaining_json_length;
        if (0 < tokens_parsed)
        {
            json += tokens_parsed;
            json_length = remaining_json_length;
        }
        else
        {
            ++json;
            --json_length;
        }
        LOG(DEBUG, "Current json pointed at: %c\n", *json);
        LOG(DEBUG, "JSON LEN: %zu\n", json_length);
    }
}

