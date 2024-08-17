#include <string.h>

#include "symbols.h"

struct symbol_table_t {
  int length;
  int capacity;

  struct symbol_entry **items;
};

static unsigned int hash(const char *key) {
  unsigned int hash = 2166136261u;
  for (int i = 0; i < (int)strlen(key); i++) {
    hash ^= (unsigned char)key[i];
    hash *= 16777619;
  }

  return hash;
}

symbol_table_t *symbol_table_new(int capacity) {
  symbol_table_t *table = malloc(sizeof(*table));

  table->capacity = capacity;
  table->length = 0;
  table->items = malloc(capacity * sizeof(table->items));

  for (int i = 0; i < table->capacity; i++)
    table->items[i] = NULL;

  return table;
}

void symbol_table_free(symbol_table_t **table) {
  assert(table && *table);

  struct symbol_entry *cur, *next;

  for (int i = 0; i < (*table)->capacity; i++) {
    for (cur = (*table)->items[i]; cur; cur = next) {
      next = cur->next;
      free(cur);
    }
  }

  free((*table)->items);
  free(*table);
  *table = NULL;
}

void symbol_table_add(symbol_table_t *table, const char *name,
                      struct ast_node *node) {
  assert(table && name);

  int i = hash(name) % table->capacity;
  struct symbol_entry *entry = malloc(sizeof(*entry));
  entry->name = name;
  entry->decl = node;
  entry->next = table->items[i];

  table->items[i] = entry;
  table->length++;
}

struct ast_node *symbol_table_get(symbol_table_t *table, const char *name) {
  assert(table && name);
  int i = hash(name) % table->capacity;
  struct symbol_entry *entry = table->items[i];

  while (entry) {
    if (strcmp(entry->name, name) == 0) {
      return entry->decl;
    }
    entry = entry->next;
  }

  return NULL;
}
