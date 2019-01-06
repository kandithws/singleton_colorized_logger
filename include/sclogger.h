/*
  MIT License

  Copyright (c) 2018 Kandith Wongsuwan

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */

#pragma once

#if __cplusplus < 201103
#error SCLOGGER requires at least C++11.
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <mutex>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <exception>
#include <algorithm>

#define SCLOG_ANSI_COLOR_RED     "\x1b[31m"
#define SCLOG_ANSI_COLOR_GREEN   "\x1b[32m"
#define SCLOG_ANSI_COLOR_YELLOW  "\x1b[33m"
#define SCLOG_ANSI_COLOR_BLUE    "\x1b[34m"
#define SCLOG_ANSI_COLOR_MAGENTA "\x1b[35m"
#define SCLOG_ANSI_COLOR_CYAN    "\x1b[36m"
#define SCLOG_ANSI_COLOR_RED_BG  "\x1b[41m"
#define SCLOG_ANSI_COLOR_RESET   "\x1b[0m"

// ------------------ FORMAT CUSTOMIZATION -------------------

#define SCLOG_COLOR_DEBUG SCLOG_ANSI_COLOR_CYAN
#define SCLOG_COLOR_INFO SCLOG_ANSI_COLOR_RESET
#define SCLOG_COLOR_SUCCESS SCLOG_ANSI_COLOR_GREEN
#define SCLOG_COLOR_WARN SCLOG_ANSI_COLOR_YELLOW
#define SCLOG_COLOR_DANGER SCLOG_ANSI_COLOR_RED
#define SCLOG_COLOR_ERROR SCLOG_ANSI_COLOR_RED_BG
#define SCLOG_COLOR_ASSERT SCLOG_ANSI_COLOR_RED
// TODO -- timestamp format and output order

#define SCLOG_FORMAT_STRING "[%s][%s]: %s\n" // [STAMP][LEVEL]: MESSAGE\n
#define SCLOG_FORMAT_TAGGED_STRING "[%s][%s][%s]: %s\n" // [STAMP][LEVEL][TAG]: MESSAGE\n

#define SCLOG_VERBOSITY_DEBUG 0
#define SCLOG_VERBOSITY_INFO 1
#define SCLOG_VERBOSITY_SUCCESS 2
#define SCLOG_VERBOSITY_WARN 3
#define SCLOG_VERBOSITY_DANGER 4
#define SCLOG_VERBOSITY_ERROR 5
#define SCLOG_VERBOSITY_ASSERT 6

#define SCLOG_STDCOUT_COLOR(VERBOSITY_COLOR, MESSAGE) \
printf("%s%s" SCLOG_ANSI_COLOR_RESET, VERBOSITY_COLOR, MESSAGE)

