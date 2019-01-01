#include <sclogger.h>


int main() {
  sclogger::init("test.log");
  //sclogger::SCLOG_SET_VERBOSITY("DEBUG");
  sclogger::SCLOG_DEBUG("HELLOWORLD from Logging %d", 1+2);
  sclogger::SCLOG_DEBUG_TAGGED("MYTAG","HELLOWORLD from Logging %d %s", 4+5, "debug");
  sclogger::SCLOG_INFO("HELLOWORLD from Logging %d", 1+2);
  sclogger::SCLOG_INFO_TAGGED("MYTAG","HELLOWORLD from Logging %d %s", 4+5, "info");
  sclogger::SCLOG_SUCCESS("HELLOWORLD from Logging %d", 1+2);
  sclogger::SCLOG_SUCCESS_TAGGED("MYTAG","HELLOWORLD from Logging %d %s", 4+5, "success");
  sclogger::SCLOG_WARN("HELLOWORLD from Logging %d", 1+2);
  sclogger::SCLOG_WARN_TAGGED("MYTAG","HELLOWORLD from Logging %d %s", 4+5, "warn");
  sclogger::SCLOG_DANGER("HELLOWORLD from Logging %d", 1+2);
  sclogger::SCLOG_DANGER_TAGGED("MYTAG","HELLOWORLD from Logging %d %s", 4+5, "danger");
  sclogger::SCLOG_ERROR("HELLOWORLD from Logging %d", 1+2);
  sclogger::SCLOG_ERROR_TAGGED("MYTAG","HELLOWORLD from Logging %d %s", 4+5, "error");
  return 0;
}
