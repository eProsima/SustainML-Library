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

TEST(BlackboxTestsPairedNodes, MLModelMetadataWithMLNode)
{
    MLModelMetadataManagedNode ml_md_node;
    MLModelManagedNode ml_node;

    TaskInjector<UserInputImplPubSubType> ui_inj(common::TopicCollection::get()[common::USER_INPUT].first);
    TaskInjector<HWConstraintsImplPubSubType> hw_cons_inj(common::TopicCollection::get()[common::HW_CONSTRAINT].first);
    TaskInjector<AppRequirementsImplPubSubType> appreq_inj(common::TopicCollection::get()[common::APP_REQUIREMENT].first);

    ml_md_node.start();
    ml_node.start();

    ui_inj.wait_discovery(1);
    hw_cons_inj.wait_discovery(1);
    appreq_inj.wait_discovery(1);

    auto ui_data = default_userinput_task_generator();
    auto hwcons_data = default_hwconstraints_task_generator();
    auto appreq_data = default_apprequirements_data_generator();

    ml_node.prepare_expected_samples(ui_data.size());

    ui_inj.inject(ui_data);
    hw_cons_inj.inject(hwcons_data);
    appreq_inj.inject(appreq_data);

    EXPECT_TRUE(ml_node.block_for_all(std::chrono::seconds(5)));
}

TEST(BlackboxTestsPairedNodes, MachineLearningWithHardwareResourcesNode)
{
    MLModelManagedNode ml_node;
    HWResourcesManagedNode hw_node;
    HWConstraintsManagedNode hwc_node;
    AppRequirementsManagedNode app_req_node;

    TaskInjector<MLModelMetadataImplPubSubType> enc_task_inj(common::TopicCollection::get()[common::ML_MODEL_METADATA].
                    first);
    TaskInjector<UserInputImplPubSubType> user_input_inj(common::TopicCollection::get()[common::USER_INPUT].
                    first);

    hw_node.start();
    ml_node.start();
    app_req_node.start();
    hwc_node.start();

    enc_task_inj.wait_discovery(1);
    user_input_inj.wait_discovery(2);

    auto enc_task_data = default_modelmetadata_task_generator();
    auto user_input_data = default_userinput_task_generator();

    hw_node.prepare_expected_samples(enc_task_data.size());

    enc_task_inj.inject(enc_task_data);
    user_input_inj.inject(user_input_data);

    EXPECT_TRUE(hw_node.block_for_all(std::chrono::seconds(5)));
}

TEST(BlackboxTestsPairedNodes, HardwareResourcesWithCarbonFootprintNode)
{
    HWResourcesManagedNode hw_node;
    CarbonFootprintManagedNode co2_node;

    TaskInjector<MLModelImplPubSubType> ml_inj(common::TopicCollection::get()[common::ML_MODEL].first);
    TaskInjector<UserInputImplPubSubType> ui_inj(common::TopicCollection::get()[common::USER_INPUT].first);
    TaskInjector<HWConstraintsImplPubSubType> hw_cons_inj(common::TopicCollection::get()[common::HW_CONSTRAINT].first);
    TaskInjector<AppRequirementsImplPubSubType> appreq_inj(common::TopicCollection::get()[common::APP_REQUIREMENT].first);

    co2_node.start();
    hw_node.start();

    ml_inj.wait_discovery(2);
    ui_inj.wait_discovery(1);
    hw_cons_inj.wait_discovery(1);
    appreq_inj.wait_discovery(1);

    auto ml_data = default_mlmodel_task_generator();
    auto ui_data = default_userinput_task_generator();
    auto hwcons_data = default_hwconstraints_task_generator();
    auto appreq_data = default_apprequirements_data_generator();

    co2_node.prepare_expected_samples(ml_data.size());

    ml_inj.inject(ml_data);
    ui_inj.inject(ui_data);
    hw_cons_inj.inject(hwcons_data);
    appreq_inj.inject(appreq_data);

    EXPECT_TRUE(co2_node.block_for_all(std::chrono::seconds(5)));
}

TEST(BlackboxTestsPairedNodes, CompleteChain)
{
    AppRequirementsManagedNode app_req_node;
    CarbonFootprintManagedNode co2_node;
    HWConstraintsManagedNode hw_cons_node;
    HWResourcesManagedNode hw_res_node;
    MLModelMetadataManagedNode ml_md_node;
    MLModelManagedNode ml_node;

    TaskInjector<UserInputImplPubSubType> ui_inj(common::TopicCollection::get()[common::USER_INPUT].first);

    co2_node.start();
    hw_cons_node.start();
    hw_res_node.start();
    ml_md_node.start();
    ml_node.start();
    app_req_node.start();

    ui_inj.wait_discovery(4);

    auto ui_data = default_userinput_task_generator();

    co2_node.prepare_expected_samples(ui_data.size());

    ui_inj.inject(ui_data);

    EXPECT_TRUE(co2_node.block_for_all(std::chrono::seconds(5)));
}

