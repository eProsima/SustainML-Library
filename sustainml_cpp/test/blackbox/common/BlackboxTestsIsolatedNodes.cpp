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

#include "BlackboxTests.hpp"

TEST(BlackboxTestsIsolatedNodes, MLModelMetadataNode)
{
    MLModelMetadataManagedNode node;

    TaskInjector<UserInputImplPubSubType> ui_inj(common::TopicCollection::get()[common::USER_INPUT].first);

    node.start();

    ui_inj.wait_discovery(1);

    auto ui_data = default_userinput_task_generator();

    node.prepare_expected_samples(ui_data.size());

    ui_inj.inject(ui_data);

    EXPECT_TRUE(node.block_for_all(std::chrono::seconds(4)));
}

TEST(BlackboxTestsIsolatedNodes, MachineLearningModelNode)
{
    MLModelManagedNode node;

    TaskInjector<MLModelMetadataImplPubSubType> ml_model_metadata_inj(common::TopicCollection::get()[common::
                    ML_MODEL_METADATA].first);
    TaskInjector<HWConstraintsImplPubSubType> hw_cons_inj(common::TopicCollection::get()[common::HW_CONSTRAINT].first);
    TaskInjector<AppRequirementsImplPubSubType> appreq_inj(common::TopicCollection::get()[common::APP_REQUIREMENT].first);

    node.start();

    ml_model_metadata_inj.wait_discovery(1);
    hw_cons_inj.wait_discovery(1);
    appreq_inj.wait_discovery(1);

    auto ml_model_metadata_data = default_modelmetadata_task_generator();
    auto hwcons_data = default_hwconstraints_task_generator();
    auto appreq_data = default_apprequirements_data_generator();

    node.prepare_expected_samples(ml_model_metadata_data.size());

    ml_model_metadata_inj.inject(ml_model_metadata_data);
    hw_cons_inj.inject(hwcons_data);
    appreq_inj.inject(appreq_data);

    EXPECT_TRUE(node.block_for_all(std::chrono::seconds(4)));
}

TEST(BlackboxTestsIsolatedNodes, HardwareResourcesNode)
{
    HWResourcesManagedNode node;

    TaskInjector<MLModelImplPubSubType> ml_inj(common::TopicCollection::get()[common::ML_MODEL].first);
    TaskInjector<HWConstraintsImplPubSubType> hw_cons_inj(common::TopicCollection::get()[common::HW_CONSTRAINT].first);
    TaskInjector<AppRequirementsImplPubSubType> appreq_inj(common::TopicCollection::get()[common::APP_REQUIREMENT].first);

    node.start();

    ml_inj.wait_discovery(1);
    hw_cons_inj.wait_discovery(1);
    appreq_inj.wait_discovery(1);

    auto ml_data = default_mlmodel_task_generator();
    auto hwcons_data = default_hwconstraints_task_generator();
    auto appreq_data = default_apprequirements_data_generator();

    node.prepare_expected_samples(ml_data.size());

    ml_inj.inject(ml_data);
    hw_cons_inj.inject(hwcons_data);
    appreq_inj.inject(appreq_data);

    EXPECT_TRUE(node.block_for_all(std::chrono::seconds(4)));
}

TEST(BlackboxTestsIsolatedNodes, CarbonFootprintNode)
{
    CarbonFootprintManagedNode node;

    TaskInjector<UserInputImplPubSubType> ui_inj(common::TopicCollection::get()[common::USER_INPUT].first);
    TaskInjector<MLModelImplPubSubType> ml_inj(common::TopicCollection::get()[common::ML_MODEL].first);
    TaskInjector<HWResourceImplPubSubType> hw_inj(common::TopicCollection::get()[common::HW_RESOURCE].first);

    node.start();

    ui_inj.wait_discovery(1);
    ml_inj.wait_discovery(1);
    hw_inj.wait_discovery(1);

    auto ui_data = default_userinput_task_generator();
    auto ml_data = default_mlmodel_task_generator();
    auto hw_data = default_hwresource_task_generator();

    node.prepare_expected_samples(ui_data.size());

    ui_inj.inject(ui_data);
    ml_inj.inject(ml_data);
    hw_inj.inject(hw_data);

    EXPECT_TRUE(node.block_for_all(std::chrono::seconds(4)));

}
