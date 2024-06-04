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
 * @file types.h
 *
 */

#ifndef _FAST_DDS_TYPES_H_
#define _FAST_DDS_TYPES_H_

#include <array>
#include <bitset>
#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <fastcdr/cdr/fixed_size_string.hpp>
#include <fastcdr/xcdr/external.hpp>
#include <fastcdr/xcdr/optional.hpp>


#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(TYPES_SOURCE)
#define TYPES_DllAPI __declspec( dllexport )
#else
#define TYPES_DllAPI __declspec( dllimport )
#endif // TYPES_SOURCE
#else
#define TYPES_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define TYPES_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
class CdrSizeCalculator;
} // namespace fastcdr
} // namespace eprosima

class AppRequirementsImpl;
class CO2FootprintImpl;
class GeoLocationImpl;
class HWConstraintsImpl;
class HWResourceImpl;
class MLModelImpl;
class MLModelMetadataImpl;
class NodeControlImpl;
class NodeStatusImpl;
class TaskIdImpl;
class UserInputImpl;

enum Status : uint32_t;
enum TaskStatus : uint32_t;
enum ErrorCode : uint32_t;
enum CmdNode : uint32_t;
enum CmdTask : uint32_t;

namespace types {
/*!
 * @brief This class represents the structure TaskId defined by the user in the IDL file.
 * @ingroup typesImpl
 */
class TaskId
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport TaskId();

