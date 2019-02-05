#ifndef _THREADS_HPP
#define _THREADS_HPP

/// \file Threads.hpp
///
/// \brief Header file for thread functionalities

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#ifdef USE_THREADS
 #include <threads/ThreadPool.hpp>
#else
 #include <threads/FakePool.hpp>
#endif

#endif
