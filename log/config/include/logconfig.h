#include <string>
#include <ostream>

#include <glog/logging.h>

std::string getFullSeverity(const google::LogMessageInfo &l);

void customPrefix(std::ostream &s, const google::LogMessageInfo &l, void *);
