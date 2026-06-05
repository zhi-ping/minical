#include "flag_table.hpp"

void FlagTable::register_flag(const FlagDescriptor& desc) {
    flags_.push_back(desc);
}

const FlagDescriptor* FlagTable::find(const std::string& token) const {
    for (const auto& desc : flags_) {
        if (desc.flag == token) {
            return &desc;
        }
    }
    return nullptr;
}
