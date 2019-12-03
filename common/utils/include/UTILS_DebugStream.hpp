/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef UTILS_DEBUG_H
#error "Do not include this header directly, use UTILS_Debug.h instead."
#endif


#ifndef UTILS_DEBUGSTREAM_HPP
#define UTILS_DEBUGSTREAM_HPP

/**
 * @file
 * @ingroup UTILS_Exception_module
 * @brief   Module for debug output handling in UTILS using C++ streams
 *
 * This header file provides the definition of preprocessor macros and helper
 * classes to allow printing debug messages using C++ output streams.  It is
 * not intended to be included directly, but only via <tt>UTILS_Debug.h</tt>.
 */

#include <string>
#include <sstream>

/**
 * @def     UTILS_DLOG
 * @ingroup UTILS_Exception_module
 * @brief   Debug logging stream for the default debug level
 *
 * The UTILS_DLOG preprocessor macro provides a C++ output stream for
 * printing debug messages.  It uses the default debug level specified
 * by the ${PACKAGE}_DEBUG_MODULE_NAME preprocessor define.  (To specify
 * a debug level explicitly, use the UTILS_DLOG_LEVEL macro instead.)
 *
 * @par Example:
 * @code
 *     #include <config.h>
 *
 *     #define EXAMPLE_DEBUG_MODULE_NAME CORE
 *     #include <UTILS_Debug.h>
 *
 *     int main( int argc, char** argv )
 *     {
 *         UTILS_DLOG << "Entering main() with argc=" << argc;
 *         return 0;
 *     }
 * @endcode
 *
 * @see UTILS_DLOG_LEVEL
 */
/**
 * @def     UTILS_DLOG_LEVEL
 * @ingroup UTILS_Exception_module
 * @brief   Debug logging stream for a specific debug level
 *
 * The UTILS_DLOG_LEVEL preprocessor macro provides a C++ output stream for
 * printing debug messages of a specific debug level.
 *
 * @par Example:
 * @code
 *     #include <config.h>
 *
 *     #include <UTILS_Debug.h>
 *
 *     int main( int argc, char** argv )
 *     {
 *         UTILS_DLOG_LEVEL( EXAMPLE_DEBUG_CORE )
 *             << "Entering main() with argc=" << argc;
 *         return 0;
 *     }
 * @endcode
 *
 * @see UTILS_DLOG
 */

#if HAVE( UTILS_DEBUG )

/* *INDENT-OFF* */

#define UTILS_DLOG \
    AFS_PACKAGE_name::utils::DebugStream( \
        UTILS_JOIN_SYMS( AFS_PACKAGE_NAME, \
                UTILS_JOIN_SYMS( _DEBUG_, \
                        PACKAGE_MANGLE_NAME( DEBUG_MODULE_NAME ) ) ), \
        __FILE__, \
        __LINE__, \
        UTILS_FUNCTION_NAME )

#define UTILS_DLOG_LEVEL( debugLevel ) \
    AFS_PACKAGE_name::utils::DebugStream( \
        debugLevel, \
        __FILE__, \
        __LINE__, \
        UTILS_FUNCTION_NAME )

/* *INDENT-ON* */


namespace AFS_PACKAGE_name
{
namespace utils
{
/**
 * @class   DebugStream
 * @ingroup UTILS_Exception_module
 * @brief   Helper class to concatenate C++ stream debug output
 *
 * This helper class buffers the C++ stream debugging output in a temporary
 * output string stream and flushes it during destruction to the default
 * debug output using UTILS_Debug_Printf().
 */
class DebugStream
{
public:
    /**
     * @brief Constructor
     *
     * Creates a new instance and stores the given parameters in member
     * variables.
     *
     * @param bitMask   Debug level bit mask
     * @param file      Source file name
     * @param line      Source line number
     * @param function  Function name
     */
    DebugStream( uint64_t    bitMask,
                 const char* file,
                 uint64_t    line,
                 const char* function )
        : m_bit_mask( bitMask ),
        m_file( file ),
        m_line( line ),
        m_function( function )
    {
    }

    /**
     * @brief Destructor
     *
     * Destroys the instance and prints the concatenated debugging output.
     */
    ~DebugStream()
    {
        UTILS_Debug_Printf( m_bit_mask,
                            AFS_PACKAGE_SRCDIR,
                            m_file.c_str(),
                            m_line,
                            m_function.c_str(),
                            "%s",
                            m_buffer.str().c_str() );
    }

    /**
     * @brief Output operator
     *
     * Appends the textual representation of the given @a item to the internal
     * buffer using the value's stream output operator.
     *
     * @param   item  Item to be printed
     * @return  Reference to the instance
     */
    template <typename T>
    DebugStream&
    operator<<( const T& item )
    {
        m_buffer << item;
        return *this;
    }


private:
    uint64_t           m_bit_mask; /**< Debug level bit mask */
    std::string        m_file;     /**< Source file name */
    uint64_t           m_line;     /**< Source line number */
    std::string        m_function; /**< Function name */
    std::ostringstream m_buffer;   /**< Message construction buffer */
};
}                                  /* namespace utils */
}                                  /* namespace AFS_PACKAGE_name */

#else /* !HAVE( UTILS_DEBUG ) */

/*
 * The release versions of UTILS_DLOG and UTILS_DLOG_LEVEL rely on dead code
 * elimination by the compiler, as there is no good way to #ifdef the stream
 * expression itself.  In practice this works quite well with all compilers
 * if a decent optimization level is used (GNU/Cray/IBM: O0; Intel/Fujitsu: O1;
 * PGI: O2).
 */

#define UTILS_DLOG \
    if ( 0 ) \
        AFS_PACKAGE_name::utils::DebugStream()

#define UTILS_DLOG_LEVEL( debugLevel ) \
    if ( 0 ) \
        AFS_PACKAGE_name::utils::DebugStream()


namespace AFS_PACKAGE_name
{
namespace utils
{
/*
 * "Do nothing" version of class DebugStream used in release mode.
 */
class DebugStream
{
public:
    DebugStream()
    {
    }

    ~DebugStream()
    {
    }

    template <typename T>
    DebugStream&
    operator<<( const T& item )
    {
        return *this;
    }
};
}    /* namespace utils */
}    /* namespace AFS_PACKAGE_name */

#endif /* !HAVE( UTILS_DEBUG ) */

#endif /* UTILS_DEBUGSTREAM_HPP */
