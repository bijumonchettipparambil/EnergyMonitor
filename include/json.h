#ifndef _JSON_JSON_H_
#define _JSON_JSON_H_

#include <stdint.h>

#include "log.h"

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
size_t json_string(const char* json, size_t json_length);

/**
 * @brief Consumes a numeric value from the JSON buffer.
 * @param json Pointer to the first digit or sign of the number.
 * @param json_length Remaining length of the buffer.
 * @return Remaining json_length after consuming the number.
 */
size_t json_number(const char* json, size_t json_length);

/**
 * @brief Consumes a boolean literal ("true" or "false") from the buffer.
 * @param json Pointer to the start of the literal.
 * @param json_length Remaining length of the buffer.
 * @return Remaining json_length after consuming the boolean.
 */
size_t json_boolean(const char* json, size_t json_length);

/**
 * @brief Consumes a "null" literal from the buffer.
 * @param json Pointer to the start of "null".
 * @param json_length Remaining length of the buffer.
 * @return Remaining json_length after consuming "null".
 */
size_t json_null(const char* json, size_t json_length);

/**
 * @brief Skips whitespace, commas, and non-separator characters.
 * @param json Pointer to the current position.
 * @param json_length Remaining length of the buffer.
 * @return Updated remaining buffer length.
 */
size_t strip_white_spaces(const char* json, size_t json_length);

/**
 * @brief Identifies the token type at the current pointer and delegates to type-specific parsers.
 * Handles strings (including quote stripping), numbers, booleans, and nulls.
 * @param json Pointer to the current attribute/value.
 * @param json_length Remaining length of the buffer.
 * @return Updated remaining buffer length after processing the attribute.
 */
size_t parse_attributes(const char* json, size_t json_length);

/**
 * @brief Primary entry point for the linear JSON parser.
 * Iteratively parses attributes and advances the buffer pointer until the length is exhausted.
 * @param json Pointer to the start of the JSON string.
 * @param json_length Total length of the JSON string.
 */
void parse_json(const char* json, size_t json_length);
    
#endif
