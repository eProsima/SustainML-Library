// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file typesImplTypeObjectSupport.hpp
 * Header file containing the API required to register the TypeObject representation of the described types in the IDL file
 *
 * This file was generated by the tool fastddsgen.
 */

#ifndef FAST_DDS_GENERATED__TYPESIMPL_TYPE_OBJECT_SUPPORT_HPP
#define FAST_DDS_GENERATED__TYPESIMPL_TYPE_OBJECT_SUPPORT_HPP

#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>


#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#ifndef DOXYGEN_SHOULD_SKIP_THIS_PUBLIC

/**
 * @brief Register Status related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_Status_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register TaskStatus related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_TaskStatus_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register ErrorCode related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_ErrorCode_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register TaskIdImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_TaskIdImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register NodeStatusImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_NodeStatusImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register CmdNode related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_CmdNode_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register CmdTask related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_CmdTask_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register NodeControlImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_NodeControlImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register UserInputImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_UserInputImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register MLModelMetadataImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_MLModelMetadataImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register AppRequirementsImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_AppRequirementsImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register HWConstraintsImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_HWConstraintsImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register MLModelImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_MLModelImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register HWResourceImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_HWResourceImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register CO2FootprintImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_CO2FootprintImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register RequestTypeImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_RequestTypeImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);
/**
 * @brief Register ResponseTypeImpl related TypeIdentifier.
 *        Fully-descriptive TypeIdentifiers are directly registered.
 *        Hash TypeIdentifiers require to fill the TypeObject information and hash it, consequently, the TypeObject is
 *        indirectly registered as well.
 *
 * @param[out] TypeIdentifier of the registered type.
 *             The returned TypeIdentifier corresponds to the complete TypeIdentifier in case of hashed TypeIdentifiers.
 *             Invalid TypeIdentifier is returned in case of error.
 */
eProsima_user_DllExport void register_ResponseTypeImpl_type_identifier(
        eprosima::fastdds::dds::xtypes::TypeIdentifierPair& type_ids);


#endif // DOXYGEN_SHOULD_SKIP_THIS_PUBLIC

#endif // FAST_DDS_GENERATED__TYPESIMPL_TYPE_OBJECT_SUPPORT_HPP
