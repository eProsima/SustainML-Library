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

#include <types/typesImpl.h>

namespace types {

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

NodeStatus::NodeStatus(const NodeStatus& x)
{
    impl_ = new NodeStatusImpl;

    this->impl_->node_status() = x.impl_->node_status();
    this->impl_->node_name() = x.impl_->node_name();
    this->impl_->task_status() = x.impl_->task_status();
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->error_code() = x.impl_->error_code();
    this->impl_->error_description() = x.impl_->error_description();
}

NodeStatus::NodeStatus(NodeStatus&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

NodeStatus& NodeStatus::operator =(const NodeStatus& x)
{
    this->impl_->node_status() = x.impl_->node_status();
    this->impl_->node_name() = x.impl_->node_name();
    this->impl_->task_status() = x.impl_->task_status();
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->error_code() = x.impl_->error_code();
    this->impl_->error_description() = x.impl_->error_description();
    return *this;
}

NodeStatus& NodeStatus::operator =(NodeStatus&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool NodeStatus::operator ==(const NodeStatus& x) const
{
    return (this->impl_ == x.impl_);
}

bool NodeStatus::operator !=(const NodeStatus& x) const
{
    return !(*this == x);
}

void NodeStatus::node_status(Status _node_status)
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

void NodeStatus::task_status(TaskStatus _task_status)
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

void NodeStatus::error_code(ErrorCode _error_code)
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

void NodeStatus::task_id(int32_t _task_id)
{
    impl_->task_id(_task_id);
}

int32_t NodeStatus::task_id() const
{
    return impl_->task_id();
}

int32_t& NodeStatus::task_id()
{
    return impl_->task_id();
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

std::string NodeStatus::error_description()
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

std::string NodeStatus::node_name()
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

NodeControl::NodeControl(const NodeControl& x)
{
    impl_ = new NodeControlImpl;

    this->impl_->cmd_node() = x.impl_->cmd_node();
    this->impl_->source_node() = x.impl_->source_node();
    this->impl_->target_node() = x.impl_->target_node();
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->cmd_task() = this->impl_->cmd_task();
}

NodeControl::NodeControl(NodeControl&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

NodeControl& NodeControl::operator =(const NodeControl& x)
{
    this->impl_->cmd_node() = x.impl_->cmd_node();
    this->impl_->source_node() = x.impl_->source_node();
    this->impl_->target_node() = x.impl_->target_node();
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->cmd_task() = this->impl_->cmd_task();
    return *this;
}

NodeControl& NodeControl::operator =(NodeControl&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool NodeControl::operator ==(const NodeControl& x) const
{
    return (this->impl_ == x.impl_);
}

bool NodeControl::operator !=(const NodeControl& x) const
{
    return !(*this == x);
}


void NodeControl::cmd_node(CmdNode _cmd_node)
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

void NodeControl::cmd_task(CmdTask _cmd_task)
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

void NodeControl::target_node(const std::string& _target_node)
{
    impl_->target_node(_target_node);
}


void NodeControl::target_node(std::string&& _target_node)
{
    return impl_->target_node(std::forward<std::string>(_target_node));
}

const std::string& NodeControl::target_node() const
{
    return impl_->target_node();
}

std::string NodeControl::target_node()
{
    return impl_->target_node();
}

void NodeControl::task_id(int32_t _task_id)
{
    impl_->task_id(_task_id);
}

int32_t NodeControl::task_id() const
{
    return impl_->task_id();
}

int32_t& NodeControl::task_id()
{
    return impl_->task_id();
}

void NodeControl::source_node(const std::string& _source_node)
{
    impl_->source_node(_source_node);
}

void NodeControl::source_node(std::string&& _source_node)
{
    impl_->source_node(std::forward<std::string>(_source_node));
}

const std::string& NodeControl::source_node() const
{
    return impl_->source_node();
}

std::string NodeControl::source_node()
{
    return impl_->source_node();
}


GeoLocation::GeoLocation()
{
    impl_ = new GeoLocationImpl();
}

GeoLocation::~GeoLocation()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

GeoLocation::GeoLocation(const GeoLocation& x)
{
    impl_ = new GeoLocationImpl;

    this->impl_->continent() = x.impl_->continent();
    this->impl_->region() = x.impl_->region();
}

GeoLocation::GeoLocation(GeoLocation&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

GeoLocation& GeoLocation::operator =(const GeoLocation& x)
{
    this->impl_->continent() = x.impl_->continent();
    this->impl_->region() = x.impl_->region();
    return *this;
}

GeoLocation& GeoLocation::operator =(GeoLocation&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool GeoLocation::operator ==(const GeoLocation& x) const
{
    return (this->impl_ == x.impl_);
}

bool GeoLocation::operator !=(const GeoLocation& x) const
{
    return !(*this == x);
}

void GeoLocation::continent(
        const std::string& _continent)
{
    impl_->continent(_continent);
}


void GeoLocation::continent(
        std::string&& _continent)
{
    impl_->continent(std::forward<std::string>(_continent));
}

const std::string& GeoLocation::continent() const
{
    return impl_->continent();
}

std::string GeoLocation::continent()
{
    return impl_->continent();
}

void GeoLocation::region(
        const std::string& _region)
{
    impl_->region(_region);
}

void GeoLocation::region(
        std::string&& _region)
{
    impl_->region(std::forward<std::string>(_region));
}

const std::string& GeoLocation::region() const
{
    return impl_->region();
}

std::string GeoLocation::region()
{
    return impl_->region();
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

UserInput::UserInput(const UserInput& x)
{
    impl_ = new UserInputImpl;

    this->impl_->geo_location() = x.impl_->geo_location();
    this->impl_->problem_description() = x.impl_->problem_description();
    this->impl_->task_id() = x.impl_->task_id();
}

UserInput::UserInput(UserInput&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

UserInput& UserInput::operator =(const UserInput& x)
{
    this->impl_->geo_location() = x.impl_->geo_location();
    this->impl_->problem_description() = x.impl_->problem_description();
    this->impl_->task_id() = x.impl_->task_id();
    return *this;
}

UserInput& UserInput::operator =(UserInput&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool UserInput::operator ==(const UserInput& x) const
{
    return (this->impl_ == x.impl_);
}

bool UserInput::operator !=(const UserInput& x) const
{
    return !(*this == x);
}

void UserInput::problem_description(
        const std::string& _problem_description)
{
    impl_->problem_description(_problem_description);
}

void UserInput::problem_description(
        std::string&& _problem_description)
{
    impl_->problem_description(std::forward<std::string>(_problem_description));
}

const std::string& UserInput::problem_description() const
{
    return impl_->problem_description();
}

std::string UserInput::problem_description()
{
    return impl_->problem_description();
}

void UserInput::geo_location(
        const GeoLocation& _geo_location)
{
    impl_->geo_location(*_geo_location.impl_);
}

void UserInput::geo_location(
        GeoLocation&& _geo_location)
{
    impl_->geo_location(std::forward<GeoLocationImpl>(*_geo_location.impl_));
}

const GeoLocationImpl& UserInput::geo_location() const
{
    return impl_->geo_location();
}

GeoLocationImpl& UserInput::geo_location()
{
    return impl_->geo_location();
}

void UserInput::task_id(
        int32_t _task_id)
{
    impl_->task_id(_task_id);
}

int32_t UserInput::task_id() const
{
    return impl_->task_id();
}

int32_t& UserInput::task_id()
{
    return impl_->task_id();
}

UserInputImpl* UserInput::get_impl()
{
    return impl_;
}

const std::type_info& UserInput::impl_typeinfo()
{
    return typeid(UserInputImpl);
}

EncodedTask::EncodedTask()
{
    impl_ = new EncodedTaskImpl;
}

EncodedTask::~EncodedTask()
{
    if (nullptr != impl_)
    {
        delete impl_;
    }
}

EncodedTask::EncodedTask(const EncodedTask& x)
{
    impl_ = new EncodedTaskImpl;

    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->keywords() = x.impl_->keywords();
}

EncodedTask::EncodedTask(EncodedTask&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

EncodedTask& EncodedTask::operator =(const EncodedTask& x)
{
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->keywords() = x.impl_->keywords();
    return *this;
}

EncodedTask& EncodedTask::operator =(EncodedTask&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool EncodedTask::operator ==(const EncodedTask& x) const
{
    return (this->impl_ == x.impl_);
}

bool EncodedTask::operator !=(const EncodedTask& x) const
{
    return !(*this == x);
}

void EncodedTask::keywords(const std::vector<std::string>& _keywords)
{
    impl_->keywords(_keywords);
}

void EncodedTask::keywords(std::vector<std::string>&& _keywords)
{
    impl_->keywords(std::forward<std::vector<std::string>>(_keywords));
}

const std::vector<std::string>& EncodedTask::keywords() const
{
    return impl_->keywords();
}

std::vector<std::string>& EncodedTask::keywords()
{
    return impl_->keywords();
}

void EncodedTask::task_id(
            int32_t _task_id)
{
    impl_->task_id(_task_id);
}

int32_t EncodedTask::task_id() const
{
    return impl_->task_id();
}

int32_t& EncodedTask::task_id()
{
    return impl_->task_id();
}

EncodedTaskImpl* EncodedTask::get_impl()
{
    return impl_;
}

const std::type_info& EncodedTask::impl_typeinfo()
{
    return typeid(EncodedTaskImpl);
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

MLModel::MLModel(const MLModel& x)
{
    impl_ = new MLModelImpl;

    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->model() = x.impl_->model();
    this->impl_->model_path() = x.impl_->model_path();
    this->impl_->model_properties_path() = x.impl_->model_properties_path();
    this->impl_->model_properties() = x.impl_->model_properties();
}

MLModel::MLModel(MLModel&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

MLModel& MLModel::operator =(const MLModel& x)
{
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->model() = x.impl_->model();
    this->impl_->model_path() = x.impl_->model_path();
    this->impl_->model_properties_path() = x.impl_->model_properties_path();
    this->impl_->model_properties() = x.impl_->model_properties();
    return *this;
}

MLModel& MLModel::operator =(MLModel&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool MLModel::operator ==(const MLModel& x) const
{
    return (this->impl_ == x.impl_);
}

bool MLModel::operator !=(const MLModel& x) const
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

std::string MLModel::model_path()
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

std::string MLModel::model()
{
    return impl_->model();
}

void MLModel::model_properties_path(const std::string& _model_properties_path)
{
    impl_->model_properties_path(_model_properties_path);
}

void MLModel::model_properties_path(std::string&& _model_properties_path)
{
    impl_->model_properties_path(std::forward<std::string>(_model_properties_path));
}

const std::string& MLModel::model_properties_path() const
{
    return impl_->model_properties_path();
}

std::string MLModel::model_properties_path()
{
    return impl_->model_properties_path();
}

void MLModel::model_properties(const std::string& _model_properties)
{
    impl_->model_properties(_model_properties);
}

void MLModel::model_properties(std::string&& _model_properties)
{
    impl_->model_properties(std::forward<std::string>(_model_properties));
}

const std::string& MLModel::model_properties() const
{
    return impl_->model_properties();
}

std::string MLModel::model_properties()
{
    return impl_->model_properties();
}

void MLModel::task_id(
            int32_t _task_id)
{
    impl_->task_id(_task_id);
}

int32_t MLModel::task_id() const
{
    return impl_->task_id();
}

int32_t& MLModel::task_id()
{
    return impl_->task_id();
}

MLModelImpl* MLModel::get_impl()
{
    return impl_;
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

HWResource::HWResource(const HWResource& x)
{
    impl_ = new HWResourceImpl;

    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->hw_description() = x.impl_->hw_description();
    this->impl_->power_consumption() = x.impl_->power_consumption();
}

HWResource::HWResource(HWResource&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

HWResource& HWResource::operator =(const HWResource& x)
{
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->hw_description() = x.impl_->hw_description();
    this->impl_->power_consumption() = x.impl_->power_consumption();
    return *this;
}

HWResource& HWResource::operator =(HWResource&& x) noexcept
{
    if (x.impl_ != this->impl_)
    {
        delete this->impl_;
        this->impl_ = x.impl_;
        x.impl_ = nullptr;
    }

    return *this;
}

bool HWResource::operator ==(const HWResource& x) const
{
    return (this->impl_ == x.impl_);
}

bool HWResource::operator !=(const HWResource& x) const
{
    return !(*this == x);
}

void HWResource::hw_description(const std::string& _hw_description)
{
    impl_->hw_description(_hw_description);
}

void HWResource::hw_description(std::string&& _hw_description)
{
    impl_->hw_description(std::forward<std::string>(_hw_description));
}

const std::string& HWResource::hw_description() const
{
    return impl_->hw_description();
}

std::string HWResource::hw_description()
{
    return impl_->hw_description();
}

void HWResource::power_consumption(double _power_consumption)
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

void HWResource::task_id(int32_t _task_id)
{
    impl_->task_id(_task_id);
}

int32_t HWResource::task_id() const
{
    return impl_->task_id();
}

int32_t& HWResource::task_id()
{
    return impl_->task_id();
}

HWResourceImpl* HWResource::get_impl()
{
    return impl_;
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

CO2Footprint::CO2Footprint(const CO2Footprint& x)
{
    impl_ = new CO2FootprintImpl;

    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->carbon_intensity() = x.impl_->carbon_intensity();
    this->impl_->co2_footprint() = x.impl_->co2_footprint();
    this->impl_->energy_consumption() = x.impl_->energy_consumption();
}

CO2Footprint::CO2Footprint(CO2Footprint&& x) noexcept
{
    this->impl_ = x.impl_;
    x.impl_ = nullptr;
}

CO2Footprint& CO2Footprint::operator =(const CO2Footprint& x)
{
    this->impl_->task_id() = x.impl_->task_id();
    this->impl_->carbon_intensity() = x.impl_->carbon_intensity();
    this->impl_->co2_footprint() = x.impl_->co2_footprint();
    this->impl_->energy_consumption() = x.impl_->energy_consumption();
    return *this;
}

CO2Footprint& CO2Footprint::operator =(CO2Footprint&& x) noexcept
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

bool CO2Footprint::operator ==(const CO2Footprint& x) const
{
    return (this->impl_ == x.impl_);
}

bool CO2Footprint::operator !=(const CO2Footprint& x) const
{
    return !(*this == x);
}

void CO2Footprint::co2_footprint(double _co2_footprint)
{
    impl_->co2_footprint(_co2_footprint);
}

double CO2Footprint::co2_footprint() const
{
    return impl_->co2_footprint();
}

double& CO2Footprint::co2_footprint()
{
    return impl_->co2_footprint();
}

void CO2Footprint::energy_consumption(double _energy_consumption)
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

void CO2Footprint::carbon_intensity(double _carbon_intensity)
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

void CO2Footprint::task_id(int32_t _task_id)
{
    impl_->task_id(_task_id);
}

int32_t CO2Footprint::task_id() const
{
    return impl_->task_id();
}

int32_t& CO2Footprint::task_id()
{
    return impl_->task_id();
}

CO2FootprintImpl* CO2Footprint::get_impl()
{
    return impl_;
}

const std::type_info& CO2Footprint::impl_typeinfo()
{
    return typeid(CO2FootprintImpl);
}

} // namespace types
