#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "json.h"
#include "log.h"

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

int is_boolean(const char* json)
{
    return (0 == strncmp(json, "true", 4) || 
            0 == strncmp(json, "false", 5));
}

int is_null(const char* json)
{
    return (0 == strncmp(json, "null", 4)); 
}

int is_begin_marker(const char ch)
{
    return is_list_end(ch) || 
        is_object_end(ch) ||
        is_comma(ch) ||
        is_separator(ch);
}

int is_end_marker(const char ch)
{
    return is_list_end(ch) || 
        is_object_end(ch) ||
        is_comma(ch) ||
        is_separator(ch);
}

int is_quote_with_escape_sequence(const char* json)
{
    return ('\\' == *(json - 1) && '"' == *json); 
}

size_t json_string(const char* json, size_t json_length)
{
    const char* value = json;
    size_t value_length = 0;
    while((!is_quote(*json) && 0 < json_length) || is_quote_with_escape_sequence(json))
    {
        ++json;
        ++value_length;
        --json_length;
    }
    //TODO Handle missing quote
    LOG(DEBUG, "JSON Length    : %zu\n", json_length);
    LOG(DEBUG, "String length   : %zu\n", value_length);
    LOG(DEBUG, "String          : %.*s\n", (int) value_length, value);
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
    LOG(DEBUG, "JSON Length    : %zu\n", json_length);
    LOG(DEBUG, "Number length  : %zu\n", value_length);
    LOG(DEBUG, "Number          : %.*s\n", (int) value_length, value);
    return json_length;
}

size_t json_boolean(const char* json, size_t json_length)
{
    const char* json_true = "true";
    const char* json_false = "false";    
    const char* value = json;
    size_t value_length = 0;
    if (strncmp(json, json_true, 4) == 0)
    {
        value_length = 4;
        json += 4; 
        json_length -= 4;
    }

    if (strncmp(json, json_false, 5) == 0)
    {
        value_length = 5;
        json += 5;
        json_length -= 5;
    }
    LOG(DEBUG, "JSON Length    : %zu\n", json_length);
    LOG(DEBUG, "Boolean length  : %zu\n", value_length);
    LOG(DEBUG, "Boolean       : %.*s\n", (int) value_length, value);
    return json_length;
}

size_t json_null(const char* json, size_t json_length)
{
    const char* json_null = "null";
    const char* value = json;
    size_t value_length = 0;
    if (strncmp(json, json_null, 4) == 0)
    {
        value_length = 4;
        json += 4; 
        json_length -= 4;
    }
    LOG(DEBUG, "JSON Length    : %zu\n", json_length);
    LOG(DEBUG, "Null length  : %zu\n", value_length);
    LOG(DEBUG, "Null             : %.*s\n", (int) value_length, value);
    return json_length;
}

size_t parse_attributes(const char* json, size_t json_length)
{
    // String: Parse and extract word.
    if (is_quote(*json))
    {
        // Skip starting quote from being re-processed
        ++json;
        --json_length;
        size_t before_length = json_length;
        json_length = json_string(json, json_length);
        LOG(DEBUG, "[String] Chars consumed: %zu\n", before_length - json_length);
        // Skip ending quote from being re-processed
        ++json;
        --json_length;
        return json_length;
   }

    // Number: Parse and extract number.
    if (is_number(*json))
    {
        size_t before_length = json_length;
        json_length = json_number(json, json_length);
        LOG(DEBUG, "[Number] Chars consumed: %zu\n", before_length - json_length);
        return json_length;
    }

    // Boolean: Parsed and extract boolean.
    if (is_boolean(json))
    {
        size_t before_length = json_length;
        json_length = json_boolean(json, json_length);
        LOG(DEBUG, "[Bool] Chars consumed: %zu\n", before_length - json_length);
        return json_length;
    }

    // Null: Parsed and extract null.
    if (is_null(json))
    {
        size_t before_length = json_length;
        json_length = json_null(json, json_length);
        LOG(DEBUG, "[Null] Chars consumed: %zu\n", before_length - json_length);
        return json_length;
    }
    return json_length;
}

void* allocate_heap_storage(void* heap_storage, const size_t size, const size_t count)
{
    // Size must be in the range 1 to MAX_HEAP_BYTES.
    if (1 > size || MAX_HEAP_BYTES < size)
    {
        LOG(ERROR, "size must be in the range %zu byte to %zu bytes.\n", 
                size, MAX_HEAP_BYTES);
        return heap_storage;
    }

    // Count must be atleast 1.
    if (1 > count)
    {
        LOG(ERROR, "count cannot be less than 1.\n");
        return heap_storage;
    }

    // Size * Count must be within MAX_HEAP_BYTES, and
    // Ensure the size * count will not result in size_t overflow.
    if (MAX_HEAP_BYTES / count < size)
    {
        LOG(ERROR, "Total heap memory requested (size %zu * count %zu) "
                "exceeds maximum heap memory %zu bytes permitted.\n",
                size, count, MAX_HEAP_BYTES);
        return heap_storage;
    }

    const size_t bytes_to_allocate = size * count;
    void* new_heap_storage = realloc(heap_storage, bytes_to_allocate);
    if (NULL == new_heap_storage)
    {
        LOG(ERROR, "Heap storage allocation failed, requested %zu bytes.\n", bytes_to_allocate);
        return heap_storage;
    }
    LOG(INFO, "Heap storage allocated for %zu(bytes).\n", bytes_to_allocate);
    return new_heap_storage;
}

void parse_json_object(const char* json, size_t json_length)
{
    LOG(DEBUG, "JSON: %s\n", json);
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    json_item* items;

    while (0 < json_length)
    {
        size_t remaining_json_length = parse_attributes(json, json_length);
        size_t tokens_parsed = json_length - remaining_json_length;
        LOG(DEBUG, "Remaining JSON: %zu\n", remaining_json_length);
        LOG(DEBUG, "Tokens parsed : %zu\n", tokens_parsed);
        json += tokens_parsed;
        json_length = remaining_json_length;
        LOG(DEBUG, "Current json pointed at: %c\n", *json);
        if (0 < remaining_json_length)
        {
            ++json;
            --json_length;
        }
        LOG(DEBUG, "JSON LEN: %zu\n", json_length);
    }
}

