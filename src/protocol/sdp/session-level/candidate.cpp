#include <vector>
#include <string>
#include "base/utils/utils.h"

#include "candidate.h"

using namespace mms;
std::string Candidate::prefix = "a=candidate:";
bool Candidate::parse(const std::string &line)
{
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos)
    {
        end_pos = line.size() - 1;
    }
    std::string valid_string = line.substr(prefix.size(), end_pos);
    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() < 8)
    {
        return false;
    }
    foundation = vs[0];
    component_id = std::atoi(vs[1].c_str());
    transport = vs[2];
    priority = std::atoi(vs[3].c_str());
    address = vs[4];
    port = std::atoi(vs[5].c_str());
    std::string t = vs[6]; // typ
    if (vs[7] == "host")
    {
        cand_type = CAND_TYPE_HOST;
    }
    else if (vs[7] == "srflx")
    {
        cand_type = CAND_TYPE_SRFLX;
    }
    else if (vs[7] == "prflx")
    {
        cand_type = CAND_TYPE_PRFLX;
    }
    else if (vs[7] == "relay")
    {
        cand_type = CAND_TYPE_RELAY;
    }

    return true;
}
