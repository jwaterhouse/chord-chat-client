#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <spdlog/spdlog.h>

// Logging configuration
#define LOG_IDENT           "chord"
#define LOG_NAME            LOG_IDENT
#define LOG_FILE            "chord_log"
#define LOG_SIZE            1024 * 1024 * 5       // 5 megabytes
#define LOG_NUM             3
#define LOG_SYSLOG_PATTERN  spdlog::set_pattern("[%l] %v");
#define LOG_INIT_SYSLOG     spdlog::syslog_logger(LOG_NAME, LOG_IDENT, LOG_PID);
#define LOG_INIT_FILE       spdlog::rotating_logger_mt(LOG_NAME, LOG_FILE, LOG_SIZE, LOG_NUM);
#define LOG_LEVEL_DEBUG     spdlog::set_level(spdlog::level::debug);
#define LOG_LEVEL_INFO      spdlog::set_level(spdlog::level::info);
#ifdef DEBUG
#define LOG_INIT_LEVEL      spdlog::set_level(spdlog::level::debug);
#else
#define LOG_INIT_LEVEL      spdlog::set_level(spdlog::level::info);
#endif
#ifdef __linux__
#define LOG_INIT            LOG_INIT_LEVEL LOG_INIT_SYSLOG LOG_SYSLOG_PATTERN
#else
#define LOG_INIT            LOG_INIT_LEVEL LOG_INIT_FILE
#endif
#define LOG                 spdlog::get(LOG_NAME)

#endif  // __LOGGER_H__
