/*tests.c*/

/**
  * @brief unit tests for nuPython's memory unit
  *
  * @note Paulina Jimenez-Gonzalez
  *
  * @note Initial tests by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <string>
#include <gtest/gtest.h>

#include "ram.h"

using namespace std;


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

// 

TEST(memory_module, write_one_str)
{
  struct RAM* memory = ram_init();
  
  struct RAM_VALUE str;



  str.value_type = RAM_TYPE_STR;
  str.types.s = "string";

  // can we store new string correctly

  bool success = ram_write_cell_by_name(memory, str, "s1");

  ASSERT_TRUE(success);
  ASSERT_EQ(ram_size(memory), 1);
  ASSERT_STREQ(memory->cells[0].types.s, "string");  
  ASSERT_STREQ(memory->map[0].varname, "s1");
  ASSERT_EQ(memory->cells[0].value_type, RAM_TYPE_STR);
  ASSERT_EQ(memory->map[0].cell, 0);

  //
  // tests passed, free memory
  //
  ram_destroy(memory);

}

TEST(memory_module, write_one_str_read_back)
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  struct RAM_VALUE str;

  str.value_type = RAM_TYPE_STR;
  str.types.s = "string";

  bool success = ram_write_cell_by_name(memory, str, "s1");
  
  ASSERT_TRUE(success);
  ASSERT_EQ(ram_size(memory), 1);

  //
  // can we read the value back successfully?
  //
  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "s1");

  //ram_print(memory);
  //ram_print_map(memory);

  ASSERT_TRUE(value != NULL);  
  ASSERT_EQ(value->value_type, RAM_TYPE_STR);
  ASSERT_STREQ(value->types.s, memory->cells[0].types.s);

  //
  // tests passed, free memory
  //
  ram_free_value(value);
  ram_destroy(memory);
}

TEST(memory_module, write_four_str)
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  vector<string> values = {"pera", "kiwi", "uva", "fresa"};
  vector<string> names = {"s1", "s2", "s3", "s4"};

  struct RAM_VALUE str;

  for (int i = 0; i < 4; i++) {
      str.value_type = RAM_TYPE_STR;
      str.types.s = (char*) values[i].c_str();

      bool success = ram_write_cell_by_name(memory, str, (char*) names[i].c_str());
      ASSERT_TRUE(success);
      ASSERT_EQ(ram_size(memory), i + 1);

      ASSERT_STREQ(memory->cells[i].types.s, (char*) values[i].c_str());
      ASSERT_STREQ(memory->map[i].varname, (char*) names[i].c_str());
      ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_STR);
      ASSERT_EQ(memory->map[i].cell, i);
  }

  //
  // tests passed, free memory
  //
  //ram_free_value(value);
  ram_destroy(memory);
}

TEST(memory_module, varname_sort_check)
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  vector<string> values = {"pera", "kiwi", "uva", "fresa"};
  vector<string> names = {"z", "b", "e", "a"};

  struct RAM_VALUE str;

  for (int i = 0; i < 4; i++) {
      str.value_type = RAM_TYPE_STR;
      str.types.s = (char*) values[i].c_str();

      bool success = ram_write_cell_by_name(memory, str, (char*) names[i].c_str());
      ASSERT_TRUE(success);
  }

  ASSERT_EQ(ram_size(memory), (int)names.size());

  vector<string> sorted_names;

  for (int i = 0; i< ram_size(memory); i++) {
    sorted_names.push_back(memory->map[i].varname);
  }

  ASSERT_TRUE(is_sorted(sorted_names.begin(), sorted_names.end()));

  //
  // tests passed, free memory
  //
  //ram_free_value(value);
  ram_destroy(memory);
}

TEST(memory_module, c_strings) 
{

  // check string are duplicated before storing

  struct RAM* memory = ram_init();

  struct RAM_VALUE str;
  str.value_type = RAM_TYPE_STR;
  str.types.s = strdup("apple");

  bool success = ram_write_cell_by_name(memory, str, "s1");

  ASSERT_TRUE(success);
  ASSERT_EQ(ram_size(memory), 1);

  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "s1");
  ASSERT_EQ(value->value_type, RAM_TYPE_STR);

  ASSERT_STREQ(value->types.s, memory->cells[0].types.s);

  strcpy(value->types.s, "banana"); // modify the copy
  ASSERT_STREQ(memory->cells[0].types.s, "apple");

  //
  // tests passed, free memory
  //
  ram_free_value(value);
  ram_destroy(memory);

}

TEST(memory_module, more_strings)
{
  struct RAM* memory = ram_init();
  
  vector<string> values = {"pera", "kiwi", "uva", "fresa"};
  vector<string> names = {"s1", "s2", "s3", "s4"};

  struct RAM_VALUE str;

  // write strings into memory

  for (int i = 0; i < 4; i++) {
    str.value_type = RAM_TYPE_STR;
    str.types.s = (char*) values[i].c_str(); // point to original string

    bool success = ram_write_cell_by_name(memory, str, (char*) names[i].c_str());
    ASSERT_TRUE(success);
    ASSERT_EQ(ram_size(memory), (int)(i+1));

    // can make more check is necessary
  }

  // read back all strings
  for (size_t i = 0; i < names.size(); i++) {
      struct RAM_VALUE* value = ram_read_cell_by_name(memory, (char*)names[i].c_str());
      ASSERT_TRUE(value != nullptr);
      ASSERT_EQ(value->value_type, RAM_TYPE_STR);

      ASSERT_STREQ(value->types.s, values[i].c_str());

      // modify copy and ensure original value is unchanged
      strcpy(value->types.s, "modified");

      // To check original memory, use the mapped address
      int addr = -1;

      for (int j = 0; j < memory->size; j++) {
          if (strcmp(memory->map[j].varname, names[i].c_str()) == 0) {
              addr = memory->map[j].cell;
              break;
          }
      }

      ASSERT_NE(addr, -1);  // sanity check
      ASSERT_STREQ(memory->cells[addr].types.s, values[i].c_str());

      // Free the copy
      ram_free_value(value);
    }

    ram_destroy(memory);
}

TEST(memory_module, doubling_memory)
{
  struct RAM* memory = ram_init();

  vector<string> values = {"pera", "kiwi", "uva", "fresa", "sandia"};
  vector<string> names = {"s1", "s2", "s3", "s4", "s5"};

  struct RAM_VALUE str;

  for (int i = 0; i < 5; i++) {
    str.value_type = RAM_TYPE_STR;
    str.types.s = (char*) values[i].c_str();

    bool success = ram_write_cell_by_name(memory, str, (char*)names[i].c_str());
    ASSERT_TRUE(success);
    ASSERT_EQ(ram_size(memory), (int)(i + 1));

    // more checks
    ASSERT_STREQ(memory->cells[i].types.s, values[i].c_str());
    ASSERT_STREQ(memory->map[i].varname, names[i].c_str());
    ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_STR);
  }

  // make sure values still there after doubling
  for (size_t i = 0; i < names.size(); i++) {
    struct RAM_VALUE* value = ram_read_cell_by_name(memory, (char*)names[i].c_str());
    ASSERT_TRUE(value != nullptr);
    ASSERT_EQ(value->value_type, RAM_TYPE_STR);
    ASSERT_STREQ(value->types.s, values[i].c_str());

    ram_free_value(value);
  }

  ram_destroy(memory);
}

TEST(memory_module, addressing) 
{
  RAM* memory = ram_init();

  RAM_VALUE v1;
  v1.value_type = RAM_TYPE_INT;
  v1.types.i = 123;
  ram_write_cell_by_name(memory, v1, "a");

  RAM_VALUE* read1 = ram_read_cell_by_name(memory, "a");
  ASSERT_EQ(read1->types.i, 123);

  RAM_VALUE v2;
  v2.value_type = RAM_TYPE_STR;
  v2.types.s = strdup("apple");
  ram_write_cell_by_name(memory, v2, "b");

  RAM_VALUE* read2 = ram_read_cell_by_name(memory, "b");
  ASSERT_STREQ(read2->types.s, "apple");

  // Test updating an existing cell
  v1.types.i = 67;
  ram_write_cell_by_name(memory, v1, "a");
  read1 = ram_read_cell_by_name(memory, "a");
  ASSERT_EQ(read1->types.i, 67);

  ram_free_value(read1);
  ram_free_value(read2);

  ram_destroy(memory);
}

TEST(memory_module, overwrite_existing_var)
{
  struct RAM* memory = ram_init();

  struct RAM_VALUE v1;
  struct RAM_VALUE v2;

  v1.value_type = RAM_TYPE_INT;
  v1.types.i = 100;
  ram_write_cell_by_name(memory, v1, "x");

  v2.value_type = RAM_TYPE_INT;
  v2.types.i = 200;
  ram_write_cell_by_name(memory, v2, "x"); // overwrite

  ASSERT_EQ(ram_size(memory), 1);
  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");
  ASSERT_EQ(value->types.i, 200);

  ram_free_value(value);
  ram_destroy(memory);

}

TEST(memory_module, read_var_not_found)
{
  struct RAM* memory = ram_init();

  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "string");
  ASSERT_TRUE(value == NULL);

  ram_free_value(value);

  ram_destroy(memory);
}

TEST (memory_module, empty_varname)
{
  struct RAM* memory = ram_init();

  struct RAM_VALUE v1;
  v1.value_type = RAM_TYPE_INT;
  v1.types.i = 24;

  bool success = ram_write_cell_by_name(memory, v1, "");
  ASSERT_TRUE(success);

  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "");
  ASSERT_TRUE(value != NULL);
  ASSERT_EQ(value->types.i, 24);

  ram_free_value(value);
  ram_destroy(memory);
}

//testing other types like real

TEST(memory_module, write_and_read_real)
{
  struct RAM* memory = ram_init();

  struct RAM_VALUE v1;
  v1.value_type = RAM_TYPE_REAL;
  v1.types.d = 1.89898989f;

  bool success = ram_write_cell_by_name(memory,v1, "fl");
  ASSERT_TRUE(success);

  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "fl");
  ASSERT_TRUE(value != NULL);
  ASSERT_EQ(value->value_type, RAM_TYPE_REAL);
  ASSERT_FLOAT_EQ(value->types.d, 1.89898989f);

  ram_free_value(value);
  ram_destroy(memory);
}

TEST(memory_module, sorting_after_modifications)
{
  struct RAM* memory = ram_init();

  vector<string> values = {"pera", "kiwi", "uva", "fresa", "mango", "naranja", "limon"};
  vector<string> names = {"z", "b", "e", "a", "c", "a", "t"};

  struct RAM_VALUE v1;
  v1.value_type = RAM_TYPE_INT;

  for (size_t i = 0; i < values.size(); i++) {
    v1.types.s = (char*) values[i].c_str();
    ram_write_cell_by_name(memory, v1, (char*)names[i].c_str());
  }

  //verify sorted
  vector<string> old_vector;

  for (int i = 0; i < ram_size(memory); i++)
    old_vector.push_back(memory->map[i].varname);
  ASSERT_TRUE(is_sorted(old_vector.begin(), old_vector.end()));

  // update a varibale

  v1.types.s = "banana";
  ram_write_cell_by_name(memory, v1, "e");

  vector<string> new_vector;
  for (int i =0; i < ram_size(memory); i++)
    new_vector.push_back(memory->map[i].varname);
  ASSERT_TRUE(is_sorted(new_vector.begin(), new_vector.end()));

  ram_destroy(memory);
}

TEST(memory_module, mixed_types)
{
  struct RAM* memory = ram_init();
  
  RAM_VALUE vi, vd, vs;
  vi.value_type = RAM_TYPE_INT; vi.types.i = 10;
  vd.value_type = RAM_TYPE_REAL; vd.types.d = 2.5f;
  vs.value_type = RAM_TYPE_STR; vs.types.s = strdup("ok");

  ram_write_cell_by_name(memory, vi, "i");
  ram_write_cell_by_name(memory, vd, "d");
  ram_write_cell_by_name(memory, vs, "s");

  RAM_VALUE* read_i = ram_read_cell_by_name(memory, "i");
  RAM_VALUE* read_d = ram_read_cell_by_name(memory, "d");
  RAM_VALUE* read_s = ram_read_cell_by_name(memory, "s");

  ASSERT_EQ(read_i->types.i, 10);
  ASSERT_FLOAT_EQ(read_d->types.d, 2.5f);
  ASSERT_STREQ(read_s->types.s, "ok");

  ram_free_value(read_i);
  ram_free_value(read_d);
  ram_free_value(read_s);
  ram_destroy(memory);

}