/*ram.c*/

/**
  * @brief Random access memory (RAM) for nuPython
  *
  * To execute Python code, the nuPython interpreter needs to store the
  * values of Python variables. The RAM functions serve this purpose.
  *
  * @note Paulina Jimenez-Gonzalez
  *
  * @note template by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // true, false
#include <string.h>
#include <assert.h>

#include "ram.h"

/**
 * @brief double_memory:
 * 
 * when size >= capacity, doubles capcity of RAM
 * 
 * @param memory
 * 
 * @return void
 */
static void double_memory(struct RAM* memory) 
{
  int old_capacity = memory->capacity;
  memory->capacity = memory->capacity * 2;

  memory->cells = (struct RAM_VALUE*) realloc(memory->cells, memory->capacity * sizeof(struct RAM_VALUE));
  memory->map = (struct RAM_MAP*) realloc(memory->map, memory->capacity * sizeof(struct RAM_MAP));

  for(int i = old_capacity; i < memory->capacity; i++) {
    memory->map[i].varname = NULL;
    memory->cells[i].value_type = RAM_TYPE_NONE;
  }

  return;
}

//
// Public functions:
//

/**
  * @brief ram_init: initialize memory unit
  *
  * Returns a pointer to a dynamically-allocated memory
  * for storing nuPython variables and their values. All
  * memory cells are initialized to the value None. You
  * take ownership of the returned memory and must call
  * ram_destroy() when you are done.
  *
  * @return pointer to struct denoting memory unit
  */
struct RAM* ram_init(void)
{
  struct RAM* memory = (struct RAM*) malloc(sizeof(struct RAM));
  memory->size = 0;
  memory->capacity = 4;
  memory->cells = (struct RAM_VALUE*) malloc(memory->capacity * sizeof(struct RAM_VALUE));
  memory->map = (struct RAM_MAP*) malloc(memory->capacity * sizeof(RAM_MAP));

  for (int i = 0; i < memory->capacity; i++) {
    memory->map[i].varname = NULL;
    memory->cells[i].value_type = RAM_TYPE_NONE;
  }

  return memory;
}


/**
  * @brief ram_destroy: frees memory associated with memory unit
  * 
  * Frees the dynamically-allocated memory associated with
  * the given memory. After the call returns, you cannot
  * use the memory.
  *
  * @return void
  */
void ram_destroy(struct RAM* memory)
{
  for(int i=0; i < memory->size; i++) {
    if(memory->cells[i].value_type == RAM_TYPE_STR && memory->cells[i].types.s != NULL) {
      free(memory->cells[i].types.s);
    }
    free(memory->map[i].varname);
  }
  free(memory->cells);
  free(memory->map);
  free(memory);

  return;
}


/**
  * @brief ram_size: # of vars in memory
  *
  * Returns the # of variables currently stored in memory.
  *
  * @return # of vars in memory
  */
int ram_size(struct RAM* memory)
{
  return memory->size;
}


/**
  * @brief ram_capacity: # of cells available in memory
  *
  * Returns the # of cells currently available in memory.
  *
  * @return # of cells available in memory
  */
int ram_capacity(struct RAM* memory)
{
  return memory->capacity;
}


/**
  * @brief ram_get_addr: address of memory cell occupied by variable
  *
  * If the given variable (e.g. "x") has been written to 
  * memory, returns the address of this variable --- an integer
  * in the range 0..N-1 where N is the number of vars currently 
  * stored in memory. Returns -1 if no such variable exists 
  * in memory. 
  *
  * NOTE: a variable has to be written to memory before you can
  * get its address. Once a variable is written to memory, its
  * address never changes. 
  *
  * @param memory Pointer to struct denoting memory unit
  * @param varname variable name
  * @return address of variable or -1 if doesn't exist
  */
int ram_get_addr(struct RAM* memory, char* varname)
{
  if (memory == NULL || varname == NULL)
    return -1;
  
  //binary seacrh over the sorted map 
  int left = 0;
  int right = memory->size - 1;
  int c;

  while (left <= right) {
    int mid = (left + right) / 2;
    c = strcmp(varname, memory->map[mid].varname);    
    
    if (c == 0)
      return memory->map[mid].cell;
    else if (c < 0)
      right = mid - 1; //search left half
    else 
      left = mid + 1; //search right half
  }
  
  return -1;
}


