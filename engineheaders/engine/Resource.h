#pragma once

#include <atomic>

namespace Engine {
    template<typename T>
    struct Resource {
        std::atomic_bool hasbegun = false; // Used for checking whether this is the first reference that's created
        std::atomic_int16_t referencecount = 0;

        virtual bool IncrementReference() {
            bool expected = false;
            bool desired = true;
            if (hasbegun.compare_exchange_strong(expected, desired)) {
                referencecount++;
                return true;
            }
            if (referencecount <= 0)
                return false;
            referencecount++;
            return true;
        }
        virtual void DecrementReference() {
            if (--referencecount == 0) {
                delete this;
            }
        }

        virtual T* Get() = 0;
        virtual const T* Get() const = 0;

        virtual ~Resource() {}
    };


    template<typename T>
    struct Reference {
        Reference(Engine::Resource<T>* resource) : Resource(resource) { if (!Resource->IncrementReference()) Resource = nullptr; }
        Reference() { Resource = nullptr; }
        Reference(const Reference& other) { Resource = other.Resource; if (Resource) Resource->IncrementReference(); }
        Reference(Reference&& other) noexcept { Resource = other.Resource; other.Resource = nullptr; }
        Reference& operator=(const Reference& other) { Resource = other.Resource; if (Resource) Resource->IncrementReference(); return *this; }
        Reference& operator=(Reference&& other) noexcept { Resource = other.Resource; other.Resource = nullptr; return *this; }


        ~Reference() { if (Resource) Resource->DecrementReference(); }

        T& operator*() { return *Resource->Get(); }
        T* operator->() { return Resource->Get(); }
        const T& operator*() const { return *Resource->Get(); }
        const T* operator->() const { return Resource->Get(); }

        operator bool() const { return Resource; }
        bool operator==(decltype(nullptr)) const { return Resource == nullptr; }
        bool operator==(const Reference& other) const { return Resource == other.Resource; }
        void Reset() { if (Resource) Resource->DecrementReference(); Resource = nullptr; }

        T* get() { return Resource->Get(); }
    private:
        Engine::Resource<T>* Resource;
    };



    template<typename Container, typename T>
    struct ManagedResource : Resource<T> {
        Container* container;
        T resource;
        
        template <typename... Args>
        ManagedResource(Container* in, Args&&... args) : resource(std::forward<Args>(args)...), container(in) {}

        void DecrementReference() {
            if (--Resource<T>::referencecount == 0) {
                container->_unmanage_resource(this);
            }
        }

        T* Get() { return &resource; }
        const T* Get() const { return &resource; }
    };

    template<typename T>
    struct UnmanagedResource : Resource<T> {
        T resource;
        
        template <typename... Args>
        UnmanagedResource(Args&&... args) : resource(std::forward<Args>(args)...) {}

        T* Get() { return &resource; }
        const T* Get() const { return &resource; }
    };

    template<typename Container, typename Interface, typename T>
    struct ManagedInterfacedResource : Resource<Interface> {
        Container* container;
        T resource;
        
        template <typename... Args>
        ManagedInterfacedResource(Container* in, Args&&... args) : resource(std::forward<Args>(args)...), container(in) {}

        void DecrementReference() {
            if (--Resource<Interface>::referencecount == 0) {
                container->_unmanage_resource(this);
            }
        }

        Interface* Get() { return &resource; }
        const Interface* Get() const { return &resource; }
    };

    template<typename Interface, typename T>
    struct UnmanagedInterfacedResource : Resource<Interface> {
        T resource;
        
        template <typename... Args>
        UnmanagedInterfacedResource(Args&&... args) : resource(std::forward<Args>(args)...) {}

        Interface* Get() { return &resource; }
        const Interface* Get() const { return &resource; }
    };
}