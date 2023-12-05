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

TEST(BlackboxTestsCallbackProcessing, TasksCorrectlyFinishDespiteDifferentProcessingTimes)
{
    TaskEncoderCallbackSignature te_cb =
            [](types::UserInput& ui, types::NodeStatus&, types::EncodedTask&)
            {
                std::srand(std::time(nullptr));
                //! Depending on the task_id, simulate different processing times
                std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (ui.task_id() + 2)));
            };

    TaskEncoderManagedNode te_node(te_cb);
    MLModelManagedNode ml_node;
    HWResourcesManagedNode hw_node;

    CarbonFootprintCallbackSignature co2_cb = [](
        types::MLModel&,
        types::UserInput&,
        types::HWResource& hardware_resources,
        types::NodeStatus&,
        types::CO2Footprint& )
            {
                std::cout << "Received CO2 Callback for task_id " << hardware_resources.task_id() << std::endl;
            };

    CarbonFootprintManagedNode co2_node(co2_cb);

    TaskInjector<UserInputImplPubSubType> ui_inj(common::TopicCollection::get()[common::USER_INPUT].first);

    co2_node.start();
    hw_node.start();
    ml_node.start();
    te_node.start();

    ui_inj.wait_discovery(2);

    auto ui_data = default_userinput_task_generator(5);

    co2_node.prepare_expected_samples(ui_data.size());

    ui_inj.inject(ui_data);

    EXPECT_TRUE(co2_node.block_for_all(std::chrono::seconds(20)));
}