/**
  * @brief ram_read_cell_by_addr: returns value in memory cell at this address
  *
  * Given a memory address (an integer in the range 0..N-1), 
  * returns a COPY of the value contained in that memory cell.
  * Returns NULL if the address is not valid.
  * 
  * NOTE: this function allocates memory for the value that
  * is returned. The caller takes ownership of the copy and 
  * must eventually free this memory via ram_free_value().
  *
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param address memory cell address
  * @return pointer to struct containing value or NULL if doesn't exist
  */
struct RAM_VALUE* ram_read_cell_by_addr(struct RAM* memory, int address)
{
  if (memory == NULL)
    return NULL;
    
  if(address < memory->size && address >= 0) {
    struct RAM_VALUE* copy = (struct RAM_VALUE*) malloc (sizeof(struct RAM_VALUE));
    copy->types.s = strdup(memory->cells[address].types.s);
    //copy->value_type = memory->cells[address].value_type;
    if (memory->cells[address].value_type == RAM_TYPE_STR) {
      copy->types.s = strdup(memory->cells[address].types.s);
    }
    else if (memory->cells[address].value_type == RAM_TYPE_REAL){
        copy->types.d = memory->cells[address].types.d;
    }
    else {
      copy->types.i = memory->cells[address].types.i;
    }
    return copy;
  }
  else {
    return NULL;
  }
}


/**
  * @brief ram_read_cell_by_name: returns value in memory cell for this variable
  *
  * If the given variable (e.g. "x") has been written to 
  * memory, returns a COPY of the value contained in memory.
  * Returns NULL if no such name exists in memory.
  *
  * NOTE: this function allocates memory for the value that
  * is returned. The caller takes ownership of the copy and 
  * must eventually free this memory via ram_free_value().
  *
  * @param memory Pointer to struct denoting memory unit
  * @param varname variable name
  * @return pointer to struct containing value or NULL if doesn't exist
  */
struct RAM_VALUE* ram_read_cell_by_name(struct RAM* memory, char* varname)
{
  for(int i=0; i < memory->size; i++) {
    if (strcmp(varname, memory->map[i].varname) == 0) {
      struct RAM_VALUE* copy = (struct RAM_VALUE*) malloc(sizeof(struct RAM_VALUE));
      int address = memory->map[i].cell;
      copy->value_type = memory->cells[address].value_type;

      if(memory->cells[address].value_type == RAM_TYPE_STR){
        copy->types.s = strdup(memory->cells[address].types.s);
        //copy->types.s = memory->cells[i].types.s;
      }
      else if (memory->cells[address].value_type == RAM_TYPE_REAL){
        copy->types.d = memory->cells[address].types.d;
      }
      else {
        copy->types.i = memory->cells[address].types.i;
      }
      return copy;
    }
  }
  return NULL;
}


/**
  * @brief ram_free_value: free value returned by read_cell() functions
  *
  * Frees the memory value returned by ram_read_cell_by_name and
  * ram_read_cell_by_addr.
  *
  * @param value Pointer to struct containing value
  * @return void
  */
void ram_free_value(struct RAM_VALUE* value)
{
  if(value == NULL){
    return;
  }
  if(value->value_type == RAM_TYPE_STR) {
    free(value->types.s);
  }
  free(value);
  return;
}


/**
  * @brief ram_write_cell_by_addr: writes a value to memory cell at this address
  *
  * Writes the given value to the memory cell at the given 
  * address. If a value already exists at this address, that
  * value is overwritten by this new value. Returns true if 
  * the value was successfully written, false if not (which 
  * implies the memory address is invalid).
  *
  * NOTE: if the value being written is a string, it will
  * be duplicated and stored.
  * 
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param value value to be written to memory
  * @param address memory cell address
  * @return true if successful, false if not (invalid address)
  */
