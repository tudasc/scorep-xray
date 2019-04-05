/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_ADAPTER_HPP
#define SCOREP_CONFIG_ADAPTER_HPP

/**
 * @file
 *
 * Collects information about available libraries
 */

#include <string>
#include <deque>
#include <set>
#include <vector>
#include <stdint.h>

#include "SCOREP_Config_LibraryDependencies.hpp"
#include "scorep_config_types.hpp"

class SCOREP_Config_LibwrapAdapter;

/* **************************************************************************************
 * class SCOREP_Config_Adapter
 * *************************************************************************************/

/**
 * The class SCOREP_Config_Adapter represents a adapter inside the config tool.
 * It contains the adapter specific data and algorithms for the config
 * tool output. This class is the basis from which specialized adapters
 * may derive their own class.
 */
class SCOREP_Config_Adapter
{
public:

    /**
     * Initializes the adapter list.
     */
    static void
    init( void );

    /**
     * Destroys the adapter list.
     */
    static void
    fini( void );

    /**
     * Prints for all adapters the help message.
     */
    static void
    printAll( void );

    /**
     * Checks all adapters whether an program argument influences it.
     */
    static bool
    checkAll( const std::string& arg );

    /**
     * Calls for all enabled adapters the addLibs() member functions.
     */
    static void
    addLibsAll( std::deque<std::string>&           libs,
                SCOREP_Config_LibraryDependencies& deps );

    /**
     * Calls for all enabled adapters the addCFlags() member functions.
     */
    static void
    addCFlagsAll( std::string&           cflags,
                  bool                   build_check,
                  SCOREP_Config_Language language,
                  bool                   nvcc );

    /**
     * Calls for all enabled adapters the addIncFlags() member functions.
     */
    static void
    addIncFlagsAll( std::string&           incflags,
                    bool                   build_check,
                    SCOREP_Config_Language language,
                    bool                   nvcc );

    /**
     * Calls for all adapters the addLdFlags() member functions.
     */
    static void
    addLdFlagsAll( std::string& ldflags,
                   bool         build_check,
                   bool         nvcc );

    /**
     * Inserts for all enabled adapters the initialization struct names into a queue.
     */
    static void
    getAllInitStructNames( std::deque<std::string>& init_structs );

    /**
     * Constructs a SCOREP_Config_Adapter.
     * @param name        The name of the adapter, as displayed in the help output.
     * @param library     The name of the library which contains the adapter. Please
     *                    omit the leading 'lib' which is prefixed automatically
     *                    if needed.
     * @param is_default  True, if this adapter is enabled by default.
     */
    SCOREP_Config_Adapter( const std::string& name,
                           const std::string& library,
                           bool               is_default );

    /**
     * Destructs the adapter.
     */
    virtual
    ~SCOREP_Config_Adapter();

    /**
     * Appends the name of the SCOREP_Subsystem struct associated with
     * this adapter to @a init_structs. The default implementation
     * appends 'SCOREP_Subsystem_<name>Adapter' Where the first letter of
     * <name> is made uppercase. Overwrite this function of your subsystem
     * name differs or if your subsystem shouldn't show up in
     * scorep_subsystems.
     */
    virtual void
    appendInitStructName( std::deque<std::string>& init_structs );

    /**
     * Returns true if at least one adapter is active.
     */
    static inline bool
    isActive()
    {
        return !all.empty();
    }

protected:
    /**
     * Prints standard help output for this adapter, based on the name.
     * Overwrite this functions if you need a different layout.
     */
    virtual void
    printHelp( void );

    /**
     * Checks whether an pogram argument influences this adapter.
     * This implementation checks for the --adapter/--noadapter flags and
     * enables/disables the adapter. Overwrite this function if you want to
     * check for other arguments.
     * @param arg  The argument which is checked.
     * @returns True if this argument is known to this adapter. False otherwise.
     */
    virtual bool
    checkArgument( const std::string& arg );

    /**
     * Adds the adapter library to the list of libraries. This implementation
     * adds the value stored in m_library. Overwrite this function if you need
     * more or different libraries.
     * @param libs The list of libs to which you may add other libs.
     * @param deps The library dependency class.
     */
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    /**
     * Overwrite this function if you want to do adapter specific modifications
     * to the compiler flags. This function is also called, when the addCFlags()
     * function will be called.
     * @param cflags       The compiler flags to which you may modify or add new flags.
     *                     This flags do not contain the include directories. For the
     *                     include flags use addIncFlags.
     * @param build_check  True '--build-check' was specified.
     * @param language     Specifies whether it is a C, C++ or Fortran compiler.
     * @param nvcc         True if compiler is nvcc.
     */
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );

    /**
     * Overwrite this function if you want to do adapter specific modifications
     * to the include flags.
     * @param incflags  The include flags to which you may modify or add new flags.
     *                  This flags contain only the include directories. For other
     *                  compiler flags use addCFlags.
     * @param build_check  True '--build-check' was specified.
     * @param language     Specifies whether it is a C, C++ or Fortran compiler.
     * @param nvcc         True if compiler is nvcc.
     */
    virtual void
    addIncFlags( std::string&           incflags,
                 bool                   build_check,
                 SCOREP_Config_Language language,
                 bool                   nvcc );

    /**
     * Overwrite this function if you want to do adapter specific modifications
     * to the linker flags.
     * @param ldflags     The linker flags to which you may modify or add new flags.
     * @param build_check True '--build-check' was specified.
     * @param nvcc        True if compiler is nvcc.
     */
    virtual void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );

    /**
     * Stores whether this adapter is currently enabled.
     */
    bool m_is_enabled;

    /**
     * The adapter name.
     */
    std::string m_name;

    /**
     * The library name.
     */
    std::string m_library;

private:
    /**
     * List of available adapters
     */
    static std::deque<SCOREP_Config_Adapter*> all;
};

/* **************************************************************************************
 * class SCOREP_Config_MockupAdapter
 * *************************************************************************************/

/**
 * This class represents a not supported adapter by this installation.
 */
class SCOREP_Config_MockupAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_MockupAdapter( const std::string& name );

    virtual void
    printHelp( void );

    virtual bool
    checkArgument( const std::string& arg );

    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    virtual void
    appendInitStructName( std::deque<std::string>& init_structs );
};

/* **************************************************************************************
 * class SCOREP_Config_CompilerAdapter
 * *************************************************************************************/

/**
 * This class represents the compiler adapter.
 */
class SCOREP_Config_CompilerAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_CompilerAdapter();
    virtual bool
    checkArgument( const std::string& arg );
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );
    virtual void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );

#if HAVE_BACKEND( GCC_PLUGIN_SUPPORT )
private:
    /** Any additional cflags */
    std::string m_cflags;
#endif
};

/* **************************************************************************************
 * class SCOREP_Config_UserAdapter
 * *************************************************************************************/

/**
 * This class represents the user adapter.
 */
class SCOREP_Config_UserAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_UserAdapter();
    virtual void
    addIncFlags( std::string&           cflags,
                 bool                   build_check,
                 SCOREP_Config_Language language,
                 bool                   nvcc );
};

/* **************************************************************************************
 * class SCOREP_Config_CudaAdapter
 * *************************************************************************************/

/**
 * This class represents the cuda adapter.
 */
class SCOREP_Config_CudaAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_CudaAdapter();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
};

/* **************************************************************************************
 * class SCOREP_Config_OpenaccAdapter
 * *************************************************************************************/

/**
 * This class represents the OpenACC adapter.
 */
class SCOREP_Config_OpenaccAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_OpenaccAdapter();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
};

/* **************************************************************************************
 * class SCOREP_Config_OpenclAdapter
 * *************************************************************************************/

/**
 * This class represents the OpenCL adapter.
 */
class SCOREP_Config_OpenclAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_OpenclAdapter();
    virtual void
    printHelp( void );
    virtual bool
    checkArgument( const std::string& arg );
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
    virtual void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );

private:
    std::string m_wrapmode;
};

/* **************************************************************************************
 * class SCOREP_Config_Opari2Adapter
 * *************************************************************************************/
class SCOREP_Config_Opari2Adapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_Opari2Adapter();
    virtual void
    addIncFlags( std::string&           incflags,
                 bool                   build_check,
                 SCOREP_Config_Language language,
                 bool                   nvcc );
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );

    virtual void
    appendInitStructName( std::deque<std::string>& init_structs );

    static void
    printOpariCFlags( bool                   build_check,
                      bool                   with_cflags,
                      SCOREP_Config_Language language,
                      bool                   nvcc );
};

/* **************************************************************************************
 * class SCOREP_Config_PreprocessAdapter
 * *************************************************************************************/
class SCOREP_Config_PreprocessAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_PreprocessAdapter();

    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    virtual void
    appendInitStructName( std::deque<std::string>& init_structs );
};

/* **************************************************************************************
 * class SCOREP_Config_MemoryAdapter
 * *************************************************************************************/

/**
 * This class represents the memory adapter.
 */
class SCOREP_Config_MemoryAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_MemoryAdapter();

    virtual void
    printHelp( void );

    virtual bool
    checkArgument( const std::string& arg );

    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    virtual void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );

private:
    std::set<std::string> m_categories;
};

/* **************************************************************************************
 * class SCOREP_Config_LibwrapAdapter
 * *************************************************************************************/

/**
 * This class represents the compiler adapter.
 */
class SCOREP_Config_LibwrapAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_LibwrapAdapter();

    void
    printHelp( void );

    bool
    checkArgument( const std::string& arg );

    void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );

    void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    void
    appendInitStructName( std::deque<std::string>& init_structs );

private:
    std::set<std::pair<std::string, std::string> > m_wrappers;
};

/* **************************************************************************************
 * class SCOREP_IoAdapter
 * *************************************************************************************/

/**
 * This class represents the I/O wrapping adapter.
 */
class SCOREP_Config_IoAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_IoAdapter();

    virtual void
    printHelp( void );

    virtual bool
    checkArgument( const std::string& arg );

    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    virtual void
    addLdFlags( std::string& ldflags,
                bool         buildCheck,
                bool         nvcc );

    virtual void
    appendInitStructName( std::deque<std::string>& initStructs );

private:
    struct SCOREP_Config_SupportedIo
    {
        SCOREP_Config_SupportedIo( const std::string& subsystem,
                                   const std::string& lib,
                                   const std::string& wrap )
            :   m_subsystem_name( subsystem )
            ,   m_lib_name( lib )
        {
            m_wrap_names.push_back( wrap );
        }

        SCOREP_Config_SupportedIo( const std::string& subsystem,
                                   const std::string& lib )
            :   m_subsystem_name( subsystem )
            ,   m_lib_name( lib )
        {
            m_wrap_names.push_back( lib );
        }

        SCOREP_Config_SupportedIo( const SCOREP_Config_SupportedIo& other )
            :   m_subsystem_name( other.m_subsystem_name )
            ,   m_lib_name( other.m_lib_name )
            ,   m_wrap_names( other.m_wrap_names )
        {
        }

        SCOREP_Config_SupportedIo()
        {
        }

        void
        add_wrap_name( const std::string& wrap )
        {
            m_wrap_names.push_back( wrap );
        }

        std::string              m_subsystem_name;
        std::string              m_lib_name;
        std::vector<std::string> m_wrap_names;
    };

    typedef std::map<std::string, SCOREP_Config_SupportedIo> SCOREP_Config_SupportedIosT;
    typedef SCOREP_Config_SupportedIosT::const_iterator      SCOREP_Config_SupportedIosCIT;
    typedef SCOREP_Config_SupportedIosT::value_type          SCOREP_Config_SupportedIosV;

    SCOREP_Config_SupportedIosT        m_supported_ios;
    std::map<std::string, std::string> m_selected_ios;
};

#endif // SCOREP_CONFIG_ADAPTER_HPP