namespace sclogger {
/*
 * Initialization Interface
 *
 * */
void init(); // only to stdout
void init(std::string outfile, bool append = false, bool to_stdout = true);
void assert(bool exp, const char* exp_str, const char* file, int line, const char* message);

/*--------------USER INTERFACE MACRO-----------------*/
#define SCLOG_SET_VERBOSITY(LEVEL) Logger::getInstance().setVerbosityLevel(LEVEL)
#define SCLOG_GET_VERBOSITY() Logger::getInstance().getVerbosityLevel()
#define SCLOG_DEBUG(MSG, ...) Logger::getInstance().log(SCLOG_VERBOSITY_DEBUG, MSG, ##__VA_ARGS__)
#define SCLOG_DEBUG_TAGGED(TAG, MSG, ...) Logger::getInstance().logTagged(SCLOG_VERBOSITY_DEBUG, TAG, MSG, ##__VA_ARGS__)
#define SCLOG_INFO(MSG, ...) Logger::getInstance().log(SCLOG_VERBOSITY_INFO, MSG, ##__VA_ARGS__)
#define SCLOG_INFO_TAGGED(TAG, MSG, ...) Logger::getInstance().logTagged(SCLOG_VERBOSITY_INFO, TAG, MSG, ##__VA_ARGS__)
#define SCLOG_SUCCESS(MSG, ...) Logger::getInstance().log(SCLOG_VERBOSITY_SUCCESS, MSG, ##__VA_ARGS__)
#define SCLOG_SUCCESS_TAGGED(TAG, MSG, ...) Logger::getInstance().logTagged(SCLOG_VERBOSITY_SUCCESS, TAG, MSG, ##__VA_ARGS__)
#define SCLOG_WARN(MSG, ...) Logger::getInstance().log(SCLOG_VERBOSITY_WARN, MSG, ##__VA_ARGS__)
#define SCLOG_WARN_TAGGED(TAG, MSG, ...) Logger::getInstance().logTagged(SCLOG_VERBOSITY_WARN, TAG, MSG, ##__VA_ARGS__)
#define SCLOG_DANGER(MSG, ...) Logger::getInstance().log(SCLOG_VERBOSITY_DANGER, MSG, ##__VA_ARGS__)
#define SCLOG_DANGER_TAGGED(TAG, MSG, ...) Logger::getInstance().logTagged(SCLOG_VERBOSITY_DANGER, TAG, MSG, ##__VA_ARGS__)
#define SCLOG_ERROR(MSG, ...) Logger::getInstance().log(SCLOG_VERBOSITY_ERROR, MSG, ##__VA_ARGS__)
#define SCLOG_ERROR_TAGGED(TAG, MSG, ...) Logger::getInstance().logTagged(SCLOG_VERBOSITY_ERROR, TAG, MSG, ##__VA_ARGS__)
#define SCLOG_ASSERT(EXP, MSG, ...) assert(EXP, #EXP,\
 __FILE__, __LINE__, sclogger::Logger::eval_msg(MSG, ##__VA_ARGS__).c_str() )

/*
 *  IMPLEMENTATION
 *
 */

class Logger {
  public:
    static Logger &getInstance();
    void setOutputFile(std::string outfile, bool append = false);
    void setToSTDOUT(bool st) { to_stdout_ = st; }
    void setVerbosityLevel(int level) { verbosity_level_ = level; }
    void setVerbosityLevel(std::string level) { verbosity_level_ = getVerbosityFromString(level); }
    int getVerbosityLevel() const { return verbosity_level_; }
    void closeLogfile();
    void init();

    template<typename T, typename... Args>
    void logTagged(int level, const char *tag, const char *fstr, const T &x, Args... args) throw();

    void logTagged(int level, const char *tag, const char *fstr) throw();

    template<typename T, typename... Args>
    void log(int level, const char *fstr, const T &x, Args... args) throw();

    void log(int level, const char *fstr);

    template<typename T, typename... Args>
    static std::ostream &mprintf(std::ostream &ostr,
                          const char *fstr, const T &x) throw();

    template<typename T, typename... Args>
    static std::ostream &mprintf(std::ostream &ostr,
                          const char *fstr, const T &x, Args... args) throw();

    template<typename T, typename... Args>
    static std::string eval_msg(const char *fstr, const T &x, Args... args) throw();

    //for overloading purpose on pre-processors macro
    static std::string eval_msg(const char *fstr) { return std::string(fstr); }

    static std::string getTimeStamp();

  private:
    Logger();
    ~Logger();
    int verbosity_level_ = -1;
    std::ofstream log_file_;
    std::mutex log_mutex_;
    bool to_stdout_ = true;

    std::string verbosity_map_[7] = {"DEBUG", "INFO", "SUCCESS", "WARN", "DANGER", "ERROR", "ASSERT"};

    int getVerbosityFromString(std::string st);

    const char *getVerbosity(const int &level) const { return verbosity_map_[level].c_str(); }

    void writeLog(int level, const char *fstr);
};


// ------------------ USER INTERFACE -----------------------


void init() {
  Logger::getInstance().init();
}

void init(std::string outfile, bool append, bool to_stdout) {
  Logger::getInstance().setOutputFile(outfile);
  Logger::getInstance().setToSTDOUT(to_stdout);
  init();
}



//----------------------------------------------------------------
Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

Logger::Logger() {}

Logger::~Logger() {
  closeLogfile();
}

void Logger::closeLogfile() {
  if (log_file_)
    log_file_.close();
}

int Logger::getVerbosityFromString(std::string st) {
  auto it = std::find(std::begin(verbosity_map_), std::end(verbosity_map_), st);

  if (it != std::end(verbosity_map_)) {
    // FOUND
    return static_cast<int >(std::distance(std::begin(verbosity_map_), it));
  } else {
    // NOT FOUND -- could be set as integer level
    if ((st.length() == 1) && (st.find_first_not_of("0123456789") == std::string::npos))
      return std::stoi(st);
    else {
      return -1;
    }
  }
}

void Logger::init() {
  if (verbosity_level_ > 0)
    return;

  std::string env = std::getenv("SCLOG_VERBOSITY_LEVEL") ? std::getenv("SCLOG_VERBOSITY_LEVEL") : "";
  if (!env.empty()) {
    verbosity_level_ = getVerbosityFromString(env);
  }

  if (verbosity_level_ < 0) { //default value
    verbosity_level_ = SCLOG_VERBOSITY_INFO;
  }
}

void Logger::setOutputFile(std::string outfile, bool append) {
  if (!log_file_.is_open()) {
    if (append)
      log_file_.open(outfile, std::ios_base::app);
    else
      log_file_.open(outfile);
  } else {
    throw std::runtime_error("Logfile has already been initialized");
  }
}

template<typename T, typename... Args>
std::ostream &Logger::mprintf(std::ostream &ostr,
                              const char *fstr, const T &x) throw() {
  size_t i = 0;
  char c = fstr[0];

  while (c != '%') {
    if (c == 0) return ostr; // string is finished
    ostr << c;
    c = fstr[++i];
  };
  c = fstr[++i];
  ostr << x;

  if (c == 0) return ostr; //

  // print the rest of the stirng
  ostr << &fstr[++i];
  return ostr;
}

template<typename T, typename... Args>
std::ostream &Logger::mprintf(std::ostream &ostr,
                              const char *fstr, const T &x, Args... args) throw() {
  size_t i = 0;
  char c = fstr[0];

  while (c != '%') {
    if (c == 0) return ostr; // string is finished
    ostr << c;
    c = fstr[++i];
  };
  c = fstr[++i];
  ostr << x;

  if (c == 0) return ostr; // string is finished

  return mprintf(ostr, &fstr[++i], args...);
}

template<typename T, typename... Args>
std::string Logger::eval_msg(const char *fstr, const T &x, Args... args) throw() {
  std::stringstream ss;
  mprintf(ss, fstr, x, args...);
  return ss.str();
}

std::string Logger::getTimeStamp() {
  return std::to_string(static_cast<uint64_t >(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  ).count()));
}

template<typename T, typename... Args>
void Logger::logTagged(int level, const char *tag, const char *fstr, const T &x, Args... args) throw() {
  logTagged(level, tag, eval_msg(fstr, x, args...).c_str());
};

void Logger::logTagged(int level, const char *tag, const char *fstr) throw() {
  std::stringstream outmsg;
  mprintf(outmsg, SCLOG_FORMAT_TAGGED_STRING, getTimeStamp(), getVerbosity(level), tag, fstr);
  writeLog(level, outmsg.str().c_str());
}

template<typename T, typename... Args>
void Logger::log(int level, const char *fstr, const T &x, Args... args) throw() {
  log(level, eval_msg(fstr, x, args...).c_str());
}

void Logger::log(int level, const char *fstr) {
  std::stringstream outmsg;
  mprintf(outmsg, SCLOG_FORMAT_STRING, getTimeStamp().c_str(), getVerbosity(level), fstr);
  writeLog(level, outmsg.str().c_str());
}

void Logger::writeLog(int level, const char *msg) {
  if (verbosity_level_ < 0)
    throw std::runtime_error("Verbosity level is not set, or init() has not been called");

  if (level < verbosity_level_)
    return;

  std::lock_guard<std::mutex> lock(log_mutex_);

  if (to_stdout_) {
    switch (level) {
      case SCLOG_VERBOSITY_DEBUG: SCLOG_STDCOUT_COLOR(SCLOG_COLOR_DEBUG, msg);
        break;
      case SCLOG_VERBOSITY_INFO: SCLOG_STDCOUT_COLOR(SCLOG_COLOR_INFO, msg);
        break;
      case SCLOG_VERBOSITY_SUCCESS: SCLOG_STDCOUT_COLOR(SCLOG_COLOR_SUCCESS, msg);
        break;
      case SCLOG_VERBOSITY_WARN: SCLOG_STDCOUT_COLOR(SCLOG_COLOR_WARN, msg);
        break;
      case SCLOG_VERBOSITY_DANGER: SCLOG_STDCOUT_COLOR(SCLOG_COLOR_DANGER, msg);
        break;
      case SCLOG_VERBOSITY_ERROR: SCLOG_STDCOUT_COLOR(SCLOG_COLOR_ERROR, msg);
        break;
      case SCLOG_VERBOSITY_ASSERT: // No print here only to log
        break;
      default: std::cout << msg;
        break;
    }
  }

  if (log_file_.is_open()) {
    log_file_ << msg;
  }
}

/* Log assertion. credit: https://www.softwariness.com/articles/assertions-in-cpp/ */
class AssertionFailureException : public std::exception {
  private:
    const char *expression;
    const char *file;
    int line;
    std::string message;
    std::string report;

