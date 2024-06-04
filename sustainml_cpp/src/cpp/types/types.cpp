// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file types.cpp
 *
 */

#include <sustainml_cpp/types/types.h>

#include <common/Common.hpp>
#include <types/typesImpl.h>

namespace types {

TaskId::TaskId()
    : problem_id_(sustainml::common::INVALID_ID)
    , iteration_id_(sustainml::common::INVALID_ID)
{
}

TaskId::TaskId(
        uint32_t problem_id,
        uint32_t iteration_id)
    : problem_id_(problem_id)
    , iteration_id_(iteration_id)
{
}

TaskId::~TaskId() = default;

TaskId::TaskId(
        const TaskId& x)
{
    problem_id_ = x.problem_id_;
    iteration_id_ = x.iteration_id_;
}

TaskId::TaskId(
        TaskId&& x) noexcept
{
    problem_id_ = x.problem_id_;
    iteration_id_ = x.iteration_id_;
}

TaskId& TaskId::operator =(
        const TaskId& x)
{
    problem_id_ = x.problem_id_;
    iteration_id_ = x.iteration_id_;
    return *this;
}

TaskId& TaskId::operator =(
        TaskId&& x) noexcept
{
    problem_id_ = x.problem_id_;
    iteration_id_ = x.iteration_id_;
    return *this;
}

bool TaskId::operator ==(
        const TaskId& x) const
{

    return (problem_id_ == x.problem_id_ &&
           iteration_id_ == x.iteration_id_);
}

bool TaskId::operator !=(
        const TaskId& x) const
{
    return !(*this == x);
}

bool TaskId::operator <(
        const TaskId& x) const
{
    return (problem_id_ != x.problem_id_ ? problem_id_ < x.problem_id_ : iteration_id_ < x.iteration_id_);
}

void TaskId::problem_id(
        uint32_t _problem_id)
{
    problem_id_ = _problem_id;
}

uint32_t TaskId::problem_id() const
{
    return problem_id_;
}

uint32_t& TaskId::problem_id()
{
    return problem_id_;
}

void TaskId::iteration_id(
        uint32_t _iteration_id)
{
    iteration_id_ = _iteration_id;
}

uint32_t TaskId::iteration_id() const
{
    return iteration_id_;
}

uint32_t& TaskId::iteration_id()
{
    return iteration_id_;
}

std::ostream& operator << (
        std::ostream& stream,
        const TaskId& task_id)
{
    stream << "{" << task_id.problem_id() << "," << task_id.iteration_id() << "}";
    return stream;
}

NodeStatus::NodeStatus()
{
    impl_ = new NodeStatusImpl;
}

NodeStatus::~NodeStatus()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

NodeStatus::NodeStatus(
        const NodeStatus& x)
{
    impl_ = new NodeStatusImpl;

    this->impl_->node_status() = x.impl_->node_status();
    this->impl_->node_name() = x.impl_->node_name();
    this->impl_->task_status() = x.impl_->task_status();
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->error_code() = x.impl_->error_code();
    this->impl_->error_description() = x.impl_->error_description();
}

NodeStatus::NodeStatus(
        NodeStatus&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

NodeStatus& NodeStatus::operator =(
        const NodeStatus& x)
{
    this->impl_->node_status() = x.impl_->node_status();
    this->impl_->node_name() = x.impl_->node_name();
    this->impl_->task_status() = x.impl_->task_status();
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->error_code() = x.impl_->error_code();
    this->impl_->error_description() = x.impl_->error_description();
    return *this;
}

NodeStatus& NodeStatus::operator =(
        NodeStatus&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool NodeStatus::operator ==(
        const NodeStatus& x) const
{
    return (this->impl_ == x.impl_);
}

bool NodeStatus::operator !=(
        const NodeStatus& x) const
{
    return !(*this == x);
}

void NodeStatus::node_status(
        Status _node_status)
{
    impl_->node_status(_node_status);
}

Status NodeStatus::node_status() const
{
    return impl_->node_status();
}

Status& NodeStatus::node_status()
{
    return impl_->node_status();
}

void NodeStatus::task_status(
        TaskStatus _task_status)
{
    impl_->task_status(_task_status);
}

TaskStatus NodeStatus::task_status() const
{
    return impl_->task_status();
}

TaskStatus& NodeStatus::task_status()
{
    return impl_->task_status();
}

void NodeStatus::error_code(
        ErrorCode _error_code)
{
    impl_->error_code(_error_code);
}

ErrorCode NodeStatus::error_code() const
{
    return impl_->error_code();
}

ErrorCode& NodeStatus::error_code()
{
    return impl_->error_code();
}

void NodeStatus::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void NodeStatus::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& NodeStatus::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& NodeStatus::task_id()
{
    return *to_task_id(&impl_->task_id());
}

void NodeStatus::error_description(
        const std::string& _error_description)

{
    impl_->error_description(_error_description);
}

void NodeStatus::error_description(
        std::string&& _error_description)
{
    impl_->error_description(std::forward<std::string>(_error_description));
}

const std::string& NodeStatus::error_description() const
{
    return impl_->error_description();
}

std::string& NodeStatus::error_description()
{
    return impl_->error_description();
}

void NodeStatus::node_name(
        const std::string& _node_name)
{
    impl_->node_name(_node_name);
}

void NodeStatus::node_name(
        std::string&& _node_name)

{
    impl_->node_name(_node_name);
}

const std::string& NodeStatus::node_name() const
{
    return impl_->node_name();
}

std::string& NodeStatus::node_name()
{
    return impl_->node_name();
}

void NodeStatus::update(
        uint32_t new_status)
{
    impl_->node_status((Status)new_status);
}

NodeStatusImpl* NodeStatus::get_impl()
{
    return impl_;
}

void NodeStatus::reset()
{
    impl_->node_status(Status::NODE_IDLE);
    impl_->node_name("");
    impl_->task_status(TaskStatus::TASK_SUCCEEDED);
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
    impl_->error_code(ErrorCode::NO_ERROR);
    impl_->error_description("");
}

NodeControl::NodeControl()
{
    impl_ = new NodeControlImpl;
}

NodeControl::~NodeControl()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

NodeControl::NodeControl(
        const NodeControl& x)
{
    impl_ = new NodeControlImpl;

    this->impl_->cmd_node() = x.impl_->cmd_node();
    this->impl_->source_node() = x.impl_->source_node();
    this->impl_->target_node() = x.impl_->target_node();
    this->impl_->cmd_task() = this->impl_->cmd_task();
    this->impl_->task_id() = x.impl_->task_id();
}

NodeControl::NodeControl(
        NodeControl&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

NodeControl& NodeControl::operator =(
        const NodeControl& x)
{
    this->impl_->cmd_node() = x.impl_->cmd_node();
    this->impl_->source_node() = x.impl_->source_node();
    this->impl_->target_node() = x.impl_->target_node();
    this->impl_->cmd_task() = this->impl_->cmd_task();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

NodeControl& NodeControl::operator =(
        NodeControl&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool NodeControl::operator ==(
        const NodeControl& x) const
{
    return (this->impl_ == x.impl_);
}

bool NodeControl::operator !=(
        const NodeControl& x) const
{
    return !(*this == x);
}

void NodeControl::cmd_node(
        CmdNode _cmd_node)
{
    impl_->cmd_node(_cmd_node);
}

CmdNode NodeControl::cmd_node() const
{
    return impl_->cmd_node();
}

CmdNode& NodeControl::cmd_node()
{
    return impl_->cmd_node();
}

void NodeControl::cmd_task(
        CmdTask _cmd_task)
{
    impl_->cmd_task(_cmd_task);
}

CmdTask NodeControl::cmd_task() const
{
    return impl_->cmd_task();
}

CmdTask& NodeControl::cmd_task()
{
    return impl_->cmd_task();
}

void NodeControl::target_node(
        const std::string& _target_node)
{
    impl_->target_node(_target_node);
}

void NodeControl::target_node(
        std::string&& _target_node)
{
    return impl_->target_node(std::forward<std::string>(_target_node));
}

const std::string& NodeControl::target_node() const
{
    return impl_->target_node();
}

std::string& NodeControl::target_node()
{
    return impl_->target_node();
}

void NodeControl::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void NodeControl::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& NodeControl::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& NodeControl::task_id()
{
    return *to_task_id(&impl_->task_id());
}

void NodeControl::source_node(
        const std::string& _source_node)
{
    impl_->source_node(_source_node);
}

void NodeControl::source_node(
        std::string&& _source_node)
{
    impl_->source_node(std::forward<std::string>(_source_node));
}

const std::string& NodeControl::source_node() const
{
    return impl_->source_node();
}

std::string& NodeControl::source_node()
{
    return impl_->source_node();
}

NodeControlImpl* NodeControl::get_impl() const
{
    return impl_;
}

void NodeControl::reset()
{
    impl_->cmd_node(CmdNode::NO_CMD_NODE);
    impl_->source_node("");
    impl_->target_node("");
    impl_->cmd_task(CmdTask::NO_CMD_TASK);
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

UserInput::UserInput()
{
    impl_ = new UserInputImpl;
}

UserInput::~UserInput()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

UserInput::UserInput(
        const UserInput& x)
{
    impl_ = new UserInputImpl;

    this->impl_->modality() = x.impl_->modality();
    this->impl_->problem_short_description() = x.impl_->problem_short_description();
    this->impl_->problem_definition() = x.impl_->problem_definition();
    this->impl_->inputs() = x.impl_->inputs();
    this->impl_->outputs() = x.impl_->outputs();
    this->impl_->minimum_samples() = x.impl_->minimum_samples();
    this->impl_->maximum_samples() = x.impl_->maximum_samples();
    this->impl_->optimize_carbon_footprint_manual() = x.impl_->optimize_carbon_footprint_manual();
    this->impl_->previous_iteration() = x.impl_->previous_iteration();
    this->impl_->optimize_carbon_footprint_auto() = x.impl_->optimize_carbon_footprint_auto();
    this->impl_->desired_carbon_footprint() = x.impl_->desired_carbon_footprint();
    this->impl_->geo_location_continent() = x.impl_->geo_location_continent();
    this->impl_->geo_location_region() = x.impl_->geo_location_region();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
}

UserInput::UserInput(
        UserInput&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

UserInput& UserInput::operator =(
        const UserInput& x)
{
    this->impl_->modality() = x.impl_->modality();
    this->impl_->problem_short_description() = x.impl_->problem_short_description();
    this->impl_->problem_definition() = x.impl_->problem_definition();
    this->impl_->inputs() = x.impl_->inputs();
    this->impl_->outputs() = x.impl_->outputs();
    this->impl_->minimum_samples() = x.impl_->minimum_samples();
    this->impl_->maximum_samples() = x.impl_->maximum_samples();
    this->impl_->optimize_carbon_footprint_manual() = x.impl_->optimize_carbon_footprint_manual();
    this->impl_->previous_iteration() = x.impl_->previous_iteration();
    this->impl_->optimize_carbon_footprint_auto() = x.impl_->optimize_carbon_footprint_auto();
    this->impl_->desired_carbon_footprint() = x.impl_->desired_carbon_footprint();
    this->impl_->geo_location_continent() = x.impl_->geo_location_continent();
    this->impl_->geo_location_region() = x.impl_->geo_location_region();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();

    return *this;
}

UserInput& UserInput::operator =(
        UserInput&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool UserInput::operator ==(
        const UserInput& x) const
{
    return (this->impl_ == x.impl_);
}

bool UserInput::operator !=(
        const UserInput& x) const
{
    return !(*this == x);
}

void UserInput::modality(
        const std::string& _modality)
{
    impl_->modality(_modality);
}

void UserInput::modality(
        std::string&& _modality)
{
    impl_->modality(std::forward<std::string>(_modality));
}

const std::string& UserInput::modality() const
{
    return impl_->modality();
}

std::string& UserInput::modality()
{
    return impl_->modality();
}

void UserInput::problem_short_description(
        const std::string& _problem_short_description)
{
    impl_->problem_short_description(_problem_short_description);
}

void UserInput::problem_short_description(
        std::string&& _problem_short_description)
{
    impl_->problem_short_description(std::forward<std::string>(_problem_short_description));
}

const std::string& UserInput::problem_short_description() const
{
    return impl_->problem_short_description();
}

std::string& UserInput::problem_short_description()
{
    return impl_->problem_short_description();
}

void UserInput::problem_definition(
        const std::string& _problem_definition)
{
    impl_->problem_definition(_problem_definition);
}

void UserInput::problem_definition(
        std::string&& _problem_definition)
{
    impl_->problem_definition(std::forward<std::string>(_problem_definition));
}

const std::string& UserInput::problem_definition() const
{
    return impl_->problem_definition();
}

std::string& UserInput::problem_definition()
{
    return impl_->problem_definition();
}

void UserInput::inputs(
        const std::vector<std::string>& _inputs)
{
    impl_->inputs(_inputs);
}

void UserInput::inputs(
        std::vector<std::string>&& _inputs)
{
    impl_->inputs(std::forward<std::vector<std::string>>(_inputs));
}

const std::vector<std::string>& UserInput::inputs() const
{
    return impl_->inputs();
}

std::vector<std::string>& UserInput::inputs()
{
    return impl_->inputs();
}

void UserInput::outputs(
        const std::vector<std::string>& _outputs)
{
    impl_->outputs(_outputs);
}

void UserInput::outputs(
        std::vector<std::string>&& _outputs)
{
    impl_->outputs(std::forward<std::vector<std::string>>(_outputs));
}

const std::vector<std::string>& UserInput::outputs() const
{
    return impl_->outputs();
}

std::vector<std::string>& UserInput::outputs()
{
    return impl_->outputs();
}

void UserInput::minimum_samples(
        uint32_t _minimum_samples)
{
    impl_->minimum_samples(_minimum_samples);
}

uint32_t UserInput::minimum_samples() const
{
    return impl_->minimum_samples();
}

uint32_t& UserInput::minimum_samples()
{
    return impl_->minimum_samples();
}

void UserInput::maximum_samples(
        uint32_t _maximum_samples)
{
    impl_->maximum_samples(_maximum_samples);
}

uint32_t UserInput::maximum_samples() const
{
    return impl_->maximum_samples();
}

uint32_t& UserInput::maximum_samples()
{
    return impl_->maximum_samples();
}

void UserInput::optimize_carbon_footprint_manual(
        bool _optimize_carbon_footprint_manual)
{
    impl_->optimize_carbon_footprint_manual(_optimize_carbon_footprint_manual);
}

bool UserInput::optimize_carbon_footprint_manual() const
{
    return impl_->optimize_carbon_footprint_manual();
}

bool& UserInput::optimize_carbon_footprint_manual()
{
    return impl_->optimize_carbon_footprint_manual();
}

void UserInput::previous_iteration(
        int32_t _previous_iteration)
{
    impl_->previous_iteration(_previous_iteration);
}

int32_t UserInput::previous_iteration() const
{
    return impl_->previous_iteration();
}

int32_t& UserInput::previous_iteration()
{
    return impl_->previous_iteration();
}

void UserInput::optimize_carbon_footprint_auto(
        bool _optimize_carbon_footprint_auto)
{
    impl_->optimize_carbon_footprint_auto(_optimize_carbon_footprint_auto);
}

bool UserInput::optimize_carbon_footprint_auto() const
{
    return impl_->optimize_carbon_footprint_auto();
}

bool& UserInput::optimize_carbon_footprint_auto()
{
    return impl_->optimize_carbon_footprint_auto();
}

void UserInput::desired_carbon_footprint(
        double _desired_carbon_footprint)
{
    impl_->desired_carbon_footprint(_desired_carbon_footprint);
}

double UserInput::desired_carbon_footprint() const
{
    return impl_->desired_carbon_footprint();
}

double& UserInput::desired_carbon_footprint()
{
    return impl_->desired_carbon_footprint();
}

void UserInput::geo_location_continent(
        const std::string& _geo_location_continent)
{
    impl_->geo_location_continent(_geo_location_continent);
}

void UserInput::geo_location_continent(
        std::string&& _geo_location_continent)
{
    impl_->geo_location_continent(std::forward<std::string>(_geo_location_continent));
}

const std::string& UserInput::geo_location_continent() const
{
    return impl_->geo_location_continent();
}

std::string& UserInput::geo_location_continent()
{
    return impl_->geo_location_continent();
}

void UserInput::geo_location_region(
        const std::string& _geo_location_region)
{
    impl_->geo_location_region(_geo_location_region);
}

void UserInput::geo_location_region(
        std::string&& _geo_location_region)
{
    impl_->geo_location_region(std::forward<std::string>(_geo_location_region));
}

const std::string& UserInput::geo_location_region() const
{
    return impl_->geo_location_region();
}

std::string& UserInput::geo_location_region()
{
    return impl_->geo_location_region();
}

void UserInput::extra_data(
        const std::vector<uint8_t>& _extra_data)
{
    impl_->extra_data(_extra_data);
}

void UserInput::extra_data(
        std::vector<uint8_t>&& _extra_data)
{
    impl_->extra_data(std::forward<std::vector<uint8_t>>(_extra_data));
}

const std::vector<uint8_t>& UserInput::extra_data() const
{
    return impl_->extra_data();
}

std::vector<uint8_t>& UserInput::extra_data()
{
    return impl_->extra_data();
}

void UserInput::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void UserInput::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& UserInput::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& UserInput::task_id()
{
    return *to_task_id(&impl_->task_id());
}

UserInputImpl* UserInput::get_impl()
{
    return impl_;
}

void UserInput::reset()
{
    impl_->modality("");
    impl_->problem_short_description("");
    impl_->problem_definition("");
    impl_->inputs().clear();
    impl_->outputs().clear();
    impl_->minimum_samples(0);
    impl_->maximum_samples(0);
    impl_->optimize_carbon_footprint_manual(false);
    impl_->previous_iteration(0);
    impl_->optimize_carbon_footprint_auto(false);
    impl_->desired_carbon_footprint(0.0);
    impl_->geo_location_continent("");
    impl_->geo_location_region("");
    impl_->extra_data().clear();
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

const std::type_info& UserInput::impl_typeinfo()
{
    return typeid(UserInputImpl);
}

MLModelMetadata::MLModelMetadata()
{
    impl_ = new MLModelMetadataImpl;
}

MLModelMetadata::~MLModelMetadata()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

MLModelMetadata::MLModelMetadata(
        const MLModelMetadata& x)
{
    impl_ = new MLModelMetadataImpl;

    this->impl_->keywords() = x.impl_->keywords();
    this->impl_->ml_model_metadata() = x.impl_->ml_model_metadata();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
}

MLModelMetadata::MLModelMetadata(
        MLModelMetadata&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

MLModelMetadata& MLModelMetadata::operator =(
        const MLModelMetadata& x)
{
    this->impl_->keywords() = x.impl_->keywords();
    this->impl_->ml_model_metadata() = x.impl_->ml_model_metadata();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

MLModelMetadata& MLModelMetadata::operator =(
        MLModelMetadata&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool MLModelMetadata::operator ==(
        const MLModelMetadata& x) const
{
    return (this->impl_ == x.impl_);
}

bool MLModelMetadata::operator !=(
        const MLModelMetadata& x) const
{
    return !(*this == x);
}

void MLModelMetadata::keywords(
        const std::vector<std::string>& _keywords)
{
    impl_->keywords(_keywords);
}

void MLModelMetadata::keywords(
        std::vector<std::string>&& _keywords)
{
    impl_->keywords(std::forward<std::vector<std::string>>(_keywords));
}

const std::vector<std::string>& MLModelMetadata::keywords() const
{
    return impl_->keywords();
}

void MLModelMetadata::ml_model_metadata(
        const std::vector<std::string>& _ml_model_metadata)
{
    impl_->ml_model_metadata(_ml_model_metadata);
}

void MLModelMetadata::ml_model_metadata(
        std::vector<std::string>&& _ml_model_metadata)
{
    impl_->ml_model_metadata(std::forward<std::vector<std::string>>(_ml_model_metadata));
}

const std::vector<std::string>& MLModelMetadata::ml_model_metadata() const
{
    return impl_->ml_model_metadata();
}

std::vector<std::string>& MLModelMetadata::ml_model_metadata()
{
    return impl_->ml_model_metadata();
}

void MLModelMetadata::extra_data(
        const std::vector<uint8_t>& _extra_data)
{
    impl_->extra_data(_extra_data);
}

void MLModelMetadata::extra_data(
        std::vector<uint8_t>&& _extra_data)
{
    impl_->extra_data(std::forward<std::vector<uint8_t>>(_extra_data));
}

const std::vector<uint8_t>& MLModelMetadata::extra_data() const
{
    return impl_->extra_data();
}

std::vector<uint8_t>& MLModelMetadata::extra_data()
{
    return impl_->extra_data();
}

std::vector<std::string>& MLModelMetadata::keywords()
{
    return impl_->keywords();
}

void MLModelMetadata::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));

}

void MLModelMetadata::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& MLModelMetadata::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& MLModelMetadata::task_id()
{
    return *to_task_id(&impl_->task_id());
}

MLModelMetadataImpl* MLModelMetadata::get_impl()
{
    return impl_;
}

void MLModelMetadata::reset()
{
    impl_->keywords().clear();
    impl_->ml_model_metadata().clear();
    impl_->extra_data().clear();
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

const std::type_info& MLModelMetadata::impl_typeinfo()
{
    return typeid(MLModelMetadataImpl);
}

AppRequirements::AppRequirements()
{
    impl_ = new AppRequirementsImpl;
}

AppRequirements::~AppRequirements()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

AppRequirements::AppRequirements(
        const AppRequirements& x)
{
    impl_ = new AppRequirementsImpl;

    this->impl_->app_requirements() = x.impl_->app_requirements();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
}

AppRequirements::AppRequirements(
        AppRequirements&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

AppRequirements& AppRequirements::operator =(
        const AppRequirements& x)
{
    this->impl_->app_requirements() = x.impl_->app_requirements();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

AppRequirements& AppRequirements::operator =(
        AppRequirements&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool AppRequirements::operator ==(
        const AppRequirements& x) const
{
    return (this->impl_ == x.impl_);
}

bool AppRequirements::operator !=(
        const AppRequirements& x) const
{
    return !(*this == x);
}

void AppRequirements::app_requirements(
        const std::vector<std::string>& _app_requirements)
{
    impl_->app_requirements(_app_requirements);
}

void AppRequirements::app_requirements(
        std::vector<std::string>&& _app_requirements)
{
    impl_->app_requirements(std::forward<std::vector<std::string>>(_app_requirements));
}

const std::vector<std::string>& AppRequirements::app_requirements() const
{
    return impl_->app_requirements();
}

std::vector<std::string>& AppRequirements::app_requirements()
{
    return impl_->app_requirements();
}

void AppRequirements::extra_data(
        const std::vector<uint8_t>& _extra_data)
{
    impl_->extra_data(_extra_data);
}

void AppRequirements::extra_data(
        std::vector<uint8_t>&& _extra_data)
{
    impl_->extra_data(std::forward<std::vector<uint8_t>>(_extra_data));
}

const std::vector<uint8_t>& AppRequirements::extra_data() const
{
    return impl_->extra_data();
}

std::vector<uint8_t>& AppRequirements::extra_data()
{
    return impl_->extra_data();
}

void AppRequirements::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void AppRequirements::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& AppRequirements::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& AppRequirements::task_id()
{
    return *to_task_id(&impl_->task_id());
}

AppRequirementsImpl* AppRequirements::get_impl()
{
    return impl_;
}

void AppRequirements::reset()
{
    impl_->app_requirements().clear();
    impl_->extra_data().clear();
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

const std::type_info& AppRequirements::impl_typeinfo()
{
    return typeid(AppRequirementsImpl);
}

HWConstraints::HWConstraints()
{
    impl_ = new HWConstraintsImpl;
}

HWConstraints::~HWConstraints()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

HWConstraints::HWConstraints(
        const HWConstraints& x)
{
    impl_ = new HWConstraintsImpl;

    this->impl_->max_memory_footprint() = x.impl_->max_memory_footprint();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
}

HWConstraints::HWConstraints(
        HWConstraints&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

HWConstraints& HWConstraints::operator =(
        const HWConstraints& x)
{
    this->impl_->max_memory_footprint() = x.impl_->max_memory_footprint();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

HWConstraints& HWConstraints::operator =(
        HWConstraints&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool HWConstraints::operator ==(
        const HWConstraints& x) const
{
    return (this->impl_ == x.impl_);
}

bool HWConstraints::operator !=(
        const HWConstraints& x) const
{
    return !(*this == x);
}

void HWConstraints::max_memory_footprint(
        uint32_t _max_memory_footprint)
{
    impl_->max_memory_footprint(_max_memory_footprint);
}

uint32_t HWConstraints::max_memory_footprint() const
{
    return impl_->max_memory_footprint();
}

uint32_t& HWConstraints::max_memory_footprint()
{
    return impl_->max_memory_footprint();
}

void HWConstraints::extra_data(
        const std::vector<uint8_t>& _extra_data)
{
    impl_->extra_data(_extra_data);
}

void HWConstraints::extra_data(
        std::vector<uint8_t>&& _extra_data)
{
    impl_->extra_data(std::forward<std::vector<uint8_t>>(_extra_data));
}

const std::vector<uint8_t>& HWConstraints::extra_data() const
{
    return impl_->extra_data();
}

std::vector<uint8_t>& HWConstraints::extra_data()
{
    return impl_->extra_data();
}

void HWConstraints::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void HWConstraints::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& HWConstraints::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& HWConstraints::task_id()
{
    return *to_task_id(&impl_->task_id());
}

HWConstraintsImpl* HWConstraints::get_impl()
{
    return impl_;
}

void HWConstraints::reset()
{
    impl_->max_memory_footprint(0);
    impl_->extra_data().clear();
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

const std::type_info& HWConstraints::impl_typeinfo()
{
    return typeid(HWConstraintsImpl);
}

MLModel::MLModel()
{
    impl_ = new MLModelImpl;
}

MLModel::~MLModel()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

MLModel::MLModel(
        const MLModel& x)
{
    impl_ = new MLModelImpl;

    this->impl_->model() = x.impl_->model();
    this->impl_->model_path() = x.impl_->model_path();
    this->impl_->raw_model() = x.impl_->raw_model();
    this->impl_->model_properties_path() = x.impl_->model_properties_path();
    this->impl_->model_properties() = x.impl_->model_properties();
    this->impl_->input_batch() = x.impl_->input_batch();
    this->impl_->target_latency() = x.impl_->target_latency();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
}

MLModel::MLModel(
        MLModel&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

MLModel& MLModel::operator =(
        const MLModel& x)
{
    this->impl_->model() = x.impl_->model();
    this->impl_->model_path() = x.impl_->model_path();
    this->impl_->raw_model() = x.impl_->raw_model();
    this->impl_->model_properties_path() = x.impl_->model_properties_path();
    this->impl_->model_properties() = x.impl_->model_properties();
    this->impl_->input_batch() = x.impl_->input_batch();
    this->impl_->target_latency() = x.impl_->target_latency();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

MLModel& MLModel::operator =(
        MLModel&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool MLModel::operator ==(
        const MLModel& x) const
{
    return (this->impl_ == x.impl_);
}

bool MLModel::operator !=(
        const MLModel& x) const
{
    return !(*this == x);
}

void MLModel::model_path(
        const std::string& _model_path)
{
    impl_->model_path(_model_path);
}

void MLModel::model_path(
        std::string&& _model_path)
{
    impl_->model_path(std::forward<std::string>(_model_path));
}

const std::string& MLModel::model_path() const
{
    return impl_->model_path();
}

std::string& MLModel::model_path()
{
    return impl_->model_path();
}

void MLModel::model(
        const std::string& _model)
{
    impl_->model(_model);
}

void MLModel::model(
        std::string&& _model)
{
    impl_->model(std::forward<std::string>(_model));
}

const std::string& MLModel::model() const
{
    return impl_->model();
}

std::string& MLModel::model()
{
    return impl_->model();
}

void MLModel::raw_model(
        const std::vector<uint8_t>& _raw_model)
{
    impl_->raw_model(_raw_model);
}

void MLModel::raw_model(
        std::vector<uint8_t>&& _raw_model)
{
    impl_->raw_model(std::forward<std::vector<uint8_t>>(_raw_model));
}

const std::vector<uint8_t>& MLModel::raw_model() const
{
    return impl_->raw_model();
}

std::vector<uint8_t>& MLModel::raw_model()
{
    return impl_->raw_model();
}

void MLModel::model_properties_path(
        const std::string& _model_properties_path)
{
    impl_->model_properties_path(_model_properties_path);
}

void MLModel::model_properties_path(
        std::string&& _model_properties_path)
{
    impl_->model_properties_path(std::forward<std::string>(_model_properties_path));
}

const std::string& MLModel::model_properties_path() const
{
    return impl_->model_properties_path();
}

std::string& MLModel::model_properties_path()
{
    return impl_->model_properties_path();
}

void MLModel::model_properties(
        const std::string& _model_properties)
{
    impl_->model_properties(_model_properties);
}

void MLModel::model_properties(
        std::string&& _model_properties)
{
    impl_->model_properties(std::forward<std::string>(_model_properties));
}

const std::string& MLModel::model_properties() const
{
    return impl_->model_properties();
}

std::string& MLModel::model_properties()
{
    return impl_->model_properties();
}

void MLModel::input_batch(
        const std::vector<std::string>& _input_batch)
{
    impl_->input_batch(_input_batch);
}

void MLModel::input_batch(
        std::vector<std::string>&& _input_batch)
{
    impl_->input_batch(std::forward<std::vector<std::string>>(_input_batch));
}

const std::vector<std::string>& MLModel::input_batch() const
{
    return impl_->input_batch();
}

std::vector<std::string>& MLModel::input_batch()
{
    return impl_->input_batch();
}

void MLModel::target_latency(
        double _target_latency)
{
    impl_->target_latency(_target_latency);
}

double MLModel::target_latency() const
{
    return impl_->target_latency();
}

double& MLModel::target_latency()
{
    return impl_->target_latency();
}

void MLModel::extra_data(
        const std::vector<uint8_t>& _extra_data)
{
    impl_->extra_data(_extra_data);
}

void MLModel::extra_data(
        std::vector<uint8_t>&& _extra_data)
{
    impl_->extra_data(std::forward<std::vector<uint8_t>>(_extra_data));
}

const std::vector<uint8_t>& MLModel::extra_data() const
{
    return impl_->extra_data();
}

std::vector<uint8_t>& MLModel::extra_data()
{
    return impl_->extra_data();
}

void MLModel::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void MLModel::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& MLModel::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& MLModel::task_id()
{
    return *to_task_id(&impl_->task_id());
}

MLModelImpl* MLModel::get_impl()
{
    return impl_;
}

void MLModel::reset()
{
    impl_->model("");
    impl_->model_path("");
    impl_->raw_model().clear();
    impl_->model_properties_path("");
    impl_->model_properties("");
    impl_->input_batch().clear();
    impl_->target_latency(0.0);
    impl_->extra_data().clear();
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

const std::type_info& MLModel::impl_typeinfo()
{
    return typeid(MLModelImpl);
}

HWResource::HWResource()
{
    impl_ = new HWResourceImpl;
}

HWResource::~HWResource()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

HWResource::HWResource(
        const HWResource& x)
{
    impl_ = new HWResourceImpl;

    this->impl_->hw_description() = x.impl_->hw_description();
    this->impl_->power_consumption() = x.impl_->power_consumption();
    this->impl_->latency() = x.impl_->latency();
    this->impl_->memory_footprint_of_ml_model() = x.impl_->memory_footprint_of_ml_model();
    this->impl_->max_hw_memory_footprint() = x.impl_->max_hw_memory_footprint();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
}

HWResource::HWResource(
        HWResource&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

HWResource& HWResource::operator =(
        const HWResource& x)
{
    this->impl_->hw_description() = x.impl_->hw_description();
    this->impl_->power_consumption() = x.impl_->power_consumption();
    this->impl_->latency() = x.impl_->latency();
    this->impl_->memory_footprint_of_ml_model() = x.impl_->memory_footprint_of_ml_model();
    this->impl_->max_hw_memory_footprint() = x.impl_->max_hw_memory_footprint();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

HWResource& HWResource::operator =(
        HWResource&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool HWResource::operator ==(
        const HWResource& x) const
{
    return (this->impl_ == x.impl_);
}

bool HWResource::operator !=(
        const HWResource& x) const
{
    return !(*this == x);
}

void HWResource::hw_description(
        const std::string& _hw_description)
{
    impl_->hw_description(_hw_description);
}

void HWResource::hw_description(
        std::string&& _hw_description)
{
    impl_->hw_description(std::forward<std::string>(_hw_description));
}

const std::string& HWResource::hw_description() const
{
    return impl_->hw_description();
}

std::string& HWResource::hw_description()
{
    return impl_->hw_description();
}

void HWResource::power_consumption(
        double _power_consumption)
{
    impl_->power_consumption(_power_consumption);
}

double HWResource::power_consumption() const
{
    return impl_->power_consumption();
}

double& HWResource::power_consumption()
{
    return impl_->power_consumption();
}

void HWResource::latency(
        double _latency)
{
    impl_->latency(_latency);
}

double HWResource::latency() const
{
    return impl_->latency();
}

double& HWResource::latency()
{
    return impl_->latency();
}

void HWResource::memory_footprint_of_ml_model(
        double _memory_footprint_of_ml_model)
{
    impl_->memory_footprint_of_ml_model(_memory_footprint_of_ml_model);
}

double HWResource::memory_footprint_of_ml_model() const
{
    return impl_->memory_footprint_of_ml_model();
}

double& HWResource::memory_footprint_of_ml_model()
{
    return impl_->memory_footprint_of_ml_model();
}

void HWResource::max_hw_memory_footprint(
        double _max_hw_memory_footprint)
{
    impl_->max_hw_memory_footprint(_max_hw_memory_footprint);
}

double HWResource::max_hw_memory_footprint() const
{
    return impl_->max_hw_memory_footprint();
}

double& HWResource::max_hw_memory_footprint()
{
    return impl_->max_hw_memory_footprint();
}

void HWResource::extra_data(
        const std::vector<uint8_t>& _extra_data)
{
    impl_->extra_data(_extra_data);
}

void HWResource::extra_data(
        std::vector<uint8_t>&& _extra_data)
{
    impl_->extra_data(std::forward<std::vector<uint8_t>>(_extra_data));
}

const std::vector<uint8_t>& HWResource::extra_data() const
{
    return impl_->extra_data();
}

std::vector<uint8_t>& HWResource::extra_data()
{
    return impl_->extra_data();
}

void HWResource::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void HWResource::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& HWResource::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& HWResource::task_id()
{
    return *to_task_id(&impl_->task_id());
}

HWResourceImpl* HWResource::get_impl()
{
    return impl_;
}

void HWResource::reset()
{
    impl_->hw_description("");
    impl_->power_consumption(0.0);
    impl_->latency(0.0);
    impl_->memory_footprint_of_ml_model(0.0);
    impl_->max_hw_memory_footprint(0.0);
    impl_->extra_data().clear();
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

const std::type_info& HWResource::impl_typeinfo()
{
    return typeid(HWResourceImpl);
}

CO2Footprint::CO2Footprint()
{
    impl_ = new CO2FootprintImpl;
}

CO2Footprint::~CO2Footprint()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

CO2Footprint::CO2Footprint(
        const CO2Footprint& x)
{
    impl_ = new CO2FootprintImpl;

    this->impl_->carbon_intensity() = x.impl_->carbon_intensity();
    this->impl_->carbon_footprint() = x.impl_->carbon_footprint();
    this->impl_->energy_consumption() = x.impl_->energy_consumption();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
}

CO2Footprint::CO2Footprint(
        CO2Footprint&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

CO2Footprint& CO2Footprint::operator =(
        const CO2Footprint& x)
{
    this->impl_->carbon_intensity() = x.impl_->carbon_intensity();
    this->impl_->carbon_footprint() = x.impl_->carbon_footprint();
    this->impl_->energy_consumption() = x.impl_->energy_consumption();
    this->impl_->extra_data() = x.impl_->extra_data();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

CO2Footprint& CO2Footprint::operator =(
        CO2Footprint&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    this->impl_ = x.impl_;
    x.impl_ = nullptr;
    return *this;
}

bool CO2Footprint::operator ==(
        const CO2Footprint& x) const
{
    return (this->impl_ == x.impl_);
}

bool CO2Footprint::operator !=(
        const CO2Footprint& x) const
{
    return !(*this == x);
}

void CO2Footprint::carbon_footprint(
        double _carbon_footprint)
{
    impl_->carbon_footprint(_carbon_footprint);
}

double CO2Footprint::carbon_footprint() const
{
    return impl_->carbon_footprint();
}

double& CO2Footprint::carbon_footprint()
{
    return impl_->carbon_footprint();
}

void CO2Footprint::energy_consumption(
        double _energy_consumption)
{
    impl_->energy_consumption(_energy_consumption);
}

double CO2Footprint::energy_consumption() const
{
    return impl_->energy_consumption();
}

double& CO2Footprint::energy_consumption()
{
    return impl_->energy_consumption();
}

void CO2Footprint::carbon_intensity(
        double _carbon_intensity)
{
    impl_->carbon_intensity(_carbon_intensity);
}

double CO2Footprint::carbon_intensity() const
{
    return impl_->carbon_intensity();
}

double& CO2Footprint::carbon_intensity()
{
    return impl_->carbon_intensity();
}

void CO2Footprint::extra_data(
        const std::vector<uint8_t>& _extra_data)
{
    impl_->extra_data(_extra_data);
}

void CO2Footprint::extra_data(
        std::vector<uint8_t>&& _extra_data)
{
    impl_->extra_data(std::forward<std::vector<uint8_t>>(_extra_data));
}

const std::vector<uint8_t>& CO2Footprint::extra_data() const
{
    return impl_->extra_data();
}

std::vector<uint8_t>& CO2Footprint::extra_data()
{
    return impl_->extra_data();
}

void CO2Footprint::task_id(
        const TaskId& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

void CO2Footprint::task_id(
        TaskId&& _task_id)
{
    impl_->task_id(*to_task_id_impl(const_cast<TaskId*>(&_task_id)));
}

const TaskId& CO2Footprint::task_id() const
{
    return *to_task_id(&impl_->task_id());
}

TaskId& CO2Footprint::task_id()
{
    return *to_task_id(&impl_->task_id());
}

CO2FootprintImpl* CO2Footprint::get_impl()
{
    return impl_;
}

void CO2Footprint::reset()
{
    impl_->carbon_intensity(0.0);
    impl_->carbon_footprint(0.0);
    impl_->energy_consumption(0.0);
    impl_->extra_data().clear();
    impl_->task_id().problem_id(sustainml::common::INVALID_ID);
    impl_->task_id().iteration_id(sustainml::common::INVALID_ID);
}

const std::type_info& CO2Footprint::impl_typeinfo()
{
    return typeid(CO2FootprintImpl);
}

} // namespace types
