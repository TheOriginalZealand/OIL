#include <assert.h>

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
     data_[len_++] = item_; \
     })
#endif
