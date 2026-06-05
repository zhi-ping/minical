#include "flag_descriptor.hpp"

FlagDescriptor::FlagDescriptor(std::string flag_name,
                               ArgKind kind,
                               std::function<void(CalOptions&, const std::string&)> apply_fn)
    : flag(std::move(flag_name))
    , arg_kind(kind)
    , apply(std::move(apply_fn))
{}