  public:

    /// Construct an assertion failure exception
    AssertionFailureException(const char *expression, const char *file, int line, const std::string &message)
        : expression(expression), file(file), line(line), message(message) {
      std::ostringstream outputStream;

      if (!message.empty()) {
        outputStream << message << " \n ";
      }

      std::string expressionString = expression;

      if (expressionString == "false" || expressionString == "0" || expressionString == "FALSE") {
        outputStream << "Unreachable code assertion";
      } else {
        outputStream << "Assertion '" << expression << "'";
      }

      outputStream << " failed in file '" << file << "', line " << line;
      std::string tmp_report = outputStream.str();
      Logger::getInstance().log(SCLOG_VERBOSITY_ASSERT, tmp_report.c_str());
      Logger::getInstance().closeLogfile();
      outputStream << ", Timestamp " << Logger::getTimeStamp();
      report = outputStream.str();
    }

    /// The assertion message
    virtual const char *what() const throw() {
      return report.c_str();
    }

    /// The expression which was asserted to be true
    const char *Expression() const throw() {
      return expression;
    }

    /// Source file
    const char *File() const throw() {
      return file;
    }

    /// Source line
    int Line() const throw() {
      return line;
    }

    /// Description of failure
    const char *Message() const throw() {
      return message.c_str();
    }

    ~AssertionFailureException() throw() {

    }
};

void assert(bool exp, const char* exp_str, const char* file, int line, const char* msg){
  if(!exp)
    throw AssertionFailureException(exp_str, file, line, msg);
}

}

