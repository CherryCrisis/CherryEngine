#pragma once

#include <any>
#include <typeindex>

namespace CCProperty
{
    struct IClearProperty
    {
        virtual ~IClearProperty() = default;

        virtual bool Set(const void* in) = 0;
        virtual bool Get(void* out) const = 0;

        const std::type_index& GetType() { return m_type; }

    protected:
        std::type_index m_type;

        IClearProperty(const std::type_index& type) : m_type(type) { }
    };

    template<typename OwnerT>
    struct IProperty : IClearProperty
    {
        virtual ~IProperty() = default;

        virtual OwnerT& SetAny(const std::any& value) = 0;
        virtual std::any GetAny() const = 0;

    protected:
        OwnerT* const m_ownerPtr = nullptr;

        IProperty(OwnerT* const ownerPtr, const std::type_index& type) : IClearProperty(type), m_ownerPtr(ownerPtr) { }
    };

    template<typename OwnerT, typename ObjectT>
    struct Property : IProperty<OwnerT>
    {
        virtual ~Property() = default;

        using SetType = void (OwnerT::*)(const ObjectT&);
        using GetType = ObjectT(OwnerT::*)();

        Property(OwnerT* ownerPtr, SetType setter, GetType getter)
            : IProperty<OwnerT>(ownerPtr, typeid(ObjectT)), m_setter(setter), m_getter(getter) { }
        
#pragma region Setter
        OwnerT& Set(const ObjectT& value)
        {
            (IProperty<OwnerT>::m_ownerPtr->*m_setter)(value);
            return *IProperty<OwnerT>::m_ownerPtr;
        }

        bool Set(const OwnerT* in)
        {
            if (!in || !m_setter)
                return false;

            Set(*in);
            return true;
        }

        bool Set(const void* in) override
        {
            return Set(static_cast<const ObjectT*>(in));
        }

        OwnerT& SetAny(const std::any& value) override
        {
            const ObjectT* objPtr = std::any_cast<ObjectT>(&value);
            const void* abstractPtr = static_cast<const void*>(objPtr);

            Set(abstractPtr);
            return *IProperty<OwnerT>::m_ownerPtr;
        }

        OwnerT& operator=(const std::any& value)
        {
            return SetAny(value);
        }

        OwnerT& operator=(const ObjectT& value)
        {
            return Set(value);
        }
#pragma endregion

#pragma region Getter
        bool Get(ObjectT* out) const
        {
            if (!out || !m_setter)
                return false;

            *out = (IProperty<OwnerT>::m_ownerPtr->*m_getter)();
            return true;
        }

        bool Get(void* out) const override
        {
            return Get(static_cast<ObjectT*>(out));
        }

        std::any GetAny() const override { return std::any((IProperty<OwnerT>::m_ownerPtr->*m_getter)()); }
        operator std::any() const { return GetAny(); }

        explicit operator ObjectT() const { return std::any_cast<ObjectT>(GetAny()); }
#pragma endregion

    protected:
        SetType const m_setter = nullptr;
        GetType const m_getter = nullptr;
    };
}
