#pragma once
///@file

#include "bsd/util/types.hh"

namespace bsd {

class AutoRemoveJail
{
    int jid;
    bool del;
public:
    AutoRemoveJail(int jid);
    AutoRemoveJail();
    ~AutoRemoveJail();
    void cancel();
    void reset(int j);
};

}
