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

TEST(BlackboxTestsIsolatedNodes, TaskEncoderNode)
{
    TaskEncoderManagedNode node;

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

    TaskInjector<EncodedTaskImplPubSubType> enc_task_inj(common::TopicCollection::get()[common::ENCODED_TASK].first);

    node.start();

    enc_task_inj.wait_discovery(1);

    auto enc_task_data = default_encodedtask_task_generator();

    node.prepare_expected_samples(enc_task_data.size());

    enc_task_inj.inject(enc_task_data);

    EXPECT_TRUE(node.block_for_all(std::chrono::seconds(4)));
}

TEST(BlackboxTestsIsolatedNodes, HardwareResourcesNode)
{
    HWResourcesManagedNode node;

    TaskInjector<MLModelImplPubSubType> ml_inj(common::TopicCollection::get()[common::ML_MODEL].first);

    node.start();

    ml_inj.wait_discovery(1);

    auto ml_data = default_mlmodel_task_generator();

    node.prepare_expected_samples(ml_data.size());

    ml_inj.inject(ml_data);

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
