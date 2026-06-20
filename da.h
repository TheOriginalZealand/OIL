// this is the code for a dynamic array

#include <assert.h>
#include <stdlib.h>

#ifndef da_h
#define da_h

#define da_append(data_, len_, cap_, item_) \
    ({ \
     if (len_ + 1 > cap_) { \
         cap_ *= 2; \
         void *data__ = realloc(data_, cap_ * sizeof(item_)); \
         assert(data__ != NULL); \
         data_ = data__; \
     } \
     ((typeof(item_) *) data_)[len_++] = item_; \
     })

#define da2_append(data_, len_, cap_, item_) \
    ({ \
     if (len_ > cap_) { \
         cap_ *= 2; \
         void *data__ = realloc(data_, cap_ * sizeof(item_)); \
         assert(data__ != NULL); \
         data_ = data__; \
     } \
     ((typeof(item_) *) data_)[len_ - 1] = item_; \
     })

#endif
