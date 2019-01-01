# singleton_colorized_logger
A simple singleton "colorized" printf-style C++11 logger in a single header file
## Features

* Can be either log through cout console or log file (with append options) or both
* Set verbosity level through *SLOG_VERBOSITY_LEVEL* env or API
* Tagging
* Thread safe

### Usage

Requires to call `sclogger::init(...)` once before logging. See example.cpp

Only to console

`sclogger::init(); // only to stdout`

Also to log file

`sclogger::init(std::string outfile, bool append = false, bool to_stdout = true);`
### Verbosity configuration
In console

`export SCLOG_VERBOSITY_LEVEL=(DEBUG|INFO|SUCCESS|WARN|DANGER|ERROR)`

or

In code

`sclogger::SCLOG_SET_VERBOSITY("(DEBUG|INFO|SUCCESS|WARN|DANGER|ERROR)");` 

## EXAMPLE OUTPUT
![Example Output](/sample_console_output.png)

