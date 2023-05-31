#ifndef LYNX_CORE_HPP
#define LYNX_CORE_HPP

#if !defined(HAS_DEBUG_LOG_TOOLS) && !defined(HAS_DEBUG_MACROS)
#define HAS_DEBUG_MACROS
#define DBG_SET_LEVEL(...)
#define DBG_SET_PATTERN(...)

#define DBG_TRACE(...)
#define DBG_DEBUG(...)
#define DBG_INFO(...)
#define DBG_WARN(...)
#define DBG_ERROR(...)
#define DBG_CRITICAL(...)
#define DBG_FATAL(...)

#define DBG_ASSERT_TRACE(...)
#define DBG_ASSERT_DEBUG(...)
#define DBG_ASSERT_INFO(...)
#define DBG_ASSERT_WARN(...)
#define DBG_ASSERT_ERROR(...)
#define DBG_ASSERT_CRITICAL(...)
#define DBG_ASSERT_FATAL(...)
#endif

#if !defined(HAS_PROFILE_TOOLS) && !defined(HAS_PROFILE_MACROS)
#define HAS_PROFILE_MACROS
#define PERF_BEGIN_SESSION(...)
#define PERF_END_SESSION(...)
#define PERF_SCOPE(...)
#define PERF_FUNCTION(...)
#define PERF_PRETTY_FUNCTION(...)
#define PERF_SET_MAX_FILE_MB(...)
#define PERF_SET_EXTENSION(...)
#endif

#ifdef HAS_ALLOCATORS
#include "mem/stack_allocator.hpp"
#include "mem/block_allocator.hpp"

#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif

#include <vector>

namespace lynx
{
template <typename T> using stk_vector = std::vector<T, mem::stack_allocator<T>>;

template <typename T> using blk_vector = std::vector<T, mem::block_allocator<T>>;

template <typename T> using scope = std::unique_ptr<T, mem::block_deleter<T>>;

template <typename T> using ref = std::shared_ptr<T>;

template <typename T, class... Args> inline scope<T> make_scope(Args &&...args)
{
    mem::block_allocator<T> alloc; // I dont think static is even worth it
    T *buff = alloc.allocate_raw(sizeof(T));
    if (!buff)
        return scope<T>(new T(std::forward<Args>(args)...));

    T *p = new (buff) T(std::forward<Args>(args)...);
    DBG_ASSERT_WARN((std::uint64_t)p % alignof(T) == 0, "Block allocated pointer {0} is not aligned! Alignment: {1}",
                    (void *)p, alignof(T))
    return scope<T>(p);
}

template <typename T, class... Args> inline ref<T> make_ref(Args &&...args)
{
    static mem::block_allocator<T> alloc;
    T *buff = alloc.allocate_raw(sizeof(T));
    if (!buff)
        return std::make_shared<T>(std::forward<Args>(args)...);

    T *p = new (buff) T(std::forward<Args>(args)...);
    DBG_ASSERT_WARN((std::uint64_t)p % alignof(T) == 0, "Block allocated pointer {0} is not aligned! Alignment: {1}",
                    (void *)p, alignof(T))
    return ref<T>(p, mem::block_deleter<T>());
}
} // namespace lynx
#else

namespace lynx
{
template <typename T> using stk_vector = std::vector<T>;

template <typename T> using blk_vector = std::vector<T>;

template <typename T> using scope = std::unique_ptr<T>;

template <typename T> using ref = std::shared_ptr<T>;

template <typename T, class... Args> inline scope<T> make_scope(Args &&...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T, class... Args> inline ref<T> make_ref(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
} // namespace lynx

#endif

#endif