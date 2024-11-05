#ifndef MACROS_H_
#define MACROS_H_

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef CLAMP
#define CLAMP(x, low, high) \
    (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif
#define INTFLOOR(x) (int)x
#define INTCEIL(x) ((int)x) + 1

// get the size of a statically allocated c array
#define c_array_len(V) (sizeof(V) / sizeof((V)[0]))

#define degToRad(angleInDegrees) ((angleInDegrees) * CONST_PI / 180.0f)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0f / CONST_PI)

#define wrapDegrees(degrees)     \
    degrees >= 0                 \
        ? fmodf(degrees, 360.0f) \
        : fmodf(degrees + 360.0f, 360.0f)

#ifdef N64
#include <stdlib.h>
#define normRandom() ((float)rand() / 1073741823.0f)
#else
#define RAND(x) (rand() % (x)) /* random number between 0 to x */
#define normRandom() ((float)rand() / (float)RAND_MAX)
#endif

#ifdef N64
#include <libdragon.h>
#define CUR_TIME_MS() TIMER_MICROS_LL(timer_ticks()) / 1000.0f
#else
#define CUR_TIME_MS() #error "CUR_TIME_MS() not implemented"
#endif

#ifndef N64
#include <stdio.h>
#define debugf printf
#endif

#ifdef N64
#define die(msg) assertf(FALSE, "died: \"%s\" in %s at %s:%d\n", msg, __FUNCTION__, __FILE__, __LINE__);
#else
#include <stdio.h>
#include <assert.h>
#define die(msg)                                                                             \
    fprintf(stderr, "died: \"%s\" in %s at %s:%d\n", msg, __FUNCTION__, __FILE__, __LINE__); \
    assert(FALSE);
#endif

#define beacon() debugf("at %d in %s\n", __LINE__, __FUNCTION__);

#ifdef N64
#define invariant(expression) assertf(expression, "invariant violated: %s", #expression)
#else
#include <assert.h>
#define invariant(expression) assert(expression)
#endif

#endif // MACROS_H_