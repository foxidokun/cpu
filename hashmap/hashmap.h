#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <stdint.h>
#include "bits.h"

/**
 * @brief      HashMap struct
 * 
 * @note Memory usage is constant and approximately equal to sizeof (hashmap) + (key_size + val_size)*allocated
 */
struct hashmap
{
    size_t    used;          /// Counter of used indexes
    size_t    allocated;     /// Counter of allocated indexes
    size_t    max_key_size;  /// Max key size
    size_t    max_val_size;  /// Max value size
    bitflags *flags;         /// Bool bit flags (is index used or not)
    void     *keys;          /// Keys array
    void     *values;        /// Array of pointers to values
    long unsigned int (*hash)(const void *, size_t); /// Hash function Key, Key size -> uint
    int (*comp)(const void *, const void *); /// Comparator for keys like strcmp (0 if equal)
};

/**
 * @brief      Allocates and initialises new empty hashmap
 *
 * @param[in]  capacity      Capacity
 * @param[in]  max_key_size  Max key size (in bytes)
 * @param[in]  max_val_size  Max value size (in bytes)
 * @param[in]  hash          Hash function Key->uint
 * @param[in]  comp          Comparator for keys like strcmp (0 if equal)
 *
 * @return     Pointer to allocated memory or NULL in case of OOM
 */
hashmap *hashmap_create (size_t capacity, size_t max_key_size, size_t max_val_size,
                    long unsigned int hash(const void *, size_t), int (*comp)(const void *, const void *));

/// Free memmory occupied by map
void hashmap_free (hashmap *map);

/// Clear all contents in map
void hashmap_clear (hashmap *map);

/**
 * @brief      Delete old_map, create new with new_size and move all content from old map to new 
 *
 * @note In case of OOM old_map 
 *
 * @param      old_map   Old hashmap    
 * @param[in]  new_size  New size
 *
 * @return     Pointer to allocated memory or NULL in case of OOM
 */
hashmap *hashmap_resize (hashmap *old_map, size_t new_size);

/**
 * @brief      Insert copy of key and copy of value into hashmap
 *
 * @param      map          Map
 * @param[in]  key          Key
 * @param      key_size     Key size
 * @param[in]  value        Value
 * @param      val_size     Value size
 *
 * @return     Non-zero value in case of full hashmap
 */
int hashmap_insert (hashmap *map, const void *key, size_t key_size, const void *value, size_t val_size);

/**
 * @brief      Returns pointer to value with this key
 * 
 * @param[in]  map   Hashmap
 * @param[in]  key   Key
 *
 * @return     Pointer to internal buffer or NULL if can't find value
 * 
 * @note Yeah, this is unsound behavior. BUT until
 * hashmap_edit or smth like this will be inplemented and until
 * you will call hashmap_clear / hashmap_resize / hashmap_free
 * this is safe function.
 */
void *hashmap_get (const hashmap *map, const void *key);

/**
 * @brief      Execute func for all values
 *
 * @param      map   Hashmap
 * @param[in]  func  The function that takes value
 */
void hashmap_forall (hashmap *map, void func (void *));

#endif