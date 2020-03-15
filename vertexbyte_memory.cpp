#if VERTEXBYTE_SLOW
#define assert(expression) if(!expression){ *(u8 *)0 = 0;}
#else
#define assert(expression) 
#endif

#define kilobytes(n) (n*1024ULL)
#define megabytes(n) (kilobytes(n)*1024ULL)
#define gigabytes(n) (megabytes(n)*1024ULL)
#define terabytes(n) (gigabytes(n)*1024ULL)

struct Game_Memory
{
  u64 permanent_memory_size;
  u64 transient_memory_size;
  u64 total_size;
  void *permanent_memory;
  void *transient_memory;
};