    /*!
     * @brief Initial values TaskId constructor.
     */
    eProsima_user_DllExport TaskId(
            uint32_t problem_id,
            uint32_t iteration_id);

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~TaskId();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object TaskId that will be copied.
     */
    eProsima_user_DllExport TaskId(
            const TaskId& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object TaskId that will be copied.
     */
    eProsima_user_DllExport TaskId(
            TaskId&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object TaskId that will be copied.
     */
    eProsima_user_DllExport TaskId& operator =(
            const TaskId& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object TaskId that will be copied.
     */
    eProsima_user_DllExport TaskId& operator =(
            TaskId&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x TaskId object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const TaskId& x) const;

    /*!
     * @brief Comparison operator.
     * @param x TaskId object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const TaskId& x) const;

    /*!
     * @brief Less than operator.
     */
    eProsima_user_DllExport bool operator <(
            const TaskId& x) const;

    /*!
     * @brief This function sets a value in member problem_id
     * @param _problem_id New value for member problem_id
     */
    eProsima_user_DllExport void problem_id(
            uint32_t _problem_id);

    /*!
     * @brief This function returns the value of member problem_id
     * @return Value of member problem_id
     */
    eProsima_user_DllExport uint32_t problem_id() const;

    /*!
     * @brief This function returns a reference to member problem_id
     * @return Reference to member problem_id
     */
    eProsima_user_DllExport uint32_t& problem_id();


    /*!
     * @brief This function sets a value in member iteration_id
     * @param _iteration_id New value for member iteration_id
     */
    eProsima_user_DllExport void iteration_id(
            uint32_t _iteration_id);

    /*!
     * @brief This function returns the value of member iteration_id
     * @return Value of member iteration_id
     */
    eProsima_user_DllExport uint32_t iteration_id() const;

    /*!
     * @brief This function returns a reference to member iteration_id
     * @return Reference to member iteration_id
     */
    eProsima_user_DllExport uint32_t& iteration_id();

    /*!
     * @brief This function overloads the operator << for the TaskId type.
     */
    friend std::ostream& operator << (
            std::ostream& stream,
            const TaskId& task_id);

private:

    uint32_t problem_id_;
    uint32_t iteration_id_;
};

/**
 * @brief Method that converts a TaskId to a TaskIdImpl.
 *
 * @param impl The TaskIdImpl that will contain the information of the TaskId.
 * @return TaskId* The TaskId that will be converted.
 * @warning This function is assuming that both classes have the same data representation in memory
 */
inline TaskId* to_task_id(
        TaskIdImpl* impl)
{
    return reinterpret_cast<TaskId*>(impl);
}

/**
 * @brief Method that converts a TaskIdImpl to a TaskId.
 *
 * @param impl The TaskId that will contain the information of the TaskIdImpl.
 * @return TaskIdImpl* The TaskIdImpl that will be converted.
 * @warning This function is assuming that both classes have the same data representation in memory
 */
inline TaskIdImpl* to_task_id_impl(
        TaskId* impl)
{
    return reinterpret_cast<TaskIdImpl*>(impl);
}

/*!
 * @brief This class represents the structure AppRequirements defined by the user in the IDL file.
 * @ingroup types
 */
class AppRequirements
{
public:

    using impl_type = AppRequirementsImpl;

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport AppRequirements();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~AppRequirements();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object AppRequirements that will be copied.
     */
    eProsima_user_DllExport AppRequirements(
            const AppRequirements& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object AppRequirements that will be copied.
     */
    eProsima_user_DllExport AppRequirements(
            AppRequirements&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object AppRequirements that will be copied.
     */
    eProsima_user_DllExport AppRequirements& operator =(
            const AppRequirements& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object AppRequirements that will be copied.
     */
    eProsima_user_DllExport AppRequirements& operator =(
            AppRequirements&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x AppRequirements object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const AppRequirements& x) const;

    /*!
     * @brief Comparison operator.
     * @param x AppRequirements object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const AppRequirements& x) const;

    /*!
     * @brief This function copies the value in member app_requirements
     * @param _app_requirements New value to be copied in member app_requirements
     */
    eProsima_user_DllExport void app_requirements(
            const std::vector<std::string>& _app_requirements);

    /*!
     * @brief This function moves the value in member app_requirements
     * @param _app_requirements New value to be moved in member app_requirements
     */
    eProsima_user_DllExport void app_requirements(
            std::vector<std::string>&& _app_requirements);

    /*!
     * @brief This function returns a constant reference to member app_requirements
     * @return Constant reference to member app_requirements
     */
    eProsima_user_DllExport const std::vector<std::string>& app_requirements() const;

    /*!
     * @brief This function returns a reference to member app_requirements
     * @return Reference to member app_requirements
     */
    eProsima_user_DllExport std::vector<std::string>& app_requirements();


    /*!
     * @brief This function copies the value in member extra_data
     * @param _extra_data New value to be copied in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            const std::vector<uint8_t>& _extra_data);

    /*!
     * @brief This function moves the value in member extra_data
     * @param _extra_data New value to be moved in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            std::vector<uint8_t>&& _extra_data);

    /*!
     * @brief This function returns a constant reference to member extra_data
     * @return Constant reference to member extra_data
     */
    eProsima_user_DllExport const std::vector<uint8_t>& extra_data() const;

    /*!
     * @brief This function returns a reference to member extra_data
     * @return Reference to member extra_data
     */
    eProsima_user_DllExport std::vector<uint8_t>& extra_data();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    AppRequirementsImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

    /*!
     * @brief This function retrives the implementation type info
     * @return Reference to the typeid
     */
    static const std::type_info& impl_typeinfo();

protected:

    AppRequirementsImpl* impl_;
    friend class AppRequirementsImpl;

};

/*!
 * @brief This class represents the structure CO2Footprint defined by the user in the IDL file.
 * @ingroup types
 */
class CO2Footprint
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport CO2Footprint();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~CO2Footprint();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object CO2Footprint that will be copied.
     */
    eProsima_user_DllExport CO2Footprint(
            const CO2Footprint& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object CO2Footprint that will be copied.
     */
    eProsima_user_DllExport CO2Footprint(
            CO2Footprint&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object CO2Footprint that will be copied.
     */
    eProsima_user_DllExport CO2Footprint& operator =(
            const CO2Footprint& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object CO2Footprint that will be copied.
     */
    eProsima_user_DllExport CO2Footprint& operator =(
            CO2Footprint&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x CO2Footprint object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const CO2Footprint& x) const;

    /*!
     * @brief Comparison operator.
     * @param x CO2Footprint object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const CO2Footprint& x) const;

    /*!
     * @brief This function sets a value in member carbon_footprint
     * @param _carbon_footprint New value for member carbon_footprint
     */
    eProsima_user_DllExport void carbon_footprint(
            double _carbon_footprint);

    /*!
     * @brief This function returns the value of member carbon_footprint
     * @return Value of member carbon_footprint
     */
    eProsima_user_DllExport double carbon_footprint() const;

    /*!
     * @brief This function returns a reference to member carbon_footprint
     * @return Reference to member carbon_footprint
     */
    eProsima_user_DllExport double& carbon_footprint();


    /*!
     * @brief This function sets a value in member energy_consumption
     * @param _energy_consumption New value for member energy_consumption
     */
    eProsima_user_DllExport void energy_consumption(
            double _energy_consumption);

    /*!
     * @brief This function returns the value of member energy_consumption
     * @return Value of member energy_consumption
     */
    eProsima_user_DllExport double energy_consumption() const;

    /*!
     * @brief This function returns a reference to member energy_consumption
     * @return Reference to member energy_consumption
     */
    eProsima_user_DllExport double& energy_consumption();


    /*!
     * @brief This function sets a value in member carbon_intensity
     * @param _carbon_intensity New value for member carbon_intensity
     */
    eProsima_user_DllExport void carbon_intensity(
            double _carbon_intensity);

    /*!
     * @brief This function returns the value of member carbon_intensity
     * @return Value of member carbon_intensity
     */
    eProsima_user_DllExport double carbon_intensity() const;

    /*!
     * @brief This function returns a reference to member carbon_intensity
     * @return Reference to member carbon_intensity
     */
    eProsima_user_DllExport double& carbon_intensity();


    /*!
     * @brief This function copies the value in member extra_data
     * @param _extra_data New value to be copied in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            const std::vector<uint8_t>& _extra_data);

    /*!
     * @brief This function moves the value in member extra_data
     * @param _extra_data New value to be moved in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            std::vector<uint8_t>&& _extra_data);

    /*!
     * @brief This function returns a constant reference to member extra_data
     * @return Constant reference to member extra_data
     */
    eProsima_user_DllExport const std::vector<uint8_t>& extra_data() const;

    /*!
     * @brief This function returns a reference to member extra_data
     * @return Reference to member extra_data
     */
    eProsima_user_DllExport std::vector<uint8_t>& extra_data();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    CO2FootprintImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

    /*!
     * @brief This function retrives the implementation type info
     * @return Reference to the typeid
     */
    static const std::type_info& impl_typeinfo();

protected:

    CO2FootprintImpl* impl_;
    friend class CO2FootprintImpl;

};
/*!
 * @brief This class represents the structure HWConstraints defined by the user in the IDL file.
 * @ingroup typesImpl
 */
class HWConstraints
{
public:

    using impl_type = HWConstraintsImpl;

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport HWConstraints();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~HWConstraints();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object HWConstraints that will be copied.
     */
    eProsima_user_DllExport HWConstraints(
            const HWConstraints& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object HWConstraints that will be copied.
     */
    eProsima_user_DllExport HWConstraints(
            HWConstraints&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object HWConstraints that will be copied.
     */
    eProsima_user_DllExport HWConstraints& operator =(
            const HWConstraints& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object HWConstraints that will be copied.
     */
    eProsima_user_DllExport HWConstraints& operator =(
            HWConstraints&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x HWConstraints object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const HWConstraints& x) const;

    /*!
     * @brief Comparison operator.
     * @param x HWConstraints object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const HWConstraints& x) const;

    /*!
     * @brief This function sets a value in member max_memory_footprint
     * @param _max_memory_footprint New value for member max_memory_footprint
     */
    eProsima_user_DllExport void max_memory_footprint(
            uint32_t _max_memory_footprint);

    /*!
     * @brief This function returns the value of member max_memory_footprint
     * @return Value of member max_memory_footprint
     */
    eProsima_user_DllExport uint32_t max_memory_footprint() const;

    /*!
     * @brief This function returns a reference to member max_memory_footprint
     * @return Reference to member max_memory_footprint
     */
    eProsima_user_DllExport uint32_t& max_memory_footprint();


    /*!
     * @brief This function copies the value in member extra_data
     * @param _extra_data New value to be copied in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            const std::vector<uint8_t>& _extra_data);

    /*!
     * @brief This function moves the value in member extra_data
     * @param _extra_data New value to be moved in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            std::vector<uint8_t>&& _extra_data);

    /*!
     * @brief This function returns a constant reference to member extra_data
     * @return Constant reference to member extra_data
     */
    eProsima_user_DllExport const std::vector<uint8_t>& extra_data() const;

    /*!
     * @brief This function returns a reference to member extra_data
     * @return Reference to member extra_data
     */
    eProsima_user_DllExport std::vector<uint8_t>& extra_data();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    HWConstraintsImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

    /*!
     * @brief This function retrives the implementation type info
     * @return Reference to the typeid
     */
    static const std::type_info& impl_typeinfo();

protected:

    HWConstraintsImpl* impl_;
    friend class HWConstraintsImpl;

};
/*!
 * @brief This class represents the structure HWResource defined by the user in the IDL file.
 * @ingroup types
 */
class HWResource
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport HWResource();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~HWResource();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object HWResource that will be copied.
     */
    eProsima_user_DllExport HWResource(
            const HWResource& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object HWResource that will be copied.
     */
    eProsima_user_DllExport HWResource(
            HWResource&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object HWResource that will be copied.
     */
    eProsima_user_DllExport HWResource& operator =(
            const HWResource& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object HWResource that will be copied.
     */
    eProsima_user_DllExport HWResource& operator =(
            HWResource&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x HWResource object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const HWResource& x) const;

    /*!
     * @brief Comparison operator.
     * @param x HWResource object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const HWResource& x) const;

    /*!
     * @brief This function copies the value in member hw_description
     * @param _hw_description New value to be copied in member hw_description
     */
    eProsima_user_DllExport void hw_description(
            const std::string& _hw_description);

    /*!
     * @brief This function moves the value in member hw_description
     * @param _hw_description New value to be moved in member hw_description
     */
    eProsima_user_DllExport void hw_description(
            std::string&& _hw_description);

    /*!
     * @brief This function returns a constant reference to member hw_description
     * @return Constant reference to member hw_description
     */
    eProsima_user_DllExport const std::string& hw_description() const;

    /*!
     * @brief This function returns a copy of the hw_description member
     * @return Copy of the hw_description member
     */
    eProsima_user_DllExport std::string& hw_description();


    /*!
     * @brief This function sets a value in member power_consumption
     * @param _power_consumption New value for member power_consumption
     */
    eProsima_user_DllExport void power_consumption(
            double _power_consumption);

    /*!
     * @brief This function returns the value of member power_consumption
     * @return Value of member power_consumption
     */
    eProsima_user_DllExport double power_consumption() const;

    /*!
     * @brief This function returns a reference to member power_consumption
     * @return Reference to member power_consumption
     */
    eProsima_user_DllExport double& power_consumption();


    /*!
     * @brief This function sets a value in member latency
     * @param _latency New value for member latency
     */
    eProsima_user_DllExport void latency(
            double _latency);

    /*!
     * @brief This function returns the value of member latency
     * @return Value of member latency
     */
    eProsima_user_DllExport double latency() const;

    /*!
     * @brief This function returns a reference to member latency
     * @return Reference to member latency
     */
    eProsima_user_DllExport double& latency();


    /*!
     * @brief This function sets a value in member memory_footprint_of_ml_model
     * @param _memory_footprint_of_ml_model New value for member memory_footprint_of_ml_model
     */
    eProsima_user_DllExport void memory_footprint_of_ml_model(
            double _memory_footprint_of_ml_model);

    /*!
     * @brief This function returns the value of member memory_footprint_of_ml_model
     * @return Value of member memory_footprint_of_ml_model
     */
    eProsima_user_DllExport double memory_footprint_of_ml_model() const;

    /*!
     * @brief This function returns a reference to member memory_footprint_of_ml_model
     * @return Reference to member memory_footprint_of_ml_model
     */
    eProsima_user_DllExport double& memory_footprint_of_ml_model();


    /*!
     * @brief This function sets a value in member max_hw_memory_footprint
     * @param _max_hw_memory_footprint New value for member max_hw_memory_footprint
     */
    eProsima_user_DllExport void max_hw_memory_footprint(
            double _max_hw_memory_footprint);

    /*!
     * @brief This function returns the value of member max_hw_memory_footprint
     * @return Value of member max_hw_memory_footprint
     */
    eProsima_user_DllExport double max_hw_memory_footprint() const;

    /*!
     * @brief This function returns a reference to member max_hw_memory_footprint
     * @return Reference to member max_hw_memory_footprint
     */
    eProsima_user_DllExport double& max_hw_memory_footprint();


    /*!
     * @brief This function copies the value in member extra_data
     * @param _extra_data New value to be copied in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            const std::vector<uint8_t>& _extra_data);

    /*!
     * @brief This function moves the value in member extra_data
     * @param _extra_data New value to be moved in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            std::vector<uint8_t>&& _extra_data);

    /*!
     * @brief This function returns a constant reference to member extra_data
     * @return Constant reference to member extra_data
     */
    eProsima_user_DllExport const std::vector<uint8_t>& extra_data() const;

    /*!
     * @brief This function returns a reference to member extra_data
     * @return Reference to member extra_data
     */
    eProsima_user_DllExport std::vector<uint8_t>& extra_data();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    HWResourceImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

    /*!
     * @brief This function retrives the implementation type info
     * @return Reference to the typeid
     */
    static const std::type_info& impl_typeinfo();

protected:

    HWResourceImpl* impl_;
    friend class HWResourceImpl;

};
/*!
 * @brief This class represents the structure MLModel defined by the user in the IDL file.
 * @ingroup types
 */
class MLModel
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport MLModel();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~MLModel();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object MLModel that will be copied.
     */
    eProsima_user_DllExport MLModel(
            const MLModel& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object MLModel that will be copied.
     */
    eProsima_user_DllExport MLModel(
            MLModel&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object MLModel that will be copied.
     */
    eProsima_user_DllExport MLModel& operator =(
            const MLModel& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object MLModel that will be copied.
     */
    eProsima_user_DllExport MLModel& operator =(
            MLModel&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x MLModel object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const MLModel& x) const;

    /*!
     * @brief Comparison operator.
     * @param x MLModel object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const MLModel& x) const;

    /*!
     * @brief This function copies the value in member model_path
     * @param _model_path New value to be copied in member model_path
     */
    eProsima_user_DllExport void model_path(
            const std::string& _model_path);

    /*!
     * @brief This function moves the value in member model_path
     * @param _model_path New value to be moved in member model_path
     */
    eProsima_user_DllExport void model_path(
            std::string&& _model_path);

    /*!
     * @brief This function returns a constant reference to member model_path
     * @return Constant reference to member model_path
     */
    eProsima_user_DllExport const std::string& model_path() const;

    /*!
     * @brief This function returns a copy of the model_path member
     * @return Copy of the model_path member
     */
    eProsima_user_DllExport std::string& model_path();


    /*!
     * @brief This function copies the value in member model
     * @param _model New value to be copied in member model
     */
    eProsima_user_DllExport void model(
            const std::string& _model);

    /*!
     * @brief This function moves the value in member model
     * @param _model New value to be moved in member model
     */
    eProsima_user_DllExport void model(
            std::string&& _model);

    /*!
     * @brief This function returns a constant reference to member model
     * @return Constant reference to member model
     */
    eProsima_user_DllExport const std::string& model() const;

    /*!
     * @brief This function returns a copy of the model member
     * @return Copy of the model member
     */
    eProsima_user_DllExport std::string& model();

    /*!
     * @brief This function copies the value in member raw_model
     * @param _raw_model New value to be copied in member raw_model
     */
    eProsima_user_DllExport void raw_model(
            const std::vector<uint8_t>& _raw_model);

    /*!
     * @brief This function moves the value in member raw_model
     * @param _raw_model New value to be moved in member raw_model
     */
    eProsima_user_DllExport void raw_model(
            std::vector<uint8_t>&& _raw_model);

    /*!
     * @brief This function returns a constant reference to member raw_model
     * @return Constant reference to member raw_model
     */
    eProsima_user_DllExport const std::vector<uint8_t>& raw_model() const;

    /*!
     * @brief This function returns a reference to member raw_model
     * @return Reference to member raw_model
     */
    eProsima_user_DllExport std::vector<uint8_t>& raw_model();

    /*!
     * @brief This function copies the value in member model_properties_path
     * @param _model_properties_path New value to be copied in member model_properties_path
     */
    eProsima_user_DllExport void model_properties_path(
            const std::string& _model_properties_path);

    /*!
     * @brief This function moves the value in member model_properties_path
     * @param _model_properties_path New value to be moved in member model_properties_path
     */
    eProsima_user_DllExport void model_properties_path(
            std::string&& _model_properties_path);

    /*!
     * @brief This function returns a constant reference to member model_properties_path
     * @return Constant reference to member model_properties_path
     */
    eProsima_user_DllExport const std::string& model_properties_path() const;

    /*!
     * @brief This function returns a copy of the model_properties_path member
     * @return Copy of the model_properties_path member
     */
    eProsima_user_DllExport std::string& model_properties_path();


    /*!
     * @brief This function copies the value in member model_properties
     * @param _model_properties New value to be copied in member model_properties
     */
    eProsima_user_DllExport void model_properties(
            const std::string& _model_properties);

    /*!
     * @brief This function moves the value in member model_properties
     * @param _model_properties New value to be moved in member model_properties
     */
    eProsima_user_DllExport void model_properties(
            std::string&& _model_properties);

    /*!
     * @brief This function returns a constant reference to member model_properties
     * @return Constant reference to member model_properties
     */
    eProsima_user_DllExport const std::string& model_properties() const;

    /*!
     * @brief This function returns a copy of the model_properties member
     * @return Copy of the model_properties member
     */
    eProsima_user_DllExport std::string& model_properties();


    /*!
     * @brief This function copies the value in member input_batch
     * @param _input_batch New value to be copied in member input_batch
     */
    eProsima_user_DllExport void input_batch(
            const std::vector<std::string>& _input_batch);

    /*!
     * @brief This function moves the value in member input_batch
     * @param _input_batch New value to be moved in member input_batch
     */
    eProsima_user_DllExport void input_batch(
            std::vector<std::string>&& _input_batch);

    /*!
     * @brief This function returns a constant reference to member input_batch
     * @return Constant reference to member input_batch
     */
    eProsima_user_DllExport const std::vector<std::string>& input_batch() const;

    /*!
     * @brief This function returns a reference to member input_batch
     * @return Reference to member input_batch
     */
    eProsima_user_DllExport std::vector<std::string>& input_batch();


    /*!
     * @brief This function sets a value in member target_latency
     * @param _target_latency New value for member target_latency
     */
    eProsima_user_DllExport void target_latency(
            double _target_latency);

    /*!
     * @brief This function returns the value of member target_latency
     * @return Value of member target_latency
     */
    eProsima_user_DllExport double target_latency() const;

    /*!
     * @brief This function returns a reference to member target_latency
     * @return Reference to member target_latency
     */
    eProsima_user_DllExport double& target_latency();


    /*!
     * @brief This function copies the value in member extra_data
     * @param _extra_data New value to be copied in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            const std::vector<uint8_t>& _extra_data);

    /*!
     * @brief This function moves the value in member extra_data
     * @param _extra_data New value to be moved in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            std::vector<uint8_t>&& _extra_data);

    /*!
     * @brief This function returns a constant reference to member extra_data
     * @return Constant reference to member extra_data
     */
    eProsima_user_DllExport const std::vector<uint8_t>& extra_data() const;

    /*!
     * @brief This function returns a reference to member extra_data
     * @return Reference to member extra_data
     */
    eProsima_user_DllExport std::vector<uint8_t>& extra_data();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    MLModelImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

    /*!
     * @brief This function retrives the implementation type info
     * @return Reference to the typeid
     */
    static const std::type_info& impl_typeinfo();

protected:

    MLModelImpl* impl_;
    friend class MLModelImpl;

};
/*!
 * @brief This class represents the structure MLModelMetadata defined by the user in the IDL file.
 * @ingroup types
 */
class MLModelMetadata
{
public:

    using impl_type = MLModelMetadataImpl;

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport MLModelMetadata();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~MLModelMetadata();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object MLModelMetadata that will be copied.
     */
    eProsima_user_DllExport MLModelMetadata(
            const MLModelMetadata& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object MLModelMetadata that will be copied.
     */
    eProsima_user_DllExport MLModelMetadata(
            MLModelMetadata&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object MLModelMetadata that will be copied.
     */
    eProsima_user_DllExport MLModelMetadata& operator =(
            const MLModelMetadata& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object MLModelMetadata that will be copied.
     */
    eProsima_user_DllExport MLModelMetadata& operator =(
            MLModelMetadata&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x MLModelMetadata object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const MLModelMetadata& x) const;

    /*!
     * @brief Comparison operator.
     * @param x MLModelMetadata object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const MLModelMetadata& x) const;

    /*!
     * @brief This function copies the value in member keywords
     * @param _keywords New value to be copied in member keywords
     */
    eProsima_user_DllExport void keywords(
            const std::vector<std::string>& _keywords);

    /*!
     * @brief This function moves the value in member keywords
     * @param _keywords New value to be moved in member keywords
     */
    eProsima_user_DllExport void keywords(
            std::vector<std::string>&& _keywords);

    /*!
     * @brief This function returns a constant reference to member keywords
     * @return Constant reference to member keywords
     */
    eProsima_user_DllExport const std::vector<std::string>& keywords() const;

    /*!
     * @brief This function returns a reference to member keywords
     * @return Reference to member keywords
     */
    eProsima_user_DllExport std::vector<std::string>& keywords();

    /*!
     * @brief This function copies the value in member ml_model_metadata
     * @param _ml_model_metadata New value to be copied in member ml_model_metadata
     */
    eProsima_user_DllExport void ml_model_metadata(
            const std::vector<std::string>& _ml_model_metadata);

    /*!
     * @brief This function moves the value in member ml_model_metadata
     * @param _ml_model_metadata New value to be moved in member ml_model_metadata
     */
    eProsima_user_DllExport void ml_model_metadata(
            std::vector<std::string>&& _ml_model_metadata);

    /*!
     * @brief This function returns a constant reference to member ml_model_metadata
     * @return Constant reference to member ml_model_metadata
     */
    eProsima_user_DllExport const std::vector<std::string>& ml_model_metadata() const;

    /*!
     * @brief This function returns a reference to member ml_model_metadata
     * @return Reference to member ml_model_metadata
     */
    eProsima_user_DllExport std::vector<std::string>& ml_model_metadata();


    /*!
     * @brief This function copies the value in member extra_data
     * @param _extra_data New value to be copied in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            const std::vector<uint8_t>& _extra_data);

    /*!
     * @brief This function moves the value in member extra_data
     * @param _extra_data New value to be moved in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            std::vector<uint8_t>&& _extra_data);

    /*!
     * @brief This function returns a constant reference to member extra_data
     * @return Constant reference to member extra_data
     */
    eProsima_user_DllExport const std::vector<uint8_t>& extra_data() const;

    /*!
     * @brief This function returns a reference to member extra_data
     * @return Reference to member extra_data
     */
    eProsima_user_DllExport std::vector<uint8_t>& extra_data();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    MLModelMetadataImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

    /*!
     * @brief This function retrives the implementation type info
     * @return Reference to the typeid
     */
    static const std::type_info& impl_typeinfo();

protected:

    MLModelMetadataImpl* impl_;
    friend class MLModelMetadataImpl;

};
/*!
 * @brief This class represents the structure NodeControl defined by the user in the IDL file.
 * @ingroup types
 */
class NodeControl
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport NodeControl();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~NodeControl();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object NodeControl that will be copied.
     */
    eProsima_user_DllExport NodeControl(
            const NodeControl& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object NodeControl that will be copied.
     */
    eProsima_user_DllExport NodeControl(
            NodeControl&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object NodeControl that will be copied.
     */
    eProsima_user_DllExport NodeControl& operator =(
            const NodeControl& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object NodeControl that will be copied.
     */
    eProsima_user_DllExport NodeControl& operator =(
            NodeControl&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x NodeControl object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const NodeControl& x) const;

    /*!
     * @brief Comparison operator.
     * @param x NodeControl object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const NodeControl& x) const;

    /*!
     * @brief This function sets a value in member cmd_node
     * @param _cmd_node New value for member cmd_node
     */
    eProsima_user_DllExport void cmd_node(
            CmdNode _cmd_node);

    /*!
     * @brief This function returns the value of member cmd_node
     * @return Value of member cmd_node
     */
    eProsima_user_DllExport CmdNode cmd_node() const;

    /*!
     * @brief This function returns a reference to member cmd_node
     * @return Reference to member cmd_node
     */
    eProsima_user_DllExport CmdNode& cmd_node();


    /*!
     * @brief This function sets a value in member cmd_task
     * @param _cmd_task New value for member cmd_task
     */
    eProsima_user_DllExport void cmd_task(
            CmdTask _cmd_task);

    /*!
     * @brief This function returns the value of member cmd_task
     * @return Value of member cmd_task
     */
    eProsima_user_DllExport CmdTask cmd_task() const;

    /*!
     * @brief This function returns a reference to member cmd_task
     * @return Reference to member cmd_task
     */
    eProsima_user_DllExport CmdTask& cmd_task();


    /*!
     * @brief This function copies the value in member target_node
     * @param _target_node New value to be copied in member target_node
     */
    eProsima_user_DllExport void target_node(
            const std::string& _target_node);

    /*!
     * @brief This function moves the value in member target_node
     * @param _target_node New value to be moved in member target_node
     */
    eProsima_user_DllExport void target_node(
            std::string&& _target_node);

    /*!
     * @brief This function returns a constant reference to member target_node
     * @return Constant reference to member target_node
     */
    eProsima_user_DllExport const std::string& target_node() const;

    /*!
     * @brief This function returns a copy of the target_node member
     * @return Copy of the target_node member
     */
    eProsima_user_DllExport std::string& target_node();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function copies the value in member source_node
     * @param _source_node New value to be copied in member source_node
     */
    eProsima_user_DllExport void source_node(
            const std::string& _source_node);

    /*!
     * @brief This function moves the value in member source_node
     * @param _source_node New value to be moved in member source_node
     */
    eProsima_user_DllExport void source_node(
            std::string&& _source_node);

    /*!
     * @brief This function returns a constant reference to member source_node
     * @return Constant reference to member source_node
     */
    eProsima_user_DllExport const std::string& source_node() const;

    /*!
     * @brief This function returns a copy of the source_node member
     * @return Copy of the source_node member
     */
    eProsima_user_DllExport std::string& source_node();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    NodeControlImpl* get_impl() const;

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

protected:

    NodeControlImpl* impl_;
    friend class NodeControlImpl;

};
/*!
 * @brief This class represents the structure NodeStatus defined by the user in the IDL file.
 * @ingroup types
 */
class NodeStatus
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport NodeStatus();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~NodeStatus();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object NodeStatus that will be copied.
     */
    eProsima_user_DllExport NodeStatus(
            const NodeStatus& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object NodeStatus that will be copied.
     */
    eProsima_user_DllExport NodeStatus(
            NodeStatus&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object NodeStatus that will be copied.
     */
    eProsima_user_DllExport NodeStatus& operator =(
            const NodeStatus& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object NodeStatus that will be copied.
     */
    eProsima_user_DllExport NodeStatus& operator =(
            NodeStatus&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x NodeStatus object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const NodeStatus& x) const;

    /*!
     * @brief Comparison operator.
     * @param x NodeStatus object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const NodeStatus& x) const;

    /*!
     * @brief This function sets a value in member node_status
     * @param _node_status New value for member node_status
     */
    eProsima_user_DllExport void node_status(
            Status _node_status);

    /*!
     * @brief This function returns the value of member node_status
     * @return Value of member node_status
     */
    eProsima_user_DllExport Status node_status() const;

    /*!
     * @brief This function returns a reference to member node_status
     * @return Reference to member node_status
     */
    eProsima_user_DllExport Status& node_status();


    /*!
     * @brief This function sets a value in member task_status
     * @param _task_status New value for member task_status
     */
    eProsima_user_DllExport void task_status(
            TaskStatus _task_status);

    /*!
     * @brief This function returns the value of member task_status
     * @return Value of member task_status
     */
    eProsima_user_DllExport TaskStatus task_status() const;

    /*!
     * @brief This function returns a reference to member task_status
     * @return Reference to member task_status
     */
    eProsima_user_DllExport TaskStatus& task_status();

    /*!
     * @brief This function sets a value in member error_code
     * @param _error_code New value for member error_code
     */
    eProsima_user_DllExport void error_code(
            ErrorCode _error_code);

    /*!
     * @brief This function returns the value of member error_code
     * @return Value of member error_code
     */
    eProsima_user_DllExport ErrorCode error_code() const;

    /*!
     * @brief This function returns a reference to member error_code
     * @return Reference to member error_code
     */
    eProsima_user_DllExport ErrorCode& error_code();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();


    /*!
     * @brief This function copies the value in member error_description
     * @param _error_description New value to be copied in member error_description
     */
    eProsima_user_DllExport void error_description(
            const std::string& _error_description);

    /*!
     * @brief This function moves the value in member error_description
     * @param _error_description New value to be moved in member error_description
     */
    eProsima_user_DllExport void error_description(
            std::string&& _error_description);

    /*!
     * @brief This function returns a constant reference to member error_description
     * @return Constant reference to member error_description
     */
    eProsima_user_DllExport const std::string& error_description() const;

    /*!
     * @brief This function returns a copy of the error_description member
     * @return Copy of the error_description member
     */
    eProsima_user_DllExport std::string& error_description();


    /*!
     * @brief This function copies the value in member node_name
     * @param _node_name New value to be copied in member node_name
     */
    eProsima_user_DllExport void node_name(
            const std::string& _node_name);

    /*!
     * @brief This function moves the value in member node_name
     * @param _node_name New value to be moved in member node_name
     */
    eProsima_user_DllExport void node_name(
            std::string&& _node_name);

    /*!
     * @brief This function returns a constant reference to member node_name
     * @return Constant reference to member node_name
     */
    eProsima_user_DllExport const std::string& node_name() const;

    /*!
     * @brief This function returns a copy of the node_name member
     * @return Copy of the node_name member
     */
    eProsima_user_DllExport std::string& node_name();

    /*!
     * @brief This function updates and publish the new given status.
     * @param new_status new status to be updated and published.
     */
    eProsima_user_DllExport void update(
            uint32_t new_status);

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    NodeStatusImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

protected:

    NodeStatusImpl* impl_;
    friend class NodeStatusImpl;

};
/*!
 * @brief This class represents the structure UserInput defined by the user in the IDL file.
 * @ingroup types
 */
class UserInput
{
public:

    using impl_type = UserInputImpl;

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport UserInput();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~UserInput();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object UserInput that will be copied.
     */
    eProsima_user_DllExport UserInput(
            const UserInput& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object UserInput that will be copied.
     */
    eProsima_user_DllExport UserInput(
            UserInput&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object UserInput that will be copied.
     */
    eProsima_user_DllExport UserInput& operator =(
            const UserInput& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object UserInput that will be copied.
     */
    eProsima_user_DllExport UserInput& operator =(
            UserInput&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x UserInput object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const UserInput& x) const;

    /*!
     * @brief Comparison operator.
     * @param x UserInput object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const UserInput& x) const;

    /*!
     * @brief This function copies the value in member modality
     * @param _modality New value to be copied in member modality
     */
    eProsima_user_DllExport void modality(
            const std::string& _modality);

    /*!
     * @brief This function moves the value in member modality
     * @param _modality New value to be moved in member modality
     */
    eProsima_user_DllExport void modality(
            std::string&& _modality);

    /*!
     * @brief This function returns a constant reference to member modality
     * @return Constant reference to member modality
     */
    eProsima_user_DllExport const std::string& modality() const;

    /*!
     * @brief This function returns a reference to member modality
     * @return Reference to member modality
     */
    eProsima_user_DllExport std::string& modality();

    /*!
     * @brief This function copies the value in member problem_short_description
     * @param _problem_short_description New value to be copied in member problem_short_description
     */
    eProsima_user_DllExport void problem_short_description(
            const std::string& _problem_short_description);

    /*!
     * @brief This function moves the value in member problem_short_description
     * @param _problem_short_description New value to be moved in member problem_short_description
     */
    eProsima_user_DllExport void problem_short_description(
            std::string&& _problem_short_description);

    /*!
     * @brief This function returns a constant reference to member problem_short_description
     * @return Constant reference to member problem_short_description
     */
    eProsima_user_DllExport const std::string& problem_short_description() const;

    /*!
     * @brief This function returns a reference to member problem_short_description
     * @return Reference to member problem_short_description
     */
    eProsima_user_DllExport std::string& problem_short_description();

    /*!
     * @brief This function copies the value in member problem_definition
     * @param _problem_definition New value to be copied in member problem_definition
     */
    eProsima_user_DllExport void problem_definition(
            const std::string& _problem_definition);

    /*!
     * @brief This function moves the value in member problem_definition
     * @param _problem_definition New value to be moved in member problem_definition
     */
    eProsima_user_DllExport void problem_definition(
            std::string&& _problem_definition);

    /*!
     * @brief This function returns a constant reference to member problem_definition
     * @return Constant reference to member problem_definition
     */
    eProsima_user_DllExport const std::string& problem_definition() const;

    /*!
     * @brief This function returns a reference to member problem_definition
     * @return Reference to member problem_definition
     */
    eProsima_user_DllExport std::string& problem_definition();


    /*!
     * @brief This function copies the value in member inputs
     * @param _inputs New value to be copied in member inputs
     */
    eProsima_user_DllExport void inputs(
            const std::vector<std::string>& _inputs);

    /*!
     * @brief This function moves the value in member inputs
     * @param _inputs New value to be moved in member inputs
     */
    eProsima_user_DllExport void inputs(
            std::vector<std::string>&& _inputs);

    /*!
     * @brief This function returns a constant reference to member inputs
     * @return Constant reference to member inputs
     */
    eProsima_user_DllExport const std::vector<std::string>& inputs() const;

    /*!
     * @brief This function returns a reference to member inputs
     * @return Reference to member inputs
     */
    eProsima_user_DllExport std::vector<std::string>& inputs();


    /*!
     * @brief This function copies the value in member outputs
     * @param _outputs New value to be copied in member outputs
     */
    eProsima_user_DllExport void outputs(
            const std::vector<std::string>& _outputs);

    /*!
     * @brief This function moves the value in member outputs
     * @param _outputs New value to be moved in member outputs
     */
    eProsima_user_DllExport void outputs(
            std::vector<std::string>&& _outputs);

    /*!
     * @brief This function returns a constant reference to member outputs
     * @return Constant reference to member outputs
     */
    eProsima_user_DllExport const std::vector<std::string>& outputs() const;

    /*!
     * @brief This function returns a reference to member outputs
     * @return Reference to member outputs
     */
    eProsima_user_DllExport std::vector<std::string>& outputs();


    /*!
     * @brief This function sets a value in member minimum_samples
     * @param _minimum_samples New value for member minimum_samples
     */
    eProsima_user_DllExport void minimum_samples(
            uint32_t _minimum_samples);

    /*!
     * @brief This function returns the value of member minimum_samples
     * @return Value of member minimum_samples
     */
    eProsima_user_DllExport uint32_t minimum_samples() const;

    /*!
     * @brief This function returns a reference to member minimum_samples
     * @return Reference to member minimum_samples
     */
    eProsima_user_DllExport uint32_t& minimum_samples();


    /*!
     * @brief This function sets a value in member maximum_samples
     * @param _maximum_samples New value for member maximum_samples
     */
    eProsima_user_DllExport void maximum_samples(
            uint32_t _maximum_samples);

    /*!
     * @brief This function returns the value of member maximum_samples
     * @return Value of member maximum_samples
     */
    eProsima_user_DllExport uint32_t maximum_samples() const;

    /*!
     * @brief This function returns a reference to member maximum_samples
     * @return Reference to member maximum_samples
     */
    eProsima_user_DllExport uint32_t& maximum_samples();


    /*!
     * @brief This function sets a value in member optimize_carbon_footprint_manual
     * @param _optimize_carbon_footprint_manual New value for member optimize_carbon_footprint_manual
     */
    eProsima_user_DllExport void optimize_carbon_footprint_manual(
            bool _optimize_carbon_footprint_manual);

    /*!
     * @brief This function returns the value of member optimize_carbon_footprint_manual
     * @return Value of member optimize_carbon_footprint_manual
     */
    eProsima_user_DllExport bool optimize_carbon_footprint_manual() const;

    /*!
     * @brief This function returns a reference to member optimize_carbon_footprint_manual
     * @return Reference to member optimize_carbon_footprint_manual
     */
    eProsima_user_DllExport bool& optimize_carbon_footprint_manual();


    /*!
     * @brief This function sets a value in member previous_iteration
     * @param _previous_iteration New value for member previous_iteration
     */
    eProsima_user_DllExport void previous_iteration(
            int32_t _previous_iteration);

    /*!
     * @brief This function returns the value of member previous_iteration
     * @return Value of member previous_iteration
     */
    eProsima_user_DllExport int32_t previous_iteration() const;

    /*!
     * @brief This function returns a reference to member previous_iteration
     * @return Reference to member previous_iteration
     */
    eProsima_user_DllExport int32_t& previous_iteration();


    /*!
     * @brief This function sets a value in member optimize_carbon_footprint_auto
     * @param _optimize_carbon_footprint_auto New value for member optimize_carbon_footprint_auto
     */
    eProsima_user_DllExport void optimize_carbon_footprint_auto(
            bool _optimize_carbon_footprint_auto);

    /*!
     * @brief This function returns the value of member optimize_carbon_footprint_auto
     * @return Value of member optimize_carbon_footprint_auto
     */
    eProsima_user_DllExport bool optimize_carbon_footprint_auto() const;

    /*!
     * @brief This function returns a reference to member optimize_carbon_footprint_auto
     * @return Reference to member optimize_carbon_footprint_auto
     */
    eProsima_user_DllExport bool& optimize_carbon_footprint_auto();


    /*!
     * @brief This function sets a value in member desired_carbon_footprint
     * @param _desired_carbon_footprint New value for member desired_carbon_footprint
     */
    eProsima_user_DllExport void desired_carbon_footprint(
            double _desired_carbon_footprint);

    /*!
     * @brief This function returns the value of member desired_carbon_footprint
     * @return Value of member desired_carbon_footprint
     */
    eProsima_user_DllExport double desired_carbon_footprint() const;

    /*!
     * @brief This function returns a reference to member desired_carbon_footprint
     * @return Reference to member desired_carbon_footprint
     */
    eProsima_user_DllExport double& desired_carbon_footprint();


    /*!
     * @brief This function copies the value in member geo_location_continent
     * @param _geo_location_continent New value to be copied in member geo_location_continent
     */
    eProsima_user_DllExport void geo_location_continent(
            const std::string& _geo_location_continent);

    /*!
     * @brief This function moves the value in member geo_location_continent
     * @param _geo_location_continent New value to be moved in member geo_location_continent
     */
    eProsima_user_DllExport void geo_location_continent(
            std::string&& _geo_location_continent);

    /*!
     * @brief This function returns a constant reference to member geo_location_continent
     * @return Constant reference to member geo_location_continent
     */
    eProsima_user_DllExport const std::string& geo_location_continent() const;

    /*!
     * @brief This function returns a reference to member geo_location_continent
     * @return Reference to member geo_location_continent
     */
    eProsima_user_DllExport std::string& geo_location_continent();


    /*!
     * @brief This function copies the value in member geo_location_region
     * @param _geo_location_region New value to be copied in member geo_location_region
     */
    eProsima_user_DllExport void geo_location_region(
            const std::string& _geo_location_region);

    /*!
     * @brief This function moves the value in member geo_location_region
     * @param _geo_location_region New value to be moved in member geo_location_region
     */
    eProsima_user_DllExport void geo_location_region(
            std::string&& _geo_location_region);

    /*!
     * @brief This function returns a constant reference to member geo_location_region
     * @return Constant reference to member geo_location_region
     */
    eProsima_user_DllExport const std::string& geo_location_region() const;

    /*!
     * @brief This function returns a reference to member geo_location_region
     * @return Reference to member geo_location_region
     */
    eProsima_user_DllExport std::string& geo_location_region();


    /*!
     * @brief This function copies the value in member extra_data
     * @param _extra_data New value to be copied in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            const std::vector<uint8_t>& _extra_data);

    /*!
     * @brief This function moves the value in member extra_data
     * @param _extra_data New value to be moved in member extra_data
     */
    eProsima_user_DllExport void extra_data(
            std::vector<uint8_t>&& _extra_data);

    /*!
     * @brief This function returns a constant reference to member extra_data
     * @return Constant reference to member extra_data
     */
    eProsima_user_DllExport const std::vector<uint8_t>& extra_data() const;

    /*!
     * @brief This function returns a reference to member extra_data
     * @return Reference to member extra_data
     */
    eProsima_user_DllExport std::vector<uint8_t>& extra_data();

    /*!
     * @brief This function copies the value in member task_id
     * @param _task_id New value to be copied in member task_id
     */
    eProsima_user_DllExport void task_id(
            const TaskId& _task_id);

    /*!
     * @brief This function moves the value in member task_id
     * @param _task_id New value to be moved in member task_id
     */
    eProsima_user_DllExport void task_id(
            TaskId&& _task_id);

    /*!
     * @brief This function returns a constant reference to member task_id
     * @return Constant reference to member task_id
     */
    eProsima_user_DllExport const TaskId& task_id() const;

    /*!
     * @brief This function returns a reference to member task_id
     * @return Reference to member task_id
     */
    eProsima_user_DllExport TaskId& task_id();

    /*!
     * @brief This function returns the implementation
     * @return Pointer to implementation
     */
    UserInputImpl* get_impl();

    /*!
     * @brief Resets the structure to default values
     */
    void reset();

    /*!
     * @brief This function retrives the implementation type info
     * @return Reference to the typeid
     */
    static const std::type_info& impl_typeinfo();

protected:

    UserInputImpl* impl_;
    friend class UserInputImpl;

};

template<typename T>
struct NodeTaskOutputData
{
    types::NodeStatus node_status;
    T output_data;

    inline void reset()
    {
        node_status.reset();
        output_data.reset();
    }

};

} // namespace types

#endif // _FAST_DDS_TYPES_H_

