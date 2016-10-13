#include "counter.hpp"

Counter::Counter(const Counter &counter) :
    mCount(counter.mCount) {
    ++(*mCount);
}

Counter &Counter::operator =(Counter counter) {
    using std::swap;
    swap(mCount, counter.mCount);
    return *this;
}

uint32_t Counter::getCount() const {
    return *mCount;
}

Counter::~Counter() {

}
