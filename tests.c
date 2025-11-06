/*tests.c*/

/**
  * @brief unit tests for nuPython's memory unit
  *
  * @note YOUR NAME
  *
  * @note Initial tests by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtest/gtest.h>

#include "ram.h"


//
// private helper functions:
//


//
// some provided unit tests to get started:
//
TEST(memory_module, initialization)
{
  //
  // create a new memory and make sure it's initialized properly:
  //
  struct RAM* memory = ram_init();

  ASSERT_TRUE(memory != NULL);        // use ASSERT_TRUE with pointers
  ASSERT_TRUE(memory->cells != NULL);
  ASSERT_TRUE(memory->map != NULL);

  ASSERT_EQ(ram_size(memory), 0);
  ASSERT_EQ(ram_capacity(memory), 4);
  
  //
  // memory cells should be initialized to NONE:
  //
  for (int i=0; i<ram_capacity(memory); i++) {
    ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_NONE);
  }

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}

TEST(memory_module, write_one_int) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  //
  // now check the memory, was x = 123 stored properly?
  //
  // since this is the first variable written, it will go
  // into memory cell 0:
  //
  ASSERT_EQ(memory->cells[0].value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].types.i, 123);
  //
  // it will also go into map location 0:
  //
  ASSERT_STREQ(memory->map[0].varname, "x");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[0].cell, 0);

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}

TEST(memory_module, write_one_int_read_back) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  //
  // can we read the value back successfully?
  //
  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");

  ASSERT_TRUE(value != NULL);  
  ASSERT_EQ(value->value_type, RAM_TYPE_INT);
  ASSERT_EQ(value->types.i, 123);

  //
  // tests passed, free memory
  //
  ram_free_value(value);
  ram_destroy(memory);
}


//
// TODO: add more unit tests
//


