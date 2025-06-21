#ifndef _WIN32

#  include "bsd/util/util.hh"
#  include "bsd/util/monitor-fd.hh"

#  include <sys/file.h>
#  include <gtest/gtest.h>

namespace bsd {
TEST(MonitorFdHup, shouldNotBlock)
{
    Pipe p;
    p.create();
    {
        // when monitor gets destroyed it should cancel the
        // background thread and do not block
        MonitorFdHup monitor(p.readSide.get());
    }
}
}

#endif
