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
 * @file typesImplTypeObject.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifndef _FAST_DDS_GENERATED_TYPESIMPL_TYPE_OBJECT_H_
#define _FAST_DDS_GENERATED_TYPESIMPL_TYPE_OBJECT_H_


#include <fastrtps/types/TypeObject.h>
#include <map>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif
#else
#define eProsima_user_DllExport
#endif

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(typesImpl_SOURCE)
#define typesImpl_DllAPI __declspec( dllexport )
#else
#define typesImpl_DllAPI __declspec( dllimport )
#endif // typesImpl_SOURCE
#else
#define typesImpl_DllAPI
#endif
#else
#define typesImpl_DllAPI
#endif // _WIN32

using namespace eprosima::fastrtps::types;

eProsima_user_DllExport void registertypesImplTypes();

eProsima_user_DllExport const TypeIdentifier* GetStatusIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetStatusObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalStatusObject();
eProsima_user_DllExport const TypeObject* GetCompleteStatusObject();

eProsima_user_DllExport const TypeIdentifier* GetTaskStatusIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetTaskStatusObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalTaskStatusObject();
eProsima_user_DllExport const TypeObject* GetCompleteTaskStatusObject();

eProsima_user_DllExport const TypeIdentifier* GetErrorCodeIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetErrorCodeObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalErrorCodeObject();
eProsima_user_DllExport const TypeObject* GetCompleteErrorCodeObject();



eProsima_user_DllExport const TypeIdentifier* GetNodeStatusImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetNodeStatusImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalNodeStatusImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteNodeStatusImplObject();

eProsima_user_DllExport const TypeIdentifier* GetCmdNodeIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetCmdNodeObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalCmdNodeObject();
eProsima_user_DllExport const TypeObject* GetCompleteCmdNodeObject();

eProsima_user_DllExport const TypeIdentifier* GetCmdTaskIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetCmdTaskObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalCmdTaskObject();
eProsima_user_DllExport const TypeObject* GetCompleteCmdTaskObject();



eProsima_user_DllExport const TypeIdentifier* GetNodeControlImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetNodeControlImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalNodeControlImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteNodeControlImplObject();



eProsima_user_DllExport const TypeIdentifier* GetGeoLocationImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetGeoLocationImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalGeoLocationImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteGeoLocationImplObject();



eProsima_user_DllExport const TypeIdentifier* GetUserInputImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetUserInputImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalUserInputImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteUserInputImplObject();



eProsima_user_DllExport const TypeIdentifier* GetMLModelMetadataImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMLModelMetadataImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalMLModelMetadataImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteMLModelMetadataImplObject();



eProsima_user_DllExport const TypeIdentifier* GetAppRequirementsImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetAppRequirementsImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalAppRequirementsImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteAppRequirementsImplObject();



eProsima_user_DllExport const TypeIdentifier* GetHWConstraintsImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetHWConstraintsImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalHWConstraintsImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteHWConstraintsImplObject();



eProsima_user_DllExport const TypeIdentifier* GetMLModelImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMLModelImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalMLModelImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteMLModelImplObject();



eProsima_user_DllExport const TypeIdentifier* GetHWResourceImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetHWResourceImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalHWResourceImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteHWResourceImplObject();



eProsima_user_DllExport const TypeIdentifier* GetCO2FootprintImplIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* GetCO2FootprintImplObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalCO2FootprintImplObject();
eProsima_user_DllExport const TypeObject* GetCompleteCO2FootprintImplObject();


#endif // _FAST_DDS_GENERATED_TYPESIMPL_TYPE_OBJECT_H_