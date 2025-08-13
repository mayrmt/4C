// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "4C_mat_cann_surrogate.hpp"

#include "4C_comm_pack_helpers.hpp"
#include "4C_global_data.hpp"
#include "4C_linalg_symmetric_tensor.hpp"
#include "4C_linalg_tensor_generators.hpp"
#include "4C_mat_par_bundle.hpp"
#include "4C_utils_enum.hpp"

#ifdef FOUR_C_WITH_PYBIND11

FOUR_C_NAMESPACE_OPEN

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
Mat::PAR::CANNSurrogate::CANNSurrogate(const Core::Mat::PAR::Parameter::Data& matdata)
    : Parameter(matdata),
      python_filename_(matdata.parameters.get<std::filesystem::path>("PYTHON_FILENAME"))
{
  if (!std::filesystem::exists(python_filename_))
    FOUR_C_THROW("File {} does not exist.", python_filename_.string());
}

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
std::shared_ptr<Core::Mat::Material> Mat::PAR::CANNSurrogate::create_material()
{
  return std::make_shared<Mat::CANNSurrogate>(this);
}

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
Mat::CANNSurrogateType Mat::CANNSurrogateType::instance_;

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
Core::Communication::ParObject* Mat::CANNSurrogateType::create(
    Core::Communication::UnpackBuffer& buffer)
{
  auto* material = new Mat::CANNSurrogate();
  material->unpack(buffer);
  return material;
}

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
Mat::CANNSurrogate::CANNSurrogate() : params_(nullptr)
{
  // guard_ = std::make_unique<pybind11::scoped_interpreter>();
  //
  // pybind11::module sys = pybind11::module::import("sys");
  // sys.attr("path").attr("insert")(0, params_->python_filename_.parent_path().string());
  // pybind11::module model = pybind11::module::import(params_->python_filename_.stem().c_str());
  // evaluate_stress_ = model.attr("evaluate_stress");
}


/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
Mat::CANNSurrogate::CANNSurrogate(Mat::PAR::CANNSurrogate* params) : params_(params) {}


/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
void Mat::CANNSurrogate::pack(Core::Communication::PackBuffer& data) const
{
  // pack type of this instance of ParObject
  int type = unique_par_object_id();
  add_to_pack(data, type);

  // matid
  int matid = -1;
  if (params_ != nullptr) matid = params_->id();  // in case we are in post-process mode
  add_to_pack(data, matid);
}


/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
void Mat::CANNSurrogate::unpack(Core::Communication::UnpackBuffer& buffer)
{
  Core::Communication::extract_and_assert_id(buffer, unique_par_object_id());

  // matid and recover params_
  int matid;
  extract_from_pack(buffer, matid);
  params_ = nullptr;
  if (Global::Problem::instance()->materials() != nullptr)
  {
    if (Global::Problem::instance()->materials()->num() != 0)
    {
      const int probinst = Global::Problem::instance()->materials()->get_read_from_problem();
      Core::Mat::PAR::Parameter* mat =
          Global::Problem::instance(probinst)->materials()->parameter_by_id(matid);
      if (mat->type() == material_type())
        params_ = static_cast<Mat::PAR::CANNSurrogate*>(mat);
      else
        FOUR_C_THROW("Type of parameter material {} does not fit to calling type {}", mat->type(),
            material_type());
    }
  }
}

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
void Mat::CANNSurrogate::evaluate(const Core::LinAlg::Tensor<double, 3, 3>* defgrad,
    const Core::LinAlg::SymmetricTensor<double, 3, 3>& glstrain,
    const Teuchos::ParameterList& params, Core::LinAlg::SymmetricTensor<double, 3, 3>& stress,
    Core::LinAlg::SymmetricTensor<double, 3, 3, 3, 3>& cmat, int gp, int eleGID)
{
  // cmat = CANNSurrogate::evaluate_stress_linearization();

  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
      stress(i, j) = 1.0;  // evaluate_stress_(glstrain(i,j)).cast<double>();;
  }
}


/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/
double Mat::CANNSurrogate::strain_energy(
    const Core::LinAlg::SymmetricTensor<double, 3, 3>& glstrain, const int gp,
    const int eleGID) const
{
  auto stress = CANNSurrogate::evaluate_stress(glstrain);

  return 0.5 * Core::LinAlg::ddot(stress, glstrain);
}

FOUR_C_NAMESPACE_CLOSE

#endif
