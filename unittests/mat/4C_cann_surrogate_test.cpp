// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include "4C_comm_pack_buffer.hpp"
#include "4C_global_data.hpp"
#include "4C_linalg_tensor_generators.hpp"
#include "4C_linalg_tensor_matrix_conversion.hpp"
#include "4C_mat_cann_surrogate.hpp"
#include "4C_mat_material_factory.hpp"
#include "4C_mat_par_bundle.hpp"
#include "4C_mat_service.hpp"
#include "4C_material_base.hpp"
#include "4C_material_parameter_base.hpp"
#include "4C_unittest_utils_assertions_test.hpp"
#include "4C_utils_singleton_owner.hpp"

namespace
{
  using namespace FourC;

  class CANNSurrogateTest : public ::testing::Test
  {
   protected:
    void SetUp() override
    {
      Core::IO::InputParameterContainer container;
      container.add("PYTHON_FILENAME",
          std::filesystem::path("/home/a11bmama/codes/mayrmt_baci/src-baci/unittests/mat/"
                                "4C_can_surrogate.py"));

      param_ = std::shared_ptr(
          Mat::make_parameter(1, Core::Materials::MaterialType::m_cann_surrogate, container));

      Global::Problem& problem = (*Global::Problem::instance());
      problem.materials()->set_read_from_problem(0);
      problem.materials()->insert(1, param_);
      material_ = std::make_shared<Mat::PlasticDruckerPrager>(
          dynamic_cast<Mat::PAR::PlasticDruckerPrager*>(param_.get()));
    }

    std::shared_ptr<Core::Mat::PAR::Parameter> param_;
    Core::Communication::PackBuffer data;
    std::shared_ptr<Mat::CANNSurrogate> material_;

    Core::Utils::SingletonOwnerRegistry::ScopeGuard guard;
  };

  //! test member function Evaluate
  TEST_F(CANNSurrogateTest, TestEvaluate)
  {
    Core::IO::InputParameterContainer container;
    material_->setup(1, container);
    Core::LinAlg::SymmetricTensor<double, 3, 3> input_strain =
        Core::LinAlg::TensorGenerators::full<3, 3>(0.05) +
        0.05 * Core::LinAlg::TensorGenerators::identity<double, 3, 3>;
    Teuchos::ParameterList paras;
    Core::LinAlg::SymmetricTensor<double, 3, 3> ref_stress{};

    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        if (i == j)
        {
          ref_stress(i, j) = 0.01;
        }
        else
        {
          ref_stress(i, j) = 0.05;
        }
      }
    }
    Core::LinAlg::SymmetricTensor<double, 3, 3, 3, 3> result_cmat{};
    Core::LinAlg::SymmetricTensor<double, 3, 3> result_stress{};
    material_->evaluate(nullptr, input_strain, paras, result_stress, result_cmat, 0, 0);
    FOUR_C_EXPECT_NEAR(result_stress, ref_stress, 1.0e-12);
  };
}  // namespace
