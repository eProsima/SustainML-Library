#include <sustainml_cpp/core/SamplesQueue.hpp>

namespace sustainml {

    template <typename T>
    SamplesQueue<T>::SamplesQueue(
            Node* node) :
            node_(node)
    {
        //! TODO
    }

    template <typename T>
    SamplesQueue<T>::~SamplesQueue()
    {
        //! TODO
    }

    template <typename T>
    void SamplesQueue<T>::insert_element(const std::shared_ptr<T> &elem)
    {
        //! TODO
    }

    template <typename T>
    void SamplesQueue<T>::remove_element_by_taskid(const int& id)
    {
        //! TODO
    }

    template <typename T>
    std::shared_ptr<T> SamplesQueue<T>::get_input_sample_from_taskid(const int &id)
    {
        //! TODO
    }

} // namespace sustainml
