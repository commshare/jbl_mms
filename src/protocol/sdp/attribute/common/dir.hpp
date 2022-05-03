#pragma once
#include <string>
#include <string>
#include <vector>
namespace mms
{
    struct DirAttr
    {
    public:
        enum MEDIA_DIR {
            MEDIA_SENDONLY = 0,
            MEDIA_RECVONLY = 1,
            MEDIA_SENDRECV = 2,
        };
    public:
        static bool isMyPrefix(const std::string & line);
        bool parse(const std::string &line);
        std::string toString() const;

        void setDir(MEDIA_DIR val) {
            dir_ = val;
        }

        MEDIA_DIR getDir() {
            return dir_;
        }
    private:
        MEDIA_DIR dir_;
    };
};