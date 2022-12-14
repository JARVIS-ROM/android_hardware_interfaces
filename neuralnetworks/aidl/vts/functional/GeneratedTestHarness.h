/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HARDWARE_NEURALNETWORKS_AIDL_GENERATED_TEST_HARNESS_H
#define ANDROID_HARDWARE_NEURALNETWORKS_AIDL_GENERATED_TEST_HARNESS_H

#include <functional>
#include <vector>

#include <TestHarness.h>
#include "Utils.h"
#include "VtsHalNeuralnetworks.h"

namespace aidl::android::hardware::neuralnetworks::vts::functional {

using NamedModel = Named<const test_helper::TestModel*>;
using GeneratedTestParam = std::tuple<NamedDevice, NamedModel>;

class GeneratedTestBase : public testing::TestWithParam<GeneratedTestParam> {
  protected:
    void SetUp() override;
    const std::shared_ptr<IDevice> kDevice = getData(std::get<NamedDevice>(GetParam()));
    const test_helper::TestModel& kTestModel = *getData(std::get<NamedModel>(GetParam()));

  private:
    void SkipIfDriverOlderThanTestModel();
};

using FilterFn = std::function<bool(const test_helper::TestModel&)>;
std::vector<NamedModel> getNamedModels(const FilterFn& filter);

using FilterNameFn = std::function<bool(const std::string&)>;
std::vector<NamedModel> getNamedModels(const FilterNameFn& filter);

std::string printGeneratedTest(const testing::TestParamInfo<GeneratedTestParam>& info);

#define INSTANTIATE_GENERATED_TEST(TestSuite, filter)                                     \
    GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(TestSuite);                             \
    INSTANTIATE_TEST_SUITE_P(TestGenerated, TestSuite,                                    \
                             testing::Combine(testing::ValuesIn(getNamedDevices()),       \
                                              testing::ValuesIn(getNamedModels(filter))), \
                             printGeneratedTest)

// Tag for the validation tests, instantiated in VtsHalNeuralnetworks.cpp.
// TODO: Clean up the hierarchy for ValidationTest.
class ValidationTest : public GeneratedTestBase {};

Model createModel(const test_helper::TestModel& testModel);

void PrepareModel(const std::shared_ptr<IDevice>& device, const Model& model,
                  std::shared_ptr<IPreparedModel>* preparedModel);

enum class TestKind {
    // Runs a test model and compares the results to a golden data
    GENERAL,
    // Same as GENERAL but sets dimensions for the output tensors to zeros
    DYNAMIC_SHAPE,
    // Same as GENERAL but use device memories for inputs and outputs
    MEMORY_DOMAIN,
    // Same as GENERAL but use executeFenced for exeuction
    FENCED_COMPUTE,
    // Tests if quantized model with TENSOR_QUANT8_ASYMM produces the same result
    // (OK/SKIPPED/FAILED) as the model with all such tensors converted to
    // TENSOR_QUANT8_ASYMM_SIGNED.
    QUANTIZATION_COUPLING,
    // Runs a test model and verifies that MISSED_DEADLINE_* is returned.
    INTINITE_LOOP_TIMEOUT
};

void EvaluatePreparedModel(const std::shared_ptr<IDevice>& device,
                           const std::shared_ptr<IPreparedModel>& preparedModel,
                           const test_helper::TestModel& testModel, TestKind testKind);

void waitForSyncFence(int syncFd);

}  // namespace aidl::android::hardware::neuralnetworks::vts::functional

#endif  // ANDROID_HARDWARE_NEURALNETWORKS_AIDL_GENERATED_TEST_HARNESS_H
