// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/**
 * @file OrchestratorNode.cpp
 */

#include <sustainml_cpp/core/RequestReplier.hpp>
#include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>

#include "ModuleNodeProxyFactory.hpp"
#include "TaskDB.ipp"

#include <common/Common.hpp>
#include <core/RequestReplier.hpp>
#include <orchestrator/TaskManager.hpp>
#include <types/typesImplPubSubTypes.hpp>
#include <types/typesImplTypeObjectSupport.hpp>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <iostream>  // + debug prints

using namespace eprosima::fastdds::dds;

namespace sustainml {
namespace orchestrator {

OrchestratorNode::OrchestratorParticipantListener::OrchestratorParticipantListener(
        OrchestratorNode* orchestrator)
    : orchestrator_(orchestrator)
{

}

void OrchestratorNode::OrchestratorParticipantListener::on_participant_discovery(
        eprosima::fastdds::dds::DomainParticipant* participant,
        eprosima::fastdds::rtps::ParticipantDiscoveryStatus reason,
        const eprosima::fastdds::rtps::ParticipantBuiltinTopicData& info,
        bool& should_be_ignored)
{
    eprosima::fastcdr::string_255 participant_name = info.participant_name;
    EPROSIMA_LOG_INFO(ORCHESTRATOR,
            "Orchestrator discovered a new Participant with name " << participant_name.to_string());

    // Synchronise with Orchestrator initialization
    if (!orchestrator_->initialized_.load())
    {
        std::unique_lock<std::mutex> lock(orchestrator_->get_mutex());
        orchestrator_->initialization_cv_.wait(lock, [&]()
                {
                    return orchestrator_->initialized_.load();
                });
    }

    // create the proxy for this node
    NodeID node_id = common::get_node_id_from_name(participant_name);

    std::lock_guard<std::mutex> lock(orchestrator_->proxies_mtx_);

    // check if the node has been terminated
    if (!orchestrator_->terminated_.load())
    {
        if (reason == eprosima::fastdds::rtps::ParticipantDiscoveryStatus::DISCOVERED_PARTICIPANT &&
                orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)] == nullptr)
        {
            EPROSIMA_LOG_INFO(ORCHESTRATOR, "Creating node proxy for " << participant_name << " node");
            ModuleNodeProxyFactory::make_node_proxy(
                node_id,
                orchestrator_,
                orchestrator_->task_db_,
                orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)]);
        }
        else if ((reason == eprosima::fastdds::rtps::ParticipantDiscoveryStatus::DROPPED_PARTICIPANT ||
                reason == eprosima::fastdds::rtps::ParticipantDiscoveryStatus::REMOVED_PARTICIPANT) &&
                orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)] != nullptr)
        {
            EPROSIMA_LOG_INFO(ORCHESTRATOR, "Setting inactive " << participant_name << " node");
            types::NodeStatus status = orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)]->get_status();
            status.node_status(Status::NODE_INACTIVE);
            orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)]->set_status(status);
            orchestrator_->handler_->on_node_status_change(node_id, status);
        }
    }
}

OrchestratorNode::OrchestratorNode(
        OrchestratorNodeHandle& handle,
        uint32_t domain)
    : domain_(domain)
    , handler_(&handle)
    , node_proxies_({
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        }),
    task_db_(new TaskDB_t()),
    task_man_(new TaskManager()),
    participant_listener_(new OrchestratorParticipantListener(this))
{
    if (!init())
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Orchestrator initialization Failed");
    }
}

OrchestratorNode::~OrchestratorNode()
{
    terminate();
}

void OrchestratorNode::destroy()
{
    if (!terminated_.load())
    {
        {
            std::lock_guard<std::mutex> lock_proxies(proxies_mtx_);
            std::lock_guard<std::mutex> lock(mtx_);
            for (size_t i = 0; i < (size_t)NodeID::MAX; i++)
            {
                if (node_proxies_[i] != nullptr)
                {
                    delete node_proxies_[i];
                    node_proxies_[i] = nullptr;
                }
            }
        }

        if (sub_ != nullptr)
        {
            sub_->delete_contained_entities();
        }

        if (pub_ != nullptr)
        {
            pub_->delete_contained_entities();
        }

        if (participant_ != nullptr)
        {
            participant_->delete_contained_entities();
        }

        DomainParticipantFactory::get_instance()->delete_participant(participant_);

        delete task_man_;
        delete req_res_;

        handler_ = nullptr;
        terminated_.store(true);
    }
}

void OrchestratorNode::print_db()
{
    std::cout << *task_db_ << std::endl;
}

bool OrchestratorNode::init()
{
    auto dpf = DomainParticipantFactory::get_instance();

    DomainParticipantQos dpqos = PARTICIPANT_QOS_DEFAULT;
    dpqos.name("ORCHESTRATOR_NODE");

    //! Set sustainML app ID participant properties
    dpqos.properties().properties().emplace_back("fastdds.application.id", "SUSTAINML", true);
    dpqos.properties().properties().emplace_back("fastdds.application.metadata", "", true);

    //! Initialize entities
    uint32_t domain = common::parse_sustainml_env(domain_);
    participant_ = dpf->create_participant(domain,
                    dpqos,
                    participant_listener_.get(),
                    StatusMask::all() >> StatusMask::data_on_readers());

    if (participant_ == nullptr)
    {
        return false;
    }

    //! Register Common Types
    std::vector<eprosima::fastdds::dds::TypeSupport> sustainml_types;
    sustainml_types.reserve(common::Topics::MAX);

    sustainml_types.push_back(static_cast<TypeSupport>(new AppRequirementsImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new CO2FootprintImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new HWConstraintsImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new HWResourceImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new MLModelImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new MLModelMetadataImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new NodeControlImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new NodeStatusImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new UserInputImplPubSubType()));

    for (auto&& type : sustainml_types)
    {
        participant_->register_type(type);
    }

    status_topic_ = participant_->create_topic(
        common::TopicCollection::get()[common::Topics::NODE_STATUS].first.c_str(),
        common::TopicCollection::get()[common::Topics::NODE_STATUS].second.c_str(), TOPIC_QOS_DEFAULT);

    control_topic_ = participant_->create_topic(
        common::TopicCollection::get()[common::Topics::NODE_CONTROL].first.c_str(),
        common::TopicCollection::get()[common::Topics::NODE_CONTROL].second.c_str(), TOPIC_QOS_DEFAULT);

    user_input_topic_ = participant_->create_topic(
        common::TopicCollection::get()[common::Topics::USER_INPUT].first.c_str(),
        common::TopicCollection::get()[common::Topics::USER_INPUT].second.c_str(), TOPIC_QOS_DEFAULT);

    if (status_topic_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the status topic");
        return false;
    }

    pub_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);

    if (pub_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the publisher");
        return false;
    }

    DataWriterQos dwqos = DATAWRITER_QOS_DEFAULT;
    dwqos.resource_limits().max_instances = 500;
    dwqos.resource_limits().max_samples_per_instance = 1;
    dwqos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;

    control_writer_ = pub_->create_datawriter(control_topic_, dwqos);

    if (control_writer_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the control writer");
        return false;
    }

    user_input_writer_ = pub_->create_datawriter(user_input_topic_, dwqos);

    if (user_input_writer_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the user input writer");
        return false;
    }

    sub_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

    if (sub_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the subscriber");
        return false;
    }

    req_res_ = new core::RequestReplier([this](void* input)
                    {
                        this->req_res_->resume_taking_data();
                    }, "sustainml/request", "sustainml/response", participant_, pub_, sub_, res_.get_impl());

    // Create/Open SQL Database and hydrate TaskDB BEFORE we mark initialized_
    {
        std::string db_file = "sustainml.db";
        database_.reset(new sustainml::database::Database(db_file));
        if (database_->initialize() != SQLITE_OK)
        {
            EPROSIMA_LOG_WARNING(ORCHESTRATOR, "Failed to open SQL database, continuing without persistence");
        }
        else
        {
            hydrate_from_db_();
        }
    }

    initialized_.store(true);
    initialization_cv_.notify_one();

    return true;
}

// --- JSON helpers (serialize) ---
static inline std::string json_escape_(const std::string& in)
{
    std::string out; out.reserve(in.size()+8);
    for (char c : in)
    {
        switch (c)
        {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}
static inline std::string q_(const std::string& s) { return std::string("\"")+json_escape_(s)+"\""; }
static inline std::string arr_strs_(const std::vector<std::string>& v)
{
    std::string out="["; bool first=true;
    for (auto& s : v) { if(!first) out+=','; first=false; out+=q_(s); }
    out += "]";
    return out;
}
static inline std::string bytes_to_string_(const std::vector<uint8_t>& v)
{
    return std::string(reinterpret_cast<const char*>(v.data()), v.size());
}
static inline std::string base64_encode_(const std::vector<uint8_t>& data)
{
    static const char* tbl="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out; out.reserve((data.size()+2)/3*4);
    size_t i=0;
    while (i+2<data.size())
    {
        uint32_t n=(data[i]<<16)|(data[i+1]<<8)|data[i+2];
        out.push_back(tbl[(n>>18)&63]); out.push_back(tbl[(n>>12)&63]);
        out.push_back(tbl[(n>>6)&63]);  out.push_back(tbl[n&63]);
        i+=3;
    }
    if (i+1==data.size())
    {
        uint32_t n=(data[i]<<16);
        out.push_back(tbl[(n>>18)&63]); out.push_back(tbl[(n>>12)&63]);
        out.push_back('='); out.push_back('=');
    }
    else if (i+2==data.size())
    {
        uint32_t n=(data[i]<<16)|(data[i+1]<<8);
        out.push_back(tbl[(n>>18)&63]); out.push_back(tbl[(n>>12)&63]);
        out.push_back(tbl[(n>>6)&63]);  out.push_back('=');
    }
    return out;
}

// Validate if a string is JSON using SQLite JSON1
static inline bool sqlite_json_valid_(sqlite3* db, const std::string& s)
{
    if (!db) return false;
    static const char* q="SELECT json_valid(?)";
    sqlite3_stmt* st=nullptr;
    if (sqlite3_prepare_v2(db,q,-1,&st,nullptr)!=SQLITE_OK) return false;
    sqlite3_bind_text(st,1,s.c_str(),(int)s.size(),SQLITE_TRANSIENT);
    bool ok=false;
    if (sqlite3_step(st)==SQLITE_ROW) ok = sqlite3_column_int(st,0)!=0;
    sqlite3_finalize(st);
    return ok;
}

// Per-type to_json (full struct)
static inline std::string to_json_user_input_(sqlite3* db, const types::UserInput& ui)
{
    std::string extra = bytes_to_string_(ui.extra_data());
    bool ejson = sqlite_json_valid_(db, extra);
    std::string out = "{";
    out += "\"modality\":" + q_(ui.modality()) + ",";
    out += "\"problem_short_description\":" + q_(ui.problem_short_description()) + ",";
    out += "\"problem_definition\":" + q_(ui.problem_definition()) + ",";
    out += "\"inputs\":" + arr_strs_(ui.inputs()) + ",";
    out += "\"outputs\":" + arr_strs_(ui.outputs()) + ",";
    out += "\"minimum_samples\":" + std::to_string(ui.minimum_samples()) + ",";
    out += "\"maximum_samples\":" + std::to_string(ui.maximum_samples()) + ",";
    out += "\"optimize_carbon_footprint_manual\":" + std::string(ui.optimize_carbon_footprint_manual()?"true":"false") + ",";
    out += "\"previous_iteration\":" + std::to_string(ui.previous_iteration()) + ",";
    out += "\"optimize_carbon_footprint_auto\":" + std::string(ui.optimize_carbon_footprint_auto()?"true":"false") + ",";
    out += "\"desired_carbon_footprint\":" + std::to_string(ui.desired_carbon_footprint()) + ",";
    out += "\"geo_location_continent\":" + q_(ui.geo_location_continent()) + ",";
    out += "\"geo_location_region\":" + q_(ui.geo_location_region()) + ",";
    out += "\"extra_data\":" + (ejson ? extra : q_(extra));
    out += "}";
    return out;
}
static inline std::string to_json_app_requirements_(sqlite3* db, const types::AppRequirements& x)
{
    std::string extra = bytes_to_string_(x.extra_data());
    bool ejson = sqlite_json_valid_(db, extra);
    std::string out="{";
    out += "\"app_requirements\":" + arr_strs_(x.app_requirements()) + ",";
    out += "\"extra_data\":" + (ejson ? extra : q_(extra));
    out += "}";
    return out;
}
static inline std::string to_json_ml_model_metadata_(sqlite3* db, const types::MLModelMetadata& x)
{
    std::string extra = bytes_to_string_(x.extra_data());
    bool ejson = sqlite_json_valid_(db, extra);
    std::string out="{";
    out += "\"keywords\":" + arr_strs_(x.keywords()) + ",";
    out += "\"ml_model_metadata\":" + arr_strs_(x.ml_model_metadata()) + ",";
    out += "\"extra_data\":" + (ejson ? extra : q_(extra));
    out += "}";
    return out;
}
static inline std::string to_json_hw_constraints_(sqlite3* db, const types::HWConstraints& x)
{
    std::string extra = bytes_to_string_(x.extra_data());
    bool ejson = sqlite_json_valid_(db, extra);
    std::string out="{";
    out += "\"max_memory_footprint\":" + std::to_string(x.max_memory_footprint()) + ",";
    out += "\"hardware_required\":" + arr_strs_(x.hardware_required()) + ",";
    out += "\"extra_data\":" + (ejson ? extra : q_(extra));
    out += "}";
    return out;
}
static inline std::string to_json_hw_resource_(sqlite3* db, const types::HWResource& x)
{
    std::string extra = bytes_to_string_(x.extra_data());
    bool ejson = sqlite_json_valid_(db, extra);
    std::string out="{";
    out += "\"hw_description\":" + q_(x.hw_description()) + ",";
    out += "\"power_consumption\":" + std::to_string(x.power_consumption()) + ",";
    out += "\"latency\":" + std::to_string(x.latency()) + ",";
    out += "\"memory_footprint_of_ml_model\":" + std::to_string(x.memory_footprint_of_ml_model()) + ",";
    out += "\"max_hw_memory_footprint\":" + std::to_string(x.max_hw_memory_footprint()) + ",";
    out += "\"extra_data\":" + (ejson ? extra : q_(extra));
    out += "}";
    return out;
}
static inline std::string to_json_ml_model_(sqlite3* /*db*/, const types::MLModel& x)
{
    std::string extra = bytes_to_string_(x.extra_data());
    std::string out="{";
    out += "\"model_path\":" + q_(x.model_path()) + ",";
    out += "\"model\":" + q_(x.model()) + ",";
    out += "\"raw_model_b64\":" + q_(base64_encode_(x.raw_model())) + ",";
    out += "\"model_properties_path\":" + q_(x.model_properties_path()) + ",";
    out += "\"model_properties\":" + q_(x.model_properties()) + ",";
    out += "\"input_batch\":" + arr_strs_(x.input_batch()) + ",";
    out += "\"target_latency\":" + std::to_string(x.target_latency()) + ",";
    out += "\"extra_data\":" + q_(extra);
    out += "}";
    return out;
}
static inline std::string to_json_co2_(sqlite3* db, const types::CO2Footprint& x)
{
    std::string extra = bytes_to_string_(x.extra_data());
    bool ejson = sqlite_json_valid_(db, extra);
    std::string out="{";
    out += "\"carbon_footprint\":" + std::to_string(x.carbon_footprint()) + ",";
    out += "\"energy_consumption\":" + std::to_string(x.energy_consumption()) + ",";
    out += "\"carbon_intensity\":" + std::to_string(x.carbon_intensity()) + ",";
    out += "\"extra_data\":" + (ejson ? extra : q_(extra));
    out += "}";
    return out;
}

// --- JSON helpers (parse via SQLite JSON1) ---
static inline bool j_text_(sqlite3* db, const std::string& j, const char* path, std::string& out)
{
    const char* q="SELECT json_extract(?, ?)";
    sqlite3_stmt* st=nullptr;
    if (sqlite3_prepare_v2(db,q,-1,&st,nullptr)!=SQLITE_OK) return false;
    sqlite3_bind_text(st,1,j.c_str(),(int)j.size(),SQLITE_TRANSIENT);
    sqlite3_bind_text(st,2,path,-1,SQLITE_TRANSIENT);
    bool ok=false;
    if (sqlite3_step(st)==SQLITE_ROW)
    {
        auto t = sqlite3_column_text(st,0);
        if (t){ out.assign(reinterpret_cast<const char*>(t)); ok=true; }
    }
    sqlite3_finalize(st); return ok;
}
static inline bool j_i64_(sqlite3* db, const std::string& j, const char* path, long long& out)
{
    const char* q="SELECT json_extract(?, ?)";
    sqlite3_stmt* st=nullptr;
    if (sqlite3_prepare_v2(db,q,-1,&st,nullptr)!=SQLITE_OK) return false;
    sqlite3_bind_text(st,1,j.c_str(),(int)j.size(),SQLITE_TRANSIENT);
    sqlite3_bind_text(st,2,path,-1,SQLITE_TRANSIENT);
    bool ok=false;
    if (sqlite3_step(st)==SQLITE_ROW && sqlite3_column_type(st,0)!=SQLITE_NULL)
    { out=sqlite3_column_int64(st,0); ok=true; }
    sqlite3_finalize(st); return ok;
}
static inline bool j_d_(sqlite3* db, const std::string& j, const char* path, double& out)
{
    const char* q="SELECT json_extract(?, ?)";
    sqlite3_stmt* st=nullptr;
    if (sqlite3_prepare_v2(db,q,-1,&st,nullptr)!=SQLITE_OK) return false;
    sqlite3_bind_text(st,1,j.c_str(),(int)j.size(),SQLITE_TRANSIENT);
    sqlite3_bind_text(st,2,path,-1,SQLITE_TRANSIENT);
    bool ok=false;
    if (sqlite3_step(st)==SQLITE_ROW && sqlite3_column_type(st,0)!=SQLITE_NULL)
    { out=sqlite3_column_double(st,0); ok=true; }
    sqlite3_finalize(st); return ok;
}
static inline void j_arr_text_(sqlite3* db, const std::string& j, const char* path, std::vector<std::string>& out)
{
    out.clear();
    const char* q="SELECT value FROM json_each(?, ?)";
    sqlite3_stmt* st=nullptr;
    if (sqlite3_prepare_v2(db,q,-1,&st,nullptr)!=SQLITE_OK) return;
    sqlite3_bind_text(st,1,j.c_str(),(int)j.size(),SQLITE_TRANSIENT);
    sqlite3_bind_text(st,2,path,-1,SQLITE_TRANSIENT);
    while (sqlite3_step(st)==SQLITE_ROW)
    {
        auto t = sqlite3_column_text(st,0);
        out.emplace_back(t? reinterpret_cast<const char*>(t) : "");
    }
    sqlite3_finalize(st);
}
static inline std::vector<uint8_t> base64_decode_(const std::string& s)
{
    auto val=[](char c)->int{
        if('A'<=c&&c<='Z')return c-'A';
        if('a'<=c&&c<='z')return c-'a'+26;
        if('0'<=c&&c<='9')return c-'0'+52;
        if(c=='+')return 62; if(c=='/')return 63; return -1;
    };
    std::vector<uint8_t> out; int acc=0,bits=0;
    for(char c:s){ if(c=='=')break; int v=val(c); if(v<0)continue; acc=(acc<<6)|v; bits+=6;
        if(bits>=8){ bits-=8; out.push_back((acc>>bits)&0xFF); } }
    return out;
}

// Per-type from_json (full struct)
static inline void from_json_user_input_(sqlite3* db, const std::string& j, types::UserInput& ui)
{
    std::string s; long long i=0; double d=0.0;
    if (j_text_(db,j,"$.modality",s)) ui.modality(s);
    if (j_text_(db,j,"$.problem_short_description",s)) ui.problem_short_description(s);
    if (j_text_(db,j,"$.problem_definition",s)) ui.problem_definition(s);
    std::vector<std::string> v;
    j_arr_text_(db,j,"$.inputs",v); if(!v.empty()) ui.inputs(v);
    j_arr_text_(db,j,"$.outputs",v); if(!v.empty()) ui.outputs(v);
    if (j_i64_(db,j,"$.minimum_samples",i)) ui.minimum_samples((uint32_t)i);
    if (j_i64_(db,j,"$.maximum_samples",i)) ui.maximum_samples((uint32_t)i);
    if (j_i64_(db,j,"$.optimize_carbon_footprint_manual",i)) ui.optimize_carbon_footprint_manual(i!=0);
    if (j_i64_(db,j,"$.previous_iteration",i)) ui.previous_iteration((int32_t)i);
    if (j_i64_(db,j,"$.optimize_carbon_footprint_auto",i)) ui.optimize_carbon_footprint_auto(i!=0);
    if (j_d_(db,j,"$.desired_carbon_footprint",d)) ui.desired_carbon_footprint(d);
    if (j_text_(db,j,"$.geo_location_continent",s)) ui.geo_location_continent(s);
    if (j_text_(db,j,"$.geo_location_region",s)) ui.geo_location_region(s);
    if (j_text_(db,j,"$.extra_data",s)) ui.extra_data(std::vector<uint8_t>(s.begin(), s.end()));
}
static inline void from_json_app_requirements_(sqlite3* db, const std::string& j, types::AppRequirements& x)
{
    std::vector<std::string> v;
    j_arr_text_(db,j,"$.app_requirements",v); if(!v.empty()) x.app_requirements(v);
    std::string s; if (j_text_(db,j,"$.extra_data",s)) x.extra_data(std::vector<uint8_t>(s.begin(), s.end()));
}
static inline void from_json_ml_model_metadata_(sqlite3* db, const std::string& j, types::MLModelMetadata& x)
{
    std::vector<std::string> v;
    j_arr_text_(db,j,"$.keywords",v); if(!v.empty()) x.keywords(v);
    j_arr_text_(db,j,"$.ml_model_metadata",v); if(!v.empty()) x.ml_model_metadata(v);
    std::string s; if (j_text_(db,j,"$.extra_data",s)) x.extra_data(std::vector<uint8_t>(s.begin(), s.end()));
}
static inline void from_json_hw_constraints_(sqlite3* db, const std::string& j, types::HWConstraints& x)
{
    long long i=0; if (j_i64_(db,j,"$.max_memory_footprint",i)) x.max_memory_footprint((uint32_t)i);
    std::vector<std::string> v; j_arr_text_(db,j,"$.hardware_required",v); if(!v.empty()) x.hardware_required(v);
    std::string s; if (j_text_(db,j,"$.extra_data",s)) x.extra_data(std::vector<uint8_t>(s.begin(), s.end()));
}
static inline void from_json_hw_resource_(sqlite3* db, const std::string& j, types::HWResource& x)
{
    std::string s; double d=0.0;
    if (j_text_(db,j,"$.hw_description",s)) x.hw_description(s);
    if (j_d_(db,j,"$.power_consumption",d)) x.power_consumption(d);
    if (j_d_(db,j,"$.latency",d)) x.latency(d);
    if (j_d_(db,j,"$.memory_footprint_of_ml_model",d)) x.memory_footprint_of_ml_model(d);
    if (j_d_(db,j,"$.max_hw_memory_footprint",d)) x.max_hw_memory_footprint(d);
    if (j_text_(db,j,"$.extra_data",s)) x.extra_data(std::vector<uint8_t>(s.begin(), s.end()));
}
static inline void from_json_ml_model_(sqlite3* db, const std::string& j, types::MLModel& x)
{
    std::string s; double d=0.0;
    if (j_text_(db,j,"$.model_path",s)) x.model_path(s);
    if (j_text_(db,j,"$.model",s)) x.model(s);
    if (j_text_(db,j,"$.raw_model_b64",s)) x.raw_model(base64_decode_(s));
    if (j_text_(db,j,"$.model_properties_path",s)) x.model_properties_path(s);
    if (j_text_(db,j,"$.model_properties",s)) x.model_properties(s);
    std::vector<std::string> v; j_arr_text_(db,j,"$.input_batch",v); if(!v.empty()) x.input_batch(v);
    if (j_d_(db,j,"$.target_latency",d)) x.target_latency(d);
    if (j_text_(db,j,"$.extra_data",s)) x.extra_data(std::vector<uint8_t>(s.begin(), s.end()));
}
static inline void from_json_co2_(sqlite3* db, const std::string& j, types::CO2Footprint& x)
{
    double d=0.0;
    if (j_d_(db,j,"$.carbon_footprint",d)) x.carbon_footprint(d);
    if (j_d_(db,j,"$.energy_consumption",d)) x.energy_consumption(d);
    if (j_d_(db,j,"$.carbon_intensity",d)) x.carbon_intensity(d);
    std::string s; if (j_text_(db,j,"$.extra_data",s)) x.extra_data(std::vector<uint8_t>(s.begin(), s.end()));
}

void OrchestratorNode::hydrate_from_db_()
{
    if (!database_)
    {
        return;
    }

    std::vector<sustainml::database::Database::Row> rows;
    if (database_->read_all_rows(rows) != SQLITE_OK)
    {
        EPROSIMA_LOG_WARNING(ORCHESTRATOR, "Database read_all_rows failed");
        return;
    }

    uint32_t max_problem_id = 0;
    sqlite3* dbh = database_->getDB();

    for (const auto& r : rows)
    {
        types::TaskId tid;
        tid.problem_id(static_cast<uint32_t>(r.problem_id));
        tid.iteration_id(static_cast<uint32_t>(r.iteration_id));
        if (tid.problem_id() > max_problem_id) max_problem_id = tid.problem_id();

        std::lock_guard<std::mutex> lock(task_db_->get_mutex());
        task_db_->prepare_new_entry_nts(tid, false);

        { types::UserInput x; x.task_id(tid); from_json_user_input_(dbh, r.user_input_json, x); task_db_->insert_task_data_nts(tid, x); }
        { types::AppRequirements x; x.task_id(tid); from_json_app_requirements_(dbh, r.app_requirements_json, x); task_db_->insert_task_data_nts(tid, x); }
        { types::MLModelMetadata x; x.task_id(tid); from_json_ml_model_metadata_(dbh, r.ml_model_metadata_json, x); task_db_->insert_task_data_nts(tid, x); }
        { types::MLModel x; x.task_id(tid); from_json_ml_model_(dbh, r.ml_model_json, x); task_db_->insert_task_data_nts(tid, x); }
        { types::HWConstraints x; x.task_id(tid); from_json_hw_constraints_(dbh, r.hw_constraints_json, x); task_db_->insert_task_data_nts(tid, x); }
        { types::HWResource x; x.task_id(tid); from_json_hw_resource_(dbh, r.hw_resources_json, x); task_db_->insert_task_data_nts(tid, x); }
        { types::CO2Footprint x; x.task_id(tid); from_json_co2_(dbh, r.carbon_footprint_json, x); task_db_->insert_task_data_nts(tid, x); }

        persisted_task_ids_.push_back(tid);
    }

    if (max_problem_id > 0)
    {
        types::TaskId last_tid;
        last_tid.problem_id(max_problem_id);
        task_man_->update_task_id(last_tid);
    }
}

void OrchestratorNode::persist_to_db_()
{

    if (!database_)
    {
        return;
    }

    auto same = [](const types::TaskId& a, const types::TaskId& b)
    {
        return a.problem_id() == b.problem_id() && a.iteration_id() == b.iteration_id();
    };

    std::vector<types::TaskId> ids;
    for (auto& t : persisted_task_ids_)
    {
        bool found = false;
        for (auto& u : ids) if (same(t,u)) { found = true; break; }
        if (!found) ids.push_back(t);
    }

    std::vector<sustainml::database::Database::Row> rows;
    sqlite3* dbh = database_->getDB();

    {
        std::lock_guard<std::mutex> lock(task_db_->get_mutex());
        for (auto& tid : ids)
        {
            types::UserInput* ui = nullptr;            task_db_->get_task_data_nts(tid, ui);
            types::AppRequirements* app = nullptr;     task_db_->get_task_data_nts(tid, app);
            types::MLModelMetadata* meta = nullptr;    task_db_->get_task_data_nts(tid, meta);
            types::MLModel* mdl = nullptr;             task_db_->get_task_data_nts(tid, mdl);
            types::HWConstraints* hwc = nullptr;       task_db_->get_task_data_nts(tid, hwc);
            types::HWResource* hwr = nullptr;          task_db_->get_task_data_nts(tid, hwr);
            types::CO2Footprint* co2 = nullptr;        task_db_->get_task_data_nts(tid, co2);

            sustainml::database::Database::Row r;
            r.problem_id            = tid.problem_id();
            r.iteration_id          = tid.iteration_id();
            r.user_input_json       = ui  ? to_json_user_input_(dbh, *ui)           : "{}";
            r.app_requirements_json = app ? to_json_app_requirements_(dbh, *app)    : "{}";
            r.ml_model_metadata_json= meta? to_json_ml_model_metadata_(dbh, *meta)  : "{}";
            r.ml_model_json         = mdl ? to_json_ml_model_(dbh, *mdl)            : "{}";
            r.hw_constraints_json   = hwc ? to_json_hw_constraints_(dbh, *hwc)      : "{}";
            r.hw_resources_json     = hwr ? to_json_hw_resource_(dbh, *hwr)         : "{}";
            r.carbon_footprint_json = co2 ? to_json_co2_(dbh, *co2)                 : "{}";

            rows.emplace_back(std::move(r));
        }

    }

    if (database_->replace_all_rows(rows) != SQLITE_OK)
    {
        EPROSIMA_LOG_WARNING(ORCHESTRATOR, "Database replace_all_rows failed");
    }
}

// Track new tasks for persistence
std::pair<types::TaskId, types::UserInput*> OrchestratorNode::prepare_new_task()
{
    std::pair<types::TaskId, types::UserInput*> output;
    auto task_id = task_man_->create_new_task_id();
    {
        std::lock_guard<std::mutex> lock(task_db_->get_mutex());
        task_db_->prepare_new_entry_nts(task_id, false);
        task_db_->get_task_data_nts(task_id, output.second);
    }
    output.first = task_id;
    persisted_task_ids_.push_back(task_id);
    return output;
}

std::pair<types::TaskId, types::UserInput*> OrchestratorNode::prepare_new_iteration(
        const types::TaskId& old_task_id,
        const types::TaskId& last_task_id)
{
    std::pair<types::TaskId, types::UserInput*> output;
    types::TaskId new_task_id(old_task_id);
    new_task_id.iteration_id(last_task_id.iteration_id() + 1);
    {
        std::lock_guard<std::mutex> lock(task_db_->get_mutex());
        task_db_->prepare_new_entry_nts(new_task_id, true);
        // Copy the UserInput from the previous iteration
        // it also updates the iteration_id in the data
        task_db_->copy_data_nts(old_task_id, new_task_id, {NodeID::ID_ORCHESTRATOR});
        task_db_->get_task_data_nts(new_task_id, output.second);
    }
    output.first = new_task_id;
    persisted_task_ids_.push_back(new_task_id);
    return output;
}

bool OrchestratorNode::start_task(
        const types::TaskId& task_id,
        types::UserInput* ui)
{
    user_input_writer_->write(ui->get_impl());
    publish_baselines(task_id);
    return true;
}

bool OrchestratorNode::start_iteration(
        const types::TaskId& task_id,
        types::UserInput* ui)
{
    user_input_writer_->write(ui->get_impl());
    publish_baselines(task_id);
    return true;
}

void OrchestratorNode::publish_baselines(
        const types::TaskId& task_id)
{
    //publish in the iteration topics
    for (size_t i = 0; i < (size_t)NodeID::MAX; i++)
    {
        std::lock_guard<std::mutex> lock(proxies_mtx_);
        if (node_proxies_[i] != nullptr &&
                node_proxies_[i]->publishes_baseline())
        {
            node_proxies_[i]->publish_data_for_iteration(task_id);
        }
    }
}

RetCode_t OrchestratorNode::get_task_data(
        const types::TaskId& task_id,
        const NodeID& node_id,
        void*& data)
{
    RetCode_t ret = RetCode_t::RETCODE_NO_DATA;

    std::lock_guard<std::mutex> lock(task_db_->get_mutex());

    switch (node_id)
    {
        case NodeID::ID_ML_MODEL_METADATA:
        {
            MapFromNodeIDToType_t<NodeID::ID_ML_MODEL_METADATA>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_ML_MODEL:
        {
            MapFromNodeIDToType_t<NodeID::ID_ML_MODEL>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_HW_RESOURCES:
        {
            MapFromNodeIDToType_t<NodeID::ID_HW_RESOURCES>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_CARBON_FOOTPRINT:
        {
            MapFromNodeIDToType_t<NodeID::ID_CARBON_FOOTPRINT>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_HW_CONSTRAINTS:
        {
            MapFromNodeIDToType_t<NodeID::ID_HW_CONSTRAINTS>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_APP_REQUIREMENTS:
        {
            MapFromNodeIDToType_t<NodeID::ID_APP_REQUIREMENTS>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_ORCHESTRATOR:
        {
            MapFromNodeIDToType_t<NodeID::ID_ORCHESTRATOR>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        default:
        {
            EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Requested Data from non-existing node ID");
            break;
        }
    }

    return ret;
}

RetCode_t OrchestratorNode::get_node_status (
        const NodeID& node_id,
        const types::NodeStatus*& status)
{
    RetCode_t ret = RetCode_t::RETCODE_NO_DATA;

    if ((int)node_id >= 0 && node_id < NodeID::MAX)
    {
        std::lock_guard<std::mutex> lock(proxies_mtx_);
        status = &node_proxies_[(int)node_id]->get_status();
        ret = RetCode_t::RETCODE_OK;
    }

    return ret;
}

void OrchestratorNode::send_control_command(
        const types::NodeControl& cmd)
{
    control_writer_->write(cmd.get_impl());
}

types::ResponseType OrchestratorNode::configuration_request (
        const types::RequestType& req)
{
    req_res_->write_req(req.get_impl());
    types::ResponseType user_res;
    req_res_->wait_until([this, &req]
            {
                bool is_expected_response =
                (res_.node_id() == req.node_id() && res_.transaction_id() == req.transaction_id()) || terminate_.load();
                if (!is_expected_response)
                {
                    req_res_->resume_taking_data();
                }
                return is_expected_response;
            });

    if (!terminate_.load())
    {
        user_res = res_;
        req_res_->resume_taking_data();
    }
    else
    {
        EPROSIMA_LOG_WARNING(ORCHESTRATOR, "Orchestrator is terminating, no response will be sent");
    }

    return user_res;
}

void OrchestratorNode::spin()
{
    EPROSIMA_LOG_INFO(ORCHESTRATOR, "Spinning Orchestrator... ");
    std::unique_lock<std::mutex> lock(mtx_);
    spin_cv_.wait(lock, [&]
            {
                return terminate_.load();
            });
}

void OrchestratorNode::terminate()
{
    if (terminated_.load()) {
        return;
    }

    terminate_.store(true);
    // skip the wait in the replier if any request is pending
    req_res_->resume_taking_data();

    persist_to_db_();
    if (database_) { database_->close(); }
    destroy();
    spin_cv_.notify_all();
}

std::vector<types::TaskId> OrchestratorNode::get_all_task_ids()
{
    auto same = [](const types::TaskId& a, const types::TaskId& b)
    {
        return a.problem_id() == b.problem_id() && a.iteration_id() == b.iteration_id();
    };

    std::vector<types::TaskId> out;
    {
        out = persisted_task_ids_;
    }

    std::sort(out.begin(), out.end(), [](const types::TaskId& a, const types::TaskId& b)
    {
        if (a.problem_id() != b.problem_id()) return a.problem_id() < b.problem_id();
        return a.iteration_id() < b.iteration_id();
    });
    out.erase(std::unique(out.begin(), out.end(), same), out.end());
    return out;
}

} // namespace orchestrator
} // namespace sustainml

