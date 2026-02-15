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

int is_next_item_key(const char ch)
{
    return is_object_begin(ch) || 
        is_comma(ch);
}

int is_next_item_value(const char ch)
{
    return is_separator(ch) || 
        is_list_begin(ch);
}

void toggle_key_or_value(const char ch)
{
    LOG(DEBUG, "Char: %c\n", ch);
    // Key or Value
    if (is_next_item_key(ch))
        is_key = 1;
    else if (is_next_item_value(ch))
        is_key = 0;
}

int is_data_item_key()
{
    return 1 == is_key;
}

int is_data_item_value()
{
    return !is_data_item_key();
}

size_t parse_string(const char* json_string, size_t json_length)
{
    const char* value = json_string;
    size_t string_length = 0;
    while((!is_quote(*json_string) && 0 < json_length) || 
            is_quote_with_escape_sequence(json_string))
    {
        ++json_string;
        ++string_length;
        --json_length;
    }
    //TODO Handle missing quote
    return string_length; 
}

size_t parse_number(const char* json_string, size_t json_length)
{
    const char* value = json_string;
    size_t number_length = 0;
    while((is_number(*json_string) || is_float(*json_string)) && 0 < json_length) 
    {
        ++json_string;
        ++number_length;
        --json_length;
    }
    return number_length;
}

size_t parse_attributes(const char* json_string, size_t json_length, 
        json_item** current_item_address, size_t* allocated_count, size_t* used_count)
{    
    // Toggle key or value indicator.
    toggle_key_or_value(*json_string);
    size_t parsed_data_length = 0;
    json_item_type data_type = INIT;
    // String: Parse and extract string.
    if (is_quote(*json_string))
    {
        // string length +1 to account for leading double quotes.
        parsed_data_length = parse_string(++json_string, --json_length);
        // Reduce json_length by 1 to account for trailing double quotes.
        --json_length;
        data_type = STRING;
    }
    // Number: Parse and extract number.
    else if (is_number(*json_string))
    {
        parsed_data_length = parse_number(json_string, json_length);
        data_type = INTEGER;
    }
    // True: Parse and extract true.
    else if (4 <= json_length && is_true(json_string))
    {
        parsed_data_length = 4;
        data_type = BOOLEAN;
    }
    // False: Parse and extract false.
    else if (5 <= json_length && is_false(json_string))
    {
        parsed_data_length = 5;
        data_type = BOOLEAN;
    }
    // Null: Parse and extract null.
    else if (4 <= json_length && is_null(json_string))
    {
        parsed_data_length = 4;
        data_type = NULL_VALUE;
    }
    //I If a data was parsed, log
    if (0 < parsed_data_length)
    {
        LOG(DEBUG, "Data length: %zu\n", parsed_data_length);
        if (is_data_item_key())
            LOG(INFO, "Key  : %.*s\n", (int) parsed_data_length, json_string);
            // Check if used < alloc
            // if yes, ++current_item_address
            // ++current_item_address
        else
            LOG(INFO, "Value: %.*s\n", (int) parsed_data_length, json_string);
        // Reduce json data length
        json_length -= parsed_data_length;
    }
    return json_length;
}

void* allocate_heap_storage(void* heap_storage, const size_t size, const size_t count)
{
    // Size must be in the range 1 to MAX_HEAP_BYTES.
    if (1 > size || MAX_HEAP_BYTES < size)
    {
        LOG(ERROR, "size must be in the range 1  byte to %zu bytes.\n", 
                MAX_HEAP_BYTES);
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
    // Compute bytes to allocate
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

json_item* allocate_json_item_storage(json_item* items_base_address, size_t *allocated_count)
{
    // Protect in-use storage from being freed
    if (items_base_address && *allocated_count == 0)
    {
        LOG(ERROR, "current_allocated_count is zero and item_pointer is no NULL.\n");
        return items_base_address;
    }
    // If pointer is not null double the storage
    // else allocate storage for 1 item
    size_t new_count = 0;
    if (items_base_address)
        new_count = *allocated_count * 2;
    // else allocate storage for 1 item
    else
        new_count = 1;
    // Allocate storage
    json_item* new_items_base_address = (json_item*) allocate_heap_storage(items_base_address, 
            sizeof(json_item), new_count);
    if (new_items_base_address)
    {
        *allocated_count = new_count;
        items_base_address = new_items_base_address; 
    }
    return items_base_address; 
   
}

void free_json_item_storage(json_item* items_base_address, const size_t allocated_count)
{
    if (items_base_address)
    {
        free(items_base_address);
        LOG(INFO, "json_item storage of %zu(bytes) freed.\n", 
            sizeof(json_item) * allocated_count);
    }
}

void parse_json_string(const char* json_string, size_t json_length)
{
    LOG(DEBUG, "JSON: %s\n", json_string);
    LOG(DEBUG, "JSON Length: %zu\n", json_length);
    const char* base_json_string = json_string;
    // Allocate base storage
    json_item* items_base_address = NULL;
    size_t allocated_count = 0;
    size_t used_count = 0;
    items_base_address = allocate_json_item_storage(items_base_address, &allocated_count);
    json_item* current_item_address = items_base_address;
    LOG(INFO, "json_item storage of %zu(bytes) allocated.\n", 
            sizeof(json_item) * allocated_count);
    while (0 < json_length)
    {
        size_t remaining_json_length = parse_attributes(json_string, json_length, 
                &current_item_address, &allocated_count, &used_count);
        size_t tokens_parsed = json_length - remaining_json_length;
        if (0 < tokens_parsed)
        {
            json_string += tokens_parsed;
            json_length = remaining_json_length;
        }
        else
        {
            ++json_string;
            --json_length;
        }
        LOG(DEBUG, "Current json pointed at: %c\n", *json_string);
        LOG(DEBUG, "JSON LEN: %zu\n", json_length);
    }
    // Free memory
    free_json_item_storage(items_base_address, allocated_count);
}
