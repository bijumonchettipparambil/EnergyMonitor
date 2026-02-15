#ifndef JSON_JSON_H_
#define JSON_JSON_H_

#include <stdint.h>

#include "log.h"

/**
 * Constants
 */
// Maximum memory 
#define MAX_HEAP_BYTES 1000000

/**
 * @brief Enum defining JSON item types.
 */
typedef enum
{
    INIT = 0,
    STRING = 1,
    INTEGER = 2,
    FLOAT = 3,
    BOOLEAN = 4,
    NULL_VALUE = 5

} json_item_type;

/**
 * @brief Enum defining JSON item types.
 */
typedef enum
{
    OBJECT = 0,
    ARRAY = 1,
    ITEM = 2
} json_object_type;

/**
 * @brief Deserialised JSON data item, i.e., a name/value pair.
 */
typedef struct
{
    const char* key;
    const size_t key_length;
    const char* value;
    const size_t value_length;
    const int value_only;
    const json_item_type item_type;
} json_item;

/**
 * @brief Deserialised JSON object, may contain zero or more json_items.
 */
typedef struct
{
    const char* name;
    const json_object_type object_type;
    void* item;
    size_t size;
    size_t capacity;
} json_object;

/**
 * @brief Checks if the character is the start of a JSON object.
 * @param ch Character to check.
 * @return 1 if '{', 0 otherwise.
 */
int is_object_begin(const char ch);

/**
 * @brief Checks if the character is the end of a JSON object.
 * @param ch Character to check.
 * @return 1 if '}', 0 otherwise.
 */
int is_object_end(const char ch);

/**
 * @brief Validates if a character is part of a valid JSON number.
 * Supports digits, signs ('+', '-'), and decimal points ('.').
 * @param ch Character to check.
 * @return 1 if valid number character, 0 otherwise.
 */
int is_number(const char ch);

/**
 * @brief Checks if the character is the start of a JSON list (array).
 * @param ch Character to check.
 * @return 1 if '[', 0 otherwise.
 */
int is_list_begin(const char ch);

/**
 * @brief Checks if the character is the end of a JSON list (array).
 * @param ch Character to check.
 * @return 1 if ']', 0 otherwise.
 */
int is_list_end(const char ch);

/**
 * @brief Checks if the character is a JSON key-value separator.
 * @param ch Character to check.
 * @return 1 if ':', 0 otherwise.
 */
int is_separator(const char ch);

/**
 * @brief Checks if the character is a double quote, used for strings.
 * @param ch Character to check.
 * @return 1 if '"', 0 otherwise.
 */
int is_quote(const char ch);

/**
 * @brief Checks if the character is a standard space.
 * @param ch Character to check.
 * @return 1 if ' ', 0 otherwise.
 */
int is_space(const char ch);

/**
 * @brief Checks if the character is a JSON element separator.
 * @param ch Character to check.
 * @return 1 if ',', 0 otherwise.
 */
int is_comma(const char ch);

/**
 * @brief Performs lookahead to check for JSON boolean literals.
 * @param json Pointer to the current position in the buffer.
 * @return 1 if "true" or "false" is found, 0 otherwise.
 */
int is_boolean(const char* json);

/**
 * @brief Performs lookahead to check for a JSON null literal.
 * @param json Pointer to the current position in the buffer.
 * @return 1 if "null" is found, 0 otherwise.
 */
int is_null(const char* json);

/**
 * @brief Consumes a string from the JSON buffer until the closing quote.
 * @param json Pointer starting after the opening quote.
 * @param json_length Remaining length of the buffer.
 * @return Remaining json_length after consuming the string content.
 */
size_t parse_string(const char* json, size_t json_length);

/**
 * @brief Consumes a numeric value from the JSON buffer.
 * @param json Pointer to the first digit or sign of the number.
 * @param json_length Remaining length of the buffer.
 * @return Remaining json_length after consuming the number.
 */
size_t parse_number(const char* json, size_t json_length);

/**
 * @brief Consumes a boolean literal ("true" or "false") from the buffer.
 * @param json Pointer to the start of the literal.
 * @param json_length Remaining length of the buffer.
 * @return Remaining json_length after consuming the boolean.
 */
size_t parse_boolean(const char* json, size_t json_length);

/**
 * @brief Consumes a "null" literal from the buffer.
 * @param json Pointer to the start of "null".
 * @param json_length Remaining length of the buffer.
 * @return Remaining json_length after consuming "null".
 */
size_t parse_null(const char* json, size_t json_length);

/**
 * @brief Checks if a character indicates a floating-point or scientific notation.
 * @param ch The character to evaluate (e.g., '.', 'e', 'E').
 * @return 1 if it is a float-indicator, 0 otherwise.
 */
int is_float(const char ch);

/**
 * @brief Detects the start of a JSON container (Object or Array).
 * @param ch The character to evaluate ('{' or '[').
 * @return 1 if it is a start bracket, 0 otherwise.
 */
int is_begin_marker(const char ch);

/**
 * @brief Detects the end of a JSON container (Object or Array).
 * @param ch The character to evaluate ('}' or ']').
 * @return 1 if it is an end bracket, 0 otherwise.
 */
int is_end_marker(const char ch);

/**
 * @brief Determines if a quote character is escaped by a preceding backslash.
 * @param json Pointer to the current quote in the JSON buffer.
 * @return 1 if the quote is escaped (e.g., \"), 0 if it is a literal/closing quote.
 */
int is_quote_with_escape_sequence(const char* json);

/**
 * @brief Identifies the token type at the current pointer and delegates to type-specific parsers.
 * Handles strings (including quote stripping), numbers, booleans, and nulls.
 * @param json Pointer to the current attribute/value.
 * @param json_length Remaining length of the buffer.
 * @return Updated remaining buffer length after processing the attribute.
 */
size_t parse_attributes(const char* json, size_t json_length, json_item** current_item_address,
        size_t* allocated_count, size_t* used_count);

/**
 * @brief Primary entry point for the linear JSON parser.
 * Iteratively parses attributes and advances the buffer pointer until the length is exhausted.
 * @param json Pointer to the start of the JSON string.
 * @param json_length Total length of the JSON string.
 */
void parse_json_string(const char* json, size_t json_length);

/**
 * @brief Allocates or resizes a block of memory on the heap with safety guards.
 *
 * This function wraps realloc to provide bounds checking and overflow protection.
 * If allocation fails or parameters are invalid, it logs an error and returns the
 * original pointer to prevent memory leaks and data loss.
 *
 * @param heap_storage Pointer to the existing memory block (NULL for new allocation).
 * @param size The size of a single element in bytes.
 * @param count The number of elements to allocate.
 * @return void* A pointer to the (re)allocated memory, or the original pointer on failure.
 */
void* allocate_heap_storage(void* heap_storage, const size_t size, const size_t count);

#endif
