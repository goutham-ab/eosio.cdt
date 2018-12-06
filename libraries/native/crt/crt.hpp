#pragma once
#include <setjmp.h>

namespace eosio { namespace cdt {
   enum output_stream_kind {
      std_out,
      std_err,
      none
   };
   struct output_stream {
      char output[1024*2];
      size_t index = 0;
      std::string to_string()const { return std::string((const char*)output, index); }
      const char* get()const { return output; }
      void push(char c) { output[index++] = c; }
      void flush() { index = 0; }
   };
}} //ns eosio::cdt

extern eosio::cdt::output_stream std_out;
extern eosio::cdt::output_stream std_err;
extern "C" jmp_buf* ___env_ptr;
extern "C" {
   void __set_env_test();
   void __reset_env();
}