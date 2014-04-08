/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
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

/**
 * @file
 *
 * Collects information about available libraries
 */

#ifndef SCOREP_CONFIG_ADAPTER_HPP
#define SCOREP_CONFIG_ADAPTER_HPP

#include <string>
#include <deque>
#include <stdint.h>

#include "SCOREP_Config_LibraryDependencies.hpp"
#include "scorep_config_types.hpp"

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
     * Calls for all adapters the addLibs() member functions.
     */
    static void
    addLibsAll( std::deque<std::string>&           libs,
                SCOREP_Config_LibraryDependencies& deps );

    /**
     * Calls for all adapters the addCFlags() member functions.
     */
    static void
    addCFlagsAll( std::string&           cflags,
                  bool                   build_check,
                  SCOREP_Config_Language language,
                  bool                   nvcc );

    /**
     * Calls for all adapters the addIncFlags() member functions.
     */
    static void
    addIncFlagsAll( std::string& incflags,
                    bool         build_check,
                    bool         nvcc );

    /**
     * Calls for all adapters the addLdFlags() member functions.
     */
    static void
    addLdFlagsAll( std::string& ldflags,
                   bool         nvcc );

    /**
     * Constructs a SCOREP_Config_Adapter.
     * @param name        The name of the adapter, as displayed in the help output.
     * @param library     The name of the library which contains the adapter. Please
     *                    omit the leading 'lib' which is prefixed automatically
     *                    if needed.
     * @param is_default  True, if this adapter is enabled by default.
     */
    SCOREP_Config_Adapter( std::string name,
                           std::string library,
                           bool        is_default );

    /**
     * Destructs the adapter.
     */
    virtual
    ~SCOREP_Config_Adapter();

    /**
     * Appends the name of the SCOREP_Subsystem struct associated with
     * this adapter to @a init_strucrs. The default implementation
     * appends 'SCOREP_Subsystem_<name>Adapter' Where the first letter of
     * <name> is made uppercase. Overwrite this function of your subsystem
     * name differs or if your subsystem shouldn't show up in
     * scorep_subsystems.
     */
    virtual void
    appendInitStructName( std::deque<std::string>& init_structs );

    /**
     * Inserts all initialization struct names into ta queue.
     */
    static void
    getAllInitStructNames( std::deque<std::string>& init_structs );

protected:
    /**
     * Prints standart help output for this adapter, based on the name.
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
     * @param cflgas       The compiler flags to which you may modify or add new flags.
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
     * @param nvcc         True if compiler is nvcc.
     */
    virtual void
    addIncFlags( std::string& incflags,
                 bool         build_check,
                 bool         nvcc );

    /**
     * Overwrite this function if you want to do adapter specific modifications
     * to the linker flags.
     * @param ldflags  the linker flags to which you may modify or add new flags.
     * @param nvcc     True if compiler is nvcc.
     */
    virtual void
    addLdFlags( std::string& ldflags,
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
 * class SCOREP_CompilerAdapter
 * *************************************************************************************/

/**
 * This class represents the compiler adapter.
 */
class SCOREP_Config_CompilerAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_CompilerAdapter();
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );
    virtual void
    addLdFlags( std::string& ldflags,
                bool         nvcc );
};

/* **************************************************************************************
 * class SCOREP_UserAdapter
 * *************************************************************************************/

/**
 * This class represents the user adapter.
 */
class SCOREP_Config_UserAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_UserAdapter();
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );
};

/* **************************************************************************************
 * class SCOREP_CudaAdapter
 * *************************************************************************************/

/**
 * This class represents the cuda adapter.
 */
class SCOREP_Config_CudaAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_CudaAdapter();
    virtual bool
    checkArgument( const std::string& arg );
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
};

/* **************************************************************************************
 * class SCOREP_PompAdapter
 * *************************************************************************************/
class SCOREP_Config_PompAdapter : public SCOREP_Config_Adapter
{
public:
    SCOREP_Config_PompAdapter();
    virtual void
    addIncFlags( std::string& incflags,
                 bool         build_check,
                 bool         nvcc );
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
 * class SCOREP_PreprocessAdapter
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

#endif
