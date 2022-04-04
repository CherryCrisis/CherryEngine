#pragma once

#include <any>
#include <typeindex>

namespace CCProperty
{
    struct IClearProperty
    {
        virtual ~IClearProperty() = default;

        virtual bool Set(void* in) = 0;
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

        virtual OwnerT& SetAny(std::any& value) = 0;
        virtual std::any GetAny() const = 0;

    protected:
        OwnerT* const m_ownerPtr = nullptr;

        IProperty(OwnerT* const ownerPtr, const std::type_index& type) : IClearProperty(type), m_ownerPtr(ownerPtr) { }
    };

    template<typename OwnerT, typename ObjectT>
    struct Property : IProperty<OwnerT>
    {
        virtual ~Property() = default;

        using SetType = void (OwnerT::*)(ObjectT&);
        using GetType = ObjectT(OwnerT::*)();

        Property(OwnerT* ownerPtr, SetType setter, GetType getter)
            : IProperty<OwnerT>(ownerPtr, typeid(ObjectT)), m_setter(setter), m_getter(getter) { }
        
#pragma region Setter
        OwnerT& Set(ObjectT& value)
        {
            (IProperty<OwnerT>::m_ownerPtr->*m_setter)(value);
            return *IProperty<OwnerT>::m_ownerPtr;
        }

        bool Set(ObjectT* in)
        {
            if (!in || !m_setter)
                return false;

            Set(*in);
            return true;
        }

        bool Set(void* in) override
        {
            ObjectT* objPtr = static_cast<ObjectT*>(in);
            return Set(objPtr);
        }

        OwnerT& SetAny(std::any& value) override
        {
            ObjectT* objPtr = std::any_cast<ObjectT>(&value);
            void* abstractPtr = static_cast<void*>(objPtr);

            Set(abstractPtr);
            return *IProperty<OwnerT>::m_ownerPtr;
        }

        OwnerT& operator=(std::any& value)
        {
            return SetAny(value);
        }

        OwnerT& operator=(ObjectT& value)
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
