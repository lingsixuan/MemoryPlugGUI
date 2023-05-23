//
// Created by ling on 23-5-23.
//

#include "Window/Data/TargetData.h"

namespace ling {
    TargetData::TargetData(const std::string &name, const std::string &ip) {
        this->name = name;
        this->ip = ip;
    }

    TargetData::~TargetData() {

    }

    const std::string &TargetData::getName() const {
        return this->name;
    }

    const std::string &TargetData::getIP() const {
        return this->ip;
    }
} // ling