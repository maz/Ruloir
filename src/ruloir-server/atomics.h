#ifndef ATOMICS_H
#define ATOMICS_H

#define CompareAndSwap(ptr, old, new)	__sync_val_compare_and_swap(ptr, old, new)
#define FetchAndAdd(ptr, val)	__sync_fetch_and_add(ptr, val)

#endif