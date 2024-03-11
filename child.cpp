#include <Windows.h>
#include <bitset>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "methods.hpp"

#define TEST__

int main(int argc, char const* argv[]) {
  // all arguments that process need are passed through cmd arguments
  unsigned int number = strtoul(argv[0], NULL, 10);  // number of process - 0,1,2,...
  unsigned int raw_data_offset = strtoul(argv[1], NULL, 10);
  unsigned int image_offset = strtoul(argv[2], NULL, 10);
  unsigned int result_offset = strtoul(argv[3], NULL, 10);
  unsigned int args_offset = strtoul(argv[4], NULL, 10);
  unsigned int n_processes = strtoul(argv[5], NULL, 10);
  unsigned int size_of_shared_memory = strtoul(argv[6], NULL, 10);

  // get access to the shared memory
  char MAP_NAME[] = "MAP_NAME";
  HANDLE mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, MAP_NAME);
  if (mapping == NULL) {
    std::cerr << "Could not open file mapping object: " << GetLastError() << std::endl;
    exit(1);
  }

  unsigned char* shared_memory_ptr = (unsigned char*)MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, size_of_shared_memory);
  if (shared_memory_ptr == NULL) {
    std::cout << "Could not map view of file: " << GetLastError() << std::endl;
    CloseHandle(mapping);
    exit(1);
  }

  // read begins and ends ( functions get_4b and set_4b can be found in methods.cpp file)
  // get_4b interprets 4 bytes as an unsigned int
  // set_4b set unsigned int to 4 bytes in memory

  // this cycle aren't used in test case
  std::vector<unsigned int> begins;
  std::vector<unsigned int> ends;
  for (int j = 0; j < n_processes; ++j) {
    begins.push_back(get_4b(shared_memory_ptr, args_offset + (number * n_processes * 2 + j) * sizeof(unsigned int)));
    ends.push_back(get_4b(shared_memory_ptr, args_offset + (number * n_processes * 2 + j + n_processes) * sizeof(unsigned int)));
  }

  // here I cast necessary pointer to BMP*, so after that I can write for example image->width_ and this works correctly
  BMP* image = (BMP*)(shared_memory_ptr + image_offset);
  // after that I write true pointer to raw_data to image->raw_data, so I can use get functions from bmp.h
  // maybe, error is here, but I don't understand why...
  image->raw_data_ = shared_memory_ptr + raw_data_offset;

// cycle for normal work
#ifndef TEST__
  ImageColors local_res{0, 0, 0};
  for (int j = 0; j < n_processes; ++j) {
    local_res += part_clr_prev(image, begins[j], ends[j]);
  }
#endif

  // cycle for testing
  ImageColors local_res{0, 0, 0};
  if (number == 0) {                                 // work wull be done only by first process
    std::cout << "NUMBER: " << number << std::endl;  // output: NUMBER: 0
    for (int j = 0; j < 1; ++j) {
      begins[j] = 0;
      ends[j] = 3000;
      std::cout << "BEG END: " << begins[j] << " " << ends[j] << "\n";  // output: BEG END: 0 3000   -    3000 because my test image is 3000 pixels width
      local_res += part_clr_prev(image, begins[j], ends[j]);            // function call to do the job
      // if there is only 1 process created, everything works correctly => part_clr_prev works correctly
      // but if there 2 or more, I get this error: (even if the second process don't do anything, like in this test case)
      // ==22800==ERROR: AddressSanitizer: access-violation on unknown address 0x0392bb84 (pc 0x000ff5a2 bp 0x005cf910 sp 0x005cf910 T0)
      // ==22800==The signal is caused by a READ memory access.
    }
  }

  set_4b(shared_memory_ptr, result_offset + 3 * number * sizeof(unsigned int), local_res.r_amount_);
  set_4b(shared_memory_ptr, result_offset + 3 * number * sizeof(unsigned int) + sizeof(unsigned int), local_res.g_amount_);
  set_4b(shared_memory_ptr, result_offset + 3 * number * sizeof(unsigned int) + 2 * sizeof(unsigned int), local_res.b_amount_);

  UnmapViewOfFile(shared_memory_ptr);
  CloseHandle(mapping);

  return 0;
}