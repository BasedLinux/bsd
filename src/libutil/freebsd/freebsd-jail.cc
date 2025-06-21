#ifdef __FreeBSD__
#  include "bsd/util/freebsd-jail.hh"

#  include <sys/resource.h>
#  include <sys/param.h>
#  include <sys/jail.h>
#  include <sys/mount.h>

#  include "bsd/util/error.hh"
#  include "bsd/util/util.hh"

namespace bsd {

AutoRemoveJail::AutoRemoveJail()
    : del{false}
{
}

AutoRemoveJail::AutoRemoveJail(int jid)
    : jid(jid)
    , del(true)
{
}

AutoRemoveJail::~AutoRemoveJail()
{
    try {
        if (del) {
            if (jail_remove(jid) < 0) {
                throw SysError("Failed to remove jail %1%", jid);
            }
        }
    } catch (...) {
        ignoreExceptionInDestructor();
    }
}

void AutoRemoveJail::cancel()
{
    del = false;
}

void AutoRemoveJail::reset(int j)
{
    del = true;
    jid = j;
}

//////////////////////////////////////////////////////////////////////

}
#endif