bool ram_write_cell_by_addr(struct RAM* memory, struct RAM_VALUE value, int address)
{
  if (memory == NULL)
    return false;
  // if overwriting a string, free the old one
  if (address < memory->capacity && address >= 0) {
    if (memory->cells[address].value_type == RAM_TYPE_STR && memory->cells[address].types.s != NULL) {
      free(memory->cells[address].types.s);
    }

    memory->cells[address].value_type = value.value_type;

    if(memory->cells[address].value_type == RAM_TYPE_STR) {
      memory->cells[address].types.s = strdup(value.types.s);
    }
    else if (memory->cells[address].value_type == RAM_TYPE_REAL) {
      memory->cells[address].types.d = value.types.d;
    }
    else {
      memory->cells[address].types.i = value.types.i;
    }
    return true;
  }
  else {
    return false;
  }
}


/**
  * ram_write_cell_by_name
  *
  * Writes the given value to a memory cell named by the given
  * variable. If a memory cell already exists with this name,
  * the existing value is overwritten by this new value. Returns
  * true since this operation always succeeds.
  *
  * NOTE: if the value being written is a string, it will
  * be duplicated and stored.
  *
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param value value to be written to memory
  * @param varname variable name
  * @return true (always successful)
  */
bool ram_write_cell_by_name(struct RAM* memory, struct RAM_VALUE value, char* varname)
{
  // check if var already exists
  int address = ram_get_addr(memory, varname);
  if (address != -1) {
    ram_write_cell_by_addr(memory, value, address);
    return true;
  }

  // Double memory if capacity = size
  if (memory->size >= memory->capacity) 
    double_memory(memory);

  // Store var alphabetically
  int index = 0;
  while (index < memory->size && strcmp(varname, memory->map[index].varname) > 0) {
    index++;
  }

  for (int i= memory->size; i > index; i--) {
    memory->map[i] = memory->map[i - 1];
  }

  memory->map[index].varname = strdup(varname);
  memory->map[index].cell = memory->size;

  ram_write_cell_by_addr(memory, value, memory->size);

  memory->size++;

  return true;

}



/**
  * @brief ram_print: prints the contents of memory
  *
  * Prints the contents of RAM to the console, for debugging.
  * RAM is printed in alphabetical order by variable name.
  *
  * @param memory Pointer to struct denoting memory unit
  * @return void
  */
void ram_print(struct RAM* memory)
{
  printf("**MEMORY PRINT**\n");

  printf("Size: %d\n", memory->size);
  printf("Capacity: %d\n", memory->capacity);
  printf("Contents:\n");

  for (int i = 0; i < memory->size; i++)
  {
   printf(" %s: ", memory->map[i].varname);

   if (memory->cells[i].value_type == RAM_TYPE_INT) {
    printf("int, %d", memory->cells[i].types.i);
   }
   else if (memory->cells[i].value_type == RAM_TYPE_REAL) {
    printf("real, %lf", memory->cells[i].types.d);
   }
   else if (memory->cells[i].value_type == RAM_TYPE_STR) {
    printf("str, '%s'", memory->cells[i].types.s);
   }
   else if (memory->cells[i].value_type == RAM_TYPE_PTR) {
    printf("ptr, %d", memory->cells[i].types.i);
   }
   else if(memory->cells[i].value_type == RAM_TYPE_BOOLEAN) {
    if (memory->cells[i].types.i == 0)
      printf("boolean, False");
    else
      printf("boolean, True");
   }
   else {
   printf("none, None");
   }
  
   printf("\n");
  }

  printf("**END PRINT**\n");
}


/**
  * @brief ram_print_map: prints the contents of memory map
  *
  * Prints contents of RAM map to the console, for debugging.
  *
  * @param memory Pointer to struct denoting memory unit
  * @return void
  */
void ram_print_map(struct RAM* memory)
{
  printf("**MEMORY MAP PRINT**\n");

  for (int i = 0; i < memory->size; i++)
  {
   printf(" %s: %d\n", memory->map[i].varname, memory->map[i].cell);
  }

  printf("**END PRINT**\n");
}
