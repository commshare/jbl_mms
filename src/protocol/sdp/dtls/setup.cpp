#include "setup.h"
#include <sstream>
#include "base/utils/utils.h"
using namespace mms;

// Setup Attribute

//    The 'setup' attribute indicates which of the end points should
//    initiate the TCP connection establishment (i.e., send the initial TCP
//    SYN).  The 'setup' attribute is charset-independent and can be a
//    session-level or a media-level attribute.  The following is the ABNF
//    of the 'setup' attribute:

//          setup-attr           =  "a=setup:" role
//          role                 =  "active" / "passive" / "actpass"
//                                  / "holdconn"

//       'active': The endpoint will initiate an outgoing connection.
//       'passive': The endpoint will accept an incoming connection.
//       'actpass': The endpoint is willing to accept an incoming
//       connection or to initiate an outgoing connection.
//       'holdconn': The endpoint does not want the connection to be
//       established for the time being.

#define ROLE_ACTIVE      "active"
#define ROLE_PASSIVE     "passive"
#define ROLE_ACTPASS     "actpass"
#define ROLD_HOLDCONN    "holdconn"

std::string SetupAttr::prefix = "a=setup:";
bool SetupAttr::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    role = line.substr(prefix.size(), end_pos);
    return true;
}

std::string SetupAttr::toString() const {
    std::ostringstream oss;
    oss << prefix << role << std::endl;
    return oss.str();
}