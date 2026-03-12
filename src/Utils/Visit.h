#pragma once

namespace BadgerEngine {

template<class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

}
