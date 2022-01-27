#include <iostream>
#include "amf0_ecma_array.hpp"
#include "amf0_object.hpp"
using namespace mms;

Json::Value Amf0EcmaArray::toJson() {
    Json::Value root;
    for (auto & p : properties_) {
        switch(p.second->getType()) {
            case NUMBER_MARKER: {
                root[p.first] = ((Amf0Number*)p.second)->getValue();
                break;
            }
            case BOOLEAN_MARKER: {
                root[p.first] = ((Amf0Boolean*)p.second)->getValue();
                break;
            }
            case STRING_MARKER: {
                root[p.first] = ((Amf0String*)p.second)->getValue();
                break;
            }
            case OBJECT_MARKER: {
                root[p.first] = ((Amf0Object*)p.second)->toJson();
                break;
            }
            case NULL_MARKER: {
                root[p.first] = nullptr;
                break;
            }
            case UNDEFINED_MARKER: {
                root[p.first] = "undefined";
                break;
            }
            default : {
                
            }
        }
    }
    return root;
}