#pragma once
#include <memory>

class Counter
{
public:
    Counter() = default;
    Counter(Counter const &counter);
    Counter(Counter &&counter) = default;
    Counter &operator=(Counter counter);

    uint32_t getCount() const;

    virtual ~Counter();
protected:
    std::shared_ptr<uint32_t> mCount = std::make_shared<uint32_t>(1);
};
