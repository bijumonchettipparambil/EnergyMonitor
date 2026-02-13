#ifndef _MAP_MAP_H_
#define _MAP_MAP_H_



typedef struct
{
    void* key;
    void* value;
} map_item;

typedef struct
{
    map_item* data;
    size_t size;
    size_t length;

} map;



#endif
