#include "logconfig.h"

#include <iomanip>

std::string getFullSeverity(const char &severity) {
    switch (severity) {
        case 'I':
            return "INFO";
        case 'W':
            return "WARNING";
        case 'E':
            return "ERROR";
        case 'F':
            return "FATAL";
        default:
            return "INFO";
    }
}

void customPrefix(std::ostream &s, const google::LogMessageInfo &l, void *) {
    s << '['
      << getFullSeverity(l.severity[0])   // Prints INFO, WARNING, etc.
      << ": "
      << std::setw(4) << 1900 + l.time.year() << '-'
      << std::setw(2) << 1 + l.time.month() << '-'
      << std::setw(2) << l.time.day()
      << 'T'
      << std::setw(2) << l.time.hour() << ':'
      << std::setw(2) << l.time.min() << ':'
      << std::setw(2) << l.time.sec() 
      << "Z "
      << l.filename << ':' << l.line_number << "]"
      << "  ";
}
