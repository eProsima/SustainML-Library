%{
#include <functional>
#include "sustainml_cpp/core/NodeImpl.hpp"
%}

// Expose the stop task functionality
%extend sustainml::core::NodeImpl {
    void register_stop_task_callback(PyObject* callback) {
        if (callback && PyCallable_Check(callback)) {
            Py_INCREF(callback);

            auto cpp_callback = [callback](const TaskIdImpl& task_id) {
                PyGILState_STATE gstate = PyGILState_Ensure();

                try {
                    // Create Python TaskId object using SWIG conversion
                    PyObject* py_task_id = SWIG_NewPointerObj(
                        const_cast<TaskIdImpl*>(&task_id),
                        SWIGTYPE_p_TaskIdImpl,
                        0
                    );

                    PyObject* result = PyObject_CallFunctionObjArgs(callback, py_task_id, NULL);

                    if (result) {
                        Py_DECREF(result);
                    } else {
                        PyErr_Print();
                    }

                    Py_DECREF(py_task_id);
                } catch (...) {
                    PyErr_Print();
                }

                PyGILState_Release(gstate);
            };

            $self->register_stop_task_callback(cpp_callback);
        }
    }
}

%extend sustainml::core::MLModelNode {
    void register_stop_task_callback(PyObject* callback) {
        if (callback && PyCallable_Check(callback)) {
            this->get_node_impl()->register_stop_task_callback(callback);
        }
    }
}
