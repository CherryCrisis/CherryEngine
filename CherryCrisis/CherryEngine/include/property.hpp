#pragma once

template<typename OwnerT, typename ObjectT>
struct Property
{
    using setType = void (OwnerT::*)(const ObjectT&);
    using getType = ObjectT(OwnerT::*)();

    Property(OwnerT* ownerPtr, setType setter, getType getter)
        : ownerPtr(ownerPtr), setter(setter), getter(getter)
    { }

    operator ObjectT() const
    {
        return (ownerPtr->*getter)();
    }

    OwnerT& operator=(const ObjectT& value)
    {
        (ownerPtr->*setter)(value);
        return *ownerPtr;
    }

protected:
    OwnerT* const ownerPtr;
    setType const setter;
    getType const getter;
};