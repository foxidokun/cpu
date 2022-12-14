#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../common/common.h"
#include "../stack/hash.h"
#include "hashmap.h"

hashmap *hashmap_create (size_t capacity, size_t max_key_size, size_t max_val_size,
                    hash_t hash(const void *, size_t), int (*comp)(const void *, const void *))
{
    assert (hash != NULL && "function can't be NULL");
    assert (comp != NULL && "function can't be NULL");

    hashmap *map = (hashmap *) calloc (1, sizeof (hashmap));
    _UNWRAP_NULL (map);

    map->used         = 0;
    map->allocated    = capacity;
    map->max_key_size = max_key_size;
    map->max_val_size = max_val_size;
    map->flags        = create_bitflags (capacity);
    map->keys         = calloc (capacity, max_key_size);
    map->values       = calloc (capacity, max_val_size);
    map->hash         = hash;
    map->comp         = comp;
    
    _UNWRAP_NULL (map->flags);
    _UNWRAP_NULL (map->keys);
    _UNWRAP_NULL (map->values);

    return map;
}

void hashmap_free (hashmap *map)
{
    assert (map != NULL && "pointer can't be NULL");

    free_bitflags (map->flags);
    free (map->keys);
    free (map->values);
    free (map);
}

void hashmap_clear (hashmap *map)
{
    assert (map != NULL && "pointer can't be NULL");

    map->used = 0;
    clear_bitflags (map->flags);
}

hashmap *hashmap_resize (hashmap *old_map, size_t new_size)
{
    assert (old_map != NULL && "pointer can't be NULL");
    assert (new_size > old_map->used && "New hashmap must be bigger than number of elements in old");

    hashmap *new_map = hashmap_create (new_size, old_map->max_key_size, old_map->max_val_size,
                                        old_map->hash, old_map->comp);
    _UNWRAP_NULL (new_map);


    for (size_t id = 0; id < old_map->allocated; ++id)
    {
        if (check_bit (old_map->flags, id))
        {
            hashmap_insert (new_map,
                (char *) old_map->keys   + id*old_map->max_key_size, old_map->max_key_size,
                (char *) old_map->values + id*old_map->max_val_size, old_map->max_val_size);
        }
    }

    hashmap_free (old_map);

    return new_map;
}

int hashmap_insert (hashmap *map, const void *key, size_t key_size, const void *value, size_t val_size)
{
    assert (map   != NULL && "pointer can't be NULL");
    assert (key   != NULL && "pointer can't be NULL");
    assert (value != NULL && "pointer can't be NULL");
    assert (key_size <= map->max_key_size && "Not enough space for this key");
    assert (val_size <= map->max_val_size && "Not enough space for this value");

    if (map->allocated == map->used) return ERROR; // OOM

    size_t id = map->hash (key, key_size) % map->allocated;

    // ID already in use with different key
    bool not_same = (bool) map->comp (key, (char *) map->keys + id*map->max_key_size);

    if (check_bit (map->flags, id) && not_same)
    {
        ssize_t id_tmp = bit_find_value (map->flags, 0, id);
        assert (id_tmp != ERROR && "Used < allocated, but all cells are occupied");

        id = (size_t) id_tmp;
    }

    set_bit_true (map->flags, id);

    if (not_same)
    {
        map->used++;

        memcpy ((char *) map->keys   + id*map->max_key_size,   key, key_size);
        memcpy ((char *) map->values + id*map->max_val_size, value, val_size);
    }

    int debug = 0;
    for (size_t i = 0; i < map->allocated; ++i)
    {
        if (check_bit (map->flags, i))
        {
            debug++;
        }
    }

    if (debug != map->used)
    {
        log (log::ERR, "real: %zu official: %zu", debug, map->used);
        assert (0);
    }
    assert (debug == map->used);

    return 0;
}

void *hashmap_get (const hashmap *map, const void *key)
{
    assert (map != NULL && "pointer can't be NULL");
    assert (key != NULL && "pointer can't be NULL");

    size_t id = map->hash (key, map->max_key_size) % map->allocated;

    if (check_bit (map->flags, id) && !map->comp (key, (char *) map->keys + id*map->max_key_size))
    {
        return (char *) map->values + id*map->max_val_size;
    }

    for (id = 0; id < map->allocated; ++id)
    {
        if (check_bit (map->flags, id))
        {
            if (!map->comp (key, (char *) map->keys + id*map->max_key_size))
            {
                return (char *) map->values + id*map->max_val_size;
            }
        }
        else 
        {
            return NULL;
        }
    }

    return NULL;
}

void hashmap_forall (hashmap *map, void func (void *))
{
    assert (map  != NULL && "pointer can't be NULL");
    assert (func != NULL && "pointer can't be NULL");

    for (size_t id = 0; id < map->allocated; ++id)
    {
        if (check_bit (map->flags, id))
        {
            func ((char *) map->values + id*map->max_val_size);
        }
    }
}