# Regenerate types instructions

The design of the SustainML CPP library follows a structure where the public types exposed does not contain any Fast DDS types in public includes.
In that way, exposing Fast DDS complex types is avoided, which makes SWIG python wrapper generation way more simple.
To reach that, the public types contain implementation types, which are the originally generated by Fast DDS Gen with the idl (see that all idl elements have the `Impl` suffix).
The next steps needs to be followed to do so while trying to regenerate SustainML types:

1. Update .idl file
1. Regenerate types with utils script

    `cd sustainml_cpp/utils/scripts && bash update_types.sh`

1. Manually introduce the `sustainml_cpp/src/cpp/types/typesImpl.hpp` changes in `sustainml_cpp/include/sustainml_cpp/types/types.hpp`, and applying these changes:
    1. Copy the complete class `ClassNameImpl` in the _types.hpp_ file
    1. Rename it as `ClassName` (remove `Impl` suffix), as well as all its usage in its methods and comments.
    1. Remove all code from the methods, leaving only their definitions.
    1. Introduce the following line right after the _public_ declaration:

        `using impl_type = ClassNameImpl;`

    1. Include at the end of the _public_ declarations the following _impl_ retrieve methods:


        ```cpp
            /*!
             * @brief This function returns the implementation
             * @return Pointer to implementation
             */
            ClassNameImpl* get_impl();

            /*!
             * @brief This function retrives the implementation type info
             * @return Reference to the typeid
             */
            static const std::type_info& impl_typeinfo();
        ```

    1. Substitute _private_ declaration with the variables, for a _protected_ declaration as the following:

        ```cpp
            protected:

                ClassNameImpl* impl_;
                friend class ClassNameImpl;
        ```
    1. Please review the existing classes in the library and ensure that the new one follows the same style.

1. Manually introduce the `sustainml_cpp/src/cpp/types/typesImpl.hpp` changes in `sustainml_cpp/src/cpp/types/types.cpp`, and applying these changes:
    1. Copy the complete class `ClassNameImpl` in the _types.cpp_ file
    1. Rename it as `ClassName` (remove `Impl` suffix), as well as all its usage in its methods and comments.
    1. Make all methods to call the corresponding `ClassNameImpl` method. That applies also to operators, constructor and destructor. The following snippet is an example, but for any other case, please review the already implemented classes to mimic:

        ```cpp
            ClassName& ClassName::operator =(
                    const ClassName& x)
            {
                this->impl_->class_variable() = x.impl_->class_variable();
                return *this;
            }

            void ClassName::variable_name(
                    const std::string& __variable_name_)
            {
                impl_->variable_name(__variable_name_);
            }

            void ClassName::variable_name(
                    std::string&& __variable_name_)
            {
                impl_->variable_name(std::forward<std::string>(__variable_name_));
            }

            std::string& ClassName::variable_name()
            {
                return impl_->variable_name();
            }
        ```

    1. At the very end of each class, implement the _impl_ retrieve methods included in the _types.h_ file:

        ```cpp
            ClassNameImpl* ClassName::get_impl()
            {
                return impl_;
            }

            const std::type_info& ClassName::impl_typeinfo()
            {
                return typeid(ClassNameImpl);
            }
        ```
