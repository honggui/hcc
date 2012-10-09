// RUN: %amp_device -D__GPU__=1 %s -m32 -emit-llvm -c -S -O3 -o %t.ll 
// RUN: mkdir -p %t
// RUN: %llc -march=c -o %t/kernel.cl < %t.ll
// RUN: pushd %t 
// RUN: objcopy -B i386:x86-64 -I binary -O elf64-x86-64 kernel.cl %t/kernel.o
// RUN: popd
// RUN: %cxxamp %link %t/kernel.o %s -o %t.out && %t.out

#include <iostream> 
#include <amp.h> 
using namespace concurrency; 
int main() 
{
  int v[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  extent<2> e(5, 2);

  array_view<int, 2> av(e, v); 
  assert(av.extent == e);
  // Testing tiled_index
  parallel_for_each(av.extent.tile<1,2>(),
    [=](tiled_index<1,2> idx) restrict(amp) { 
    av[idx] -= 1; 
  });
  assert(av.extent == e);
  for(unsigned int i = 0; i < av.extent[0]; i++)
    for(unsigned int j = 0; j < av.extent[1]; j++)
      assert(i*2+j == static_cast<char>(av(i, j)));
  return 0;
}
