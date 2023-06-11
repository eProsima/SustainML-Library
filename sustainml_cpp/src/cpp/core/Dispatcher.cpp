#include <sustainml_cpp/core/Dispatcher.hpp>

namespace sustainml {

    Dispatcher::Dispatcher(Node *node) :
        node_(node),
        stop_(false),
        thread_pool_(N_THREADS_DEFAULT)
    {
        //! TODO
    }

    Dispatcher::~Dispatcher()
    {
        //! TODO
    }

    void Dispatcher::start()
    {
        //! TODO
    }

    void Dispatcher::stop()
    {
        //! TODO
    }

    void Dispatcher::register_retriever(SamplesQueryable* sr)
    {
        //! TODO
    }

    void Dispatcher::notify(const int &task_id)
    {
        //! TODO
    }

    void Dispatcher::process(const int& task_id)
    {
        //! TODO
    }

    void Dispatcher::routine()
    {
        //! TODO
    }

} // namespace sustainml
