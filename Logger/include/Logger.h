//
// Created by nickberryman on 15/11/25.
//

#ifndef SLOWGRID_LOGGER_H
#define SLOWGRID_LOGGER_H
#include <stdexcept>
#include <string>

#ifndef NDEBUG
#define LOGGER_ASSERT_ERROR(x) \
{\
bool b = false; \
try { x } \
catch (std::runtime_error& e) {  b = true; } \
Logging::assert_except(b);\
}
#else
#define LOGGER_ASSERT_ERROR(x)
#endif


#ifndef NDEBUG
#define LOGGER_ASSERT_EXCEPT(x) Logging::assert_except(x);
#else
#define LOGGER_ASSERT_EXCEPT(x)
#endif

#ifndef NDEBUG
#define LOGGER_ASSERT_ABORT(x) Logging::assert::abort(x);
#else
#define LOGGER_ASSERT_ABORT(x)
#endif

#ifndef NDEBUG
#define LOGGER_ASSERT_LOG(x, y) Logging::assert_log(x,y);
#else
#define LOGGER_ASSERT_LOG(x)
#endif



#define LOGGER_TRACE std::string( __FILE__ ) \
    + std::string( ":" ) \
    + std::to_string( __LINE__ ) \
    + std::string( " in " ) \
    + std::string( __func__ )

#endif //SLOWGRID_LOGGER_H
