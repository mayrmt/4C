// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FOUR_C_MAT_CANN_SURROGATE_HPP
#define FOUR_C_MAT_CANN_SURROGATE_HPP


#include "4C_config.hpp"

#include "4C_comm_parobjectfactory.hpp"
#include "4C_linalg_fixedsizematrix.hpp"
#include "4C_linalg_symmetric_tensor.hpp"
#include "4C_linalg_tensor_generators.hpp"
#include "4C_linalg_tensor_matrix_conversion.hpp"
#include "4C_mat_so3_material.hpp"
#include "4C_material_parameter_base.hpp"

#include <filesystem>
#include <memory>

#ifdef FOUR_C_WITH_PYBIND11

#include <pybind11/embed.h>
#include <pybind11/numpy.h>

FOUR_C_NAMESPACE_OPEN

namespace Mat
{
  namespace PAR
  {
    /*----------------------------------------------------------------------*/
    /// material parameters for CAN Surrogate Model
    class CANNSurrogate : public Core::Mat::PAR::Parameter
    {
     public:
      /// standard constructor
      CANNSurrogate(const Core::Mat::PAR::Parameter::Data& matdata);

      /// @name material parameters
      //@{

      /// Path to Python file with CAN surrogate model
      const std::filesystem::path python_filename_;

      //@}

      std::shared_ptr<Core::Mat::Material> create_material() override;

    };  // class CANNSurrogate
  }  // namespace PAR

  class CANNSurrogateType : public Core::Communication::ParObjectType
  {
   public:
    [[nodiscard]] std::string name() const override { return "CANNSurrogateType"; }

    static CANNSurrogateType& instance() { return instance_; };

    Core::Communication::ParObject* create(Core::Communication::UnpackBuffer& buffer) override;

   private:
    static CANNSurrogateType instance_;
  };

  /*----------------------------------------------------------------------*/
  /// Wrapper for CAN Surrogate material
  class CANNSurrogate : public So3Material
  {
   public:
    /// construct empty material object
    CANNSurrogate();

    /// construct the material object given material parameters
    explicit CANNSurrogate(Mat::PAR::CANNSurrogate* params);

    [[nodiscard]] int unique_par_object_id() const override
    {
      return CANNSurrogateType::instance().unique_par_object_id();
    }

    void pack(Core::Communication::PackBuffer& data) const override;

    void unpack(Core::Communication::UnpackBuffer& buffer) override;

    //@}

    //! @name Access methods

    [[nodiscard]] Core::Materials::MaterialType material_type() const override
    {
      return Core::Materials::m_cann_surrogate;
    }

    void valid_kinematics(Inpar::Solid::KinemType kinem) override
    {
      if (kinem != Inpar::Solid::KinemType::linear &&
          kinem != Inpar::Solid::KinemType::nonlinearTotLag)
        FOUR_C_THROW("element and material kinematics are not compatible");
    }

    [[nodiscard]] std::shared_ptr<Core::Mat::Material> clone() const override
    {
      return std::make_shared<CANNSurrogate>(*this);
    }

    // /// Path to Python file with CAN surrogate model
    // [[nodiscard]] std::filesystem::path python_filename() const
    // {
    //   return params_->python_filename_;
    // }

    [[nodiscard]] Core::Mat::PAR::Parameter* parameter() const override { return params_; }

    //@}

    //! @name Evaluation methods

    //! Calculate stresses using one of the methods to evaluate the elasticity tensor
    void evaluate(const Core::LinAlg::Tensor<double, 3, 3>* defgrad,
        const Core::LinAlg::SymmetricTensor<double, 3, 3>& glstrain,
        const Teuchos::ParameterList& params, Core::LinAlg::SymmetricTensor<double, 3, 3>& stress,
        Core::LinAlg::SymmetricTensor<double, 3, 3, 3, 3>& cmat, int gp, int eleGID) override;

    //! Calculate strain energy
    [[nodiscard]] double strain_energy(const Core::LinAlg::SymmetricTensor<double, 3, 3>& glstrain,
        int gp, int eleGID) const override;
    //@}

    static constexpr Core::LinAlg::SymmetricTensor<double, 3, 3> evaluate_stress(
        const Core::LinAlg::SymmetricTensor<double, 3, 3>& glstrain)
    {
      return Core::LinAlg::TensorGenerators::identity<double, 3, 3>;
    }

    static Core::LinAlg::SymmetricTensor<double, 3, 3, 3, 3> evaluate_stress_linearization()
    {
      Core::LinAlg::SymmetricTensor<double, 3, 3, 3, 3> cmat{};
      Core::LinAlg::Matrix<6, 6> cmat_view = Core::LinAlg::make_stress_like_voigt_view(cmat);

      cmat_view(0, 0) = 1.0;
      cmat_view(1, 1) = 1.0;
      cmat_view(2, 2) = 1.0;
      // ~~~
      cmat_view(3, 3) = 1.0;
      cmat_view(4, 4) = 1.0;
      cmat_view(5, 5) = 1.0;

      return cmat;
    }


   private:
    /// my material parameters
    Mat::PAR::CANNSurrogate* params_;

    // std::unique_ptr<pybind11::scoped_interpreter> guard_ = nullptr;

    // pybind11::object evaluate_stress_;
  };
}  // namespace Mat

FOUR_C_NAMESPACE_CLOSE

#endif
#endif
