#include <eosiolib/name.hpp>
#include <eosiolib/action.hpp>
#include "intrinsics.hpp"
#include "crt.hpp"
#include <cstdint>
#include <functional>
#include <stdio.h>
#include <setjmp.h>

eosio::cdt::output_stream std_out;
eosio::cdt::output_stream std_err;

extern "C" {
   int main(int, char**);
   char* _mmap();
   
   static jmp_buf env;
   static jmp_buf test_env;
   static volatile int jmp_ret;
   jmp_buf* ___env_ptr = &env;
   char* ___heap;
   char* ___heap_ptr;
   void ___putc(char c);
   bool ___disable_output;
   bool ___has_failed;

   void _prints_l(const char* cstr, uint32_t len, uint8_t which) {
      for (int i=0; i < len; i++) {
         if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
         else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
         if (!___disable_output)
            ___putc(cstr[i]);
      }
   }

   void _prints(const char* cstr, uint8_t which) {
      for (int i=0; cstr[i] != '\0'; i++) {
         if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
         else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
         if (!___disable_output)
            ___putc(cstr[i]);
      }
   }

   void __set_env_test() {
      ___env_ptr = &test_env;
   }
   void __reset_env() {
      ___env_ptr = &env;
   }

   int _wrap_main(int argc, char** argv) {
      using namespace eosio::native;
      int ret_val = 0;
      ___heap = _mmap();
      ___heap_ptr = ___heap;
      ___disable_output = false;
      ___has_failed = false;
      // preset the print functions
      intrinsics::set_intrinsic<intrinsics::prints_l>([](const char* cs, uint32_t l) {
            _prints_l(cs, l, eosio::cdt::output_stream_kind::std_out);
            });
      intrinsics::set_intrinsic<intrinsics::prints>([](const char* cs) {
            _prints(cs, eosio::cdt::output_stream_kind::std_out);
            });
      intrinsics::set_intrinsic<intrinsics::printi>([](int64_t v) {
            printf("%lli\n", v);
            });
      intrinsics::set_intrinsic<intrinsics::printui>([](uint64_t v) {
            printf("%llu\n", v);
            });
      intrinsics::set_intrinsic<intrinsics::printi128>([](const int128_t* v) {
            int* tmp = (int*)v;
            printf("0x%04x%04x%04x%04x\n", tmp[0], tmp[1], tmp[2], tmp[3]);
            });
      intrinsics::set_intrinsic<intrinsics::printui128>([](const uint128_t* v) {
            int* tmp = (int*)v;
            printf("0x%04x%04x%04x%04x\n", tmp[0], tmp[1], tmp[2], tmp[3]);
            });
      intrinsics::set_intrinsic<intrinsics::printsf>([](float v) {
            prints("What?\n");
            printi(*(int*)&v);
            prints("\n");
            _prints(std::to_string(v).c_str(), eosio::cdt::output_stream_kind::std_out);
            //printf("%f\n", v);
            });
      intrinsics::set_intrinsic<intrinsics::printdf>([](double v) {
            printf("%f\n", v);
            });
      intrinsics::set_intrinsic<intrinsics::printqf>([](const long double* v) {
            int* tmp = (int*)v;
            printf("0x%04x%04x%04x%04x\n", tmp[0], tmp[1], tmp[2], tmp[3]);
            });
      intrinsics::set_intrinsic<intrinsics::printn>([](uint64_t nm) {
            std::string s = eosio::name(nm).to_string();
            prints_l(s.c_str(), s.length());
            });

      jmp_ret = setjmp(env); 
      if (jmp_ret == 0) {
         ret_val = main(argc, argv);
      } else {
         ret_val = -1;
      }
      return ret_val;
   }
}