// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FOUR_C_PASI_PARTITIONED_TWOWAYCOUP_HPP
#define FOUR_C_PASI_PARTITIONED_TWOWAYCOUP_HPP

/*---------------------------------------------------------------------------*
 | headers                                                                   |
 *---------------------------------------------------------------------------*/
#include "4C_config.hpp"

#include "4C_pasi_partitioned.hpp"

FOUR_C_NAMESPACE_OPEN

/*---------------------------------------------------------------------------*
 | class declarations                                                        |
 *---------------------------------------------------------------------------*/
namespace PaSI
{
  /*!
   * \brief two way coupled partitioned algorithm
   *
   * Two way coupled partitioned particle structure interaction algorithm following a
   * Dirichlet-Neumann coupling scheme with particle field as Dirichlet partition and Structure
   * field as Neumann partition.
   *
   * \author Sebastian Fuchs \date 02/2017
   */
  class PasiPartTwoWayCoup : public PartitionedAlgo
  {
   public:
    /*!
     * \brief constructor
     *
     * \author Sebastian Fuchs \date 02/2017
     *
     * \param[in] comm   communicator
     * \param[in] params particle structure interaction parameter list
     */
    explicit PasiPartTwoWayCoup(const Epetra_Comm& comm, const Teuchos::ParameterList& params);

    /*!
     * \brief init pasi algorithm
     *
     * \author Sebastian Fuchs \date 02/2017
     */
    void init() override;

    /*!
     * \brief setup pasi algorithm
     *
     * \author Sebastian Fuchs \date 02/2017
     */
    void setup() override;

    /*!
     * \brief read restart information for given time step
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] restartstep restart step
     */
    void read_restart(int restartstep) override;

    /*!
     * \brief partitioned two way coupled timeloop
     *
     * \author Sebastian Fuchs \date 02/2017
     */
    void timeloop() final;

   protected:
    /*!
     * \brief iteration loop between coupled fields
     *
     * \author Sebastian Fuchs \date 02/2017
     */
    virtual void outerloop();

    /*!
     * \brief output of fields
     *
     * \author Sebastian Fuchs \date 03/2017
     */
    void output() override;

    /*!
     * \brief reset increment states
     *
     * Reset the interface displacement increment and the interface force increment states to the
     * interface displacement and the interface force. The increments are build after the structure
     * and particle field are solved.
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] intfdispnp  interface displacement
     * \param[in] intfforcenp interface force
     */
    void reset_increment_states(const Core::LinAlg::Vector<double>& intfdispnp,
        const Core::LinAlg::Vector<double>& intfforcenp);

    /*!
     * \brief build increment states
     *
     * Finalize the interface displacement increment and the interface force increment states.
     *
     * \author Sebastian Fuchs \date 03/2017
     */
    void build_increment_states();

    /*!
     * \brief set interface forces
     *
     * Apply the interface forces as handed in to the structural field.
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] intfforcenp interface force
     */
    void set_interface_forces(Teuchos::RCP<const Core::LinAlg::Vector<double>> intfforcenp);

    /*!
     * \brief reset particle states
     *
     * Reset the particle states to the converged states of the last time step.
     *
     * \author Sebastian Fuchs \date 03/2017
     */
    void reset_particle_states();

    /*!
     * \brief clear interface forces
     *
     * Clear the interface forces in the particle wall handler.
     *
     * \author Sebastian Fuchs \date 05/2019
     */
    void clear_interface_forces();

    /*!
     * \brief get interface forces
     *
     * Get the interface forces via assemblation of the forces from the particle wall handler. This
     * includes communication, since the structural discretization and the particle wall
     * discretization are in general distributed independently of each other to all processors.
     *
     * \author Sebastian Fuchs \date 05/2019
     */
    void get_interface_forces();

    /*!
     * \brief convergence check of the outer loop
     *
     * Convergence check of the partitioned coupling outer loop based on relative and scaled
     * interface displacement and force increment norms.
     *
     * \author Sebastian Fuchs \date 02/2017
     *
     * \param[in] itnum iteration counter
     *
     * \return flag indicating status of convergence check
     */
    bool convergence_check(int itnum);

    /*!
     * \brief save particle states
     *
     * Save the converged particle states of the last time step.
     *
     * \author Sebastian Fuchs \date 05/2019
     */
    void save_particle_states();

    //! interface force acting
    Teuchos::RCP<Core::LinAlg::Vector<double>> intfforcenp_;

    //! interface displacement increment of the outer loop
    Teuchos::RCP<Core::LinAlg::Vector<double>> intfdispincnp_;

    //! interface force increment of the outer loop
    Teuchos::RCP<Core::LinAlg::Vector<double>> intfforceincnp_;

    //! maximum iteration steps
    const int itmax_;

    //! tolerance of relative interface displacement increments in partitioned iterations
    const double convtolrelativedisp_;

    //! tolerance of dof and dt scaled interface displacement increments in partitioned iterations
    const double convtolscaleddisp_;

    //! tolerance of relative interface force increments in partitioned iterations
    const double convtolrelativeforce_;

    //! tolerance of dof and dt scaled interface force increments in partitioned iterations
    const double convtolscaledforce_;

    //! ignore convergence check and proceed simulation
    const bool ignoreconvcheck_;

    //! write restart every n steps
    const int writerestartevery_;
  };

  /*!
   * \brief two way coupled partitioned algorithm with constant interface displacement relaxation
   *
   * Two way coupled partitioned particle structure interaction algorithm following a
   * Dirichlet-Neumann coupling scheme with particle field as Dirichlet partition and Structure
   * field as Neumann partition and constant interface displacement relaxation.
   *
   * \author Sebastian Fuchs \date 03/2017
   */
  class PasiPartTwoWayCoupDispRelax : public PasiPartTwoWayCoup
  {
   public:
    /*!
     * \brief constructor
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] comm   communicator
     * \param[in] params particle structure interaction parameter list
     */
    explicit PasiPartTwoWayCoupDispRelax(
        const Epetra_Comm& comm, const Teuchos::ParameterList& params);

    /*!
     * \brief init pasi algorithm
     *
     * \author Sebastian Fuchs \date 07/2020
     */
    void init() override;

   protected:
    /*!
     * \brief iteration loop between coupled fields with relaxed displacements
     *
     * \author Sebastian Fuchs \date 02/2017
     */
    void outerloop() override;

    /*!
     * \brief calculate relaxation parameter
     *
     * No computation of the relaxation parameter necessary in the constant case.
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] omega relaxation parameter
     * \param[in] itnum iteration counter
     */
    virtual void calc_omega(double& omega, const int itnum);

    //! relaxed interface displacement
    Teuchos::RCP<Core::LinAlg::Vector<double>> relaxintfdispnp_;

    //! relaxed interface velocity
    Teuchos::RCP<Core::LinAlg::Vector<double>> relaxintfvelnp_;

    //! relaxed interface acceleration
    Teuchos::RCP<Core::LinAlg::Vector<double>> relaxintfaccnp_;

    //! relaxation parameter
    double omega_;

   private:
    /*!
     * \brief init relaxation of interface states
     *
     * \author Sebastian Fuchs \date 07/2020
     */
    void init_relaxation_interface_states();

    /*!
     * \brief perform relaxation of interface states
     *
     * \author Sebastian Fuchs \date 11/2019
     */
    void perform_relaxation_interface_states();
  };

  /*!
   * \brief two way coupled partitioned algorithm with dynamic interface displacement relaxation
   *
   * Two way coupled partitioned particle structure interaction algorithm following a
   * Dirichlet-Neumann coupling scheme with particle field as Dirichlet partition and Structure
   * field as Neumann partition and dynamic interface displacement relaxation following Aitken's
   * delta^2 method.
   *
   * \author Sebastian Fuchs \date 03/2017
   */
  class PasiPartTwoWayCoupDispRelaxAitken : public PasiPartTwoWayCoupDispRelax
  {
   public:
    /*!
     * \brief constructor
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] comm   communicator
     * \param[in] params particle structure interaction parameter list
     */
    PasiPartTwoWayCoupDispRelaxAitken(
        const Epetra_Comm& comm, const Teuchos::ParameterList& params);

    /*!
     * \brief init pasi algorithm
     *
     * \author Sebastian Fuchs \date 03/2017
     */
    void init() override;

    /*!
     * \brief read restart information for given time step
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] restartstep restart step
     */
    void read_restart(int restartstep) override;

   protected:
    /*!
     * \brief output of fields
     *
     * \author Sebastian Fuchs \date 03/2017
     */
    void output() override;

    /*!
     * \brief calculate relaxation parameter
     *
     * Computation of the relaxation parameter following Aitken's delta^2 method.
     *
     * Refer to PhD thesis U. Kuettler, equation (3.5.29).
     *
     * \author Sebastian Fuchs \date 03/2017
     *
     * \param[in] omega relaxation parameter
     * \param[in] itnum iteration counter
     */
    void calc_omega(double& omega, const int itnum) override;

    //! old interface displacement increment of the outer loop
    Teuchos::RCP<Core::LinAlg::Vector<double>> intfdispincnpold_;

    //! maximal relaxation parameter
    double maxomega_;

    //! minimal relaxation parameter
    double minomega_;
  };

}  // namespace PaSI

/*---------------------------------------------------------------------------*/
FOUR_C_NAMESPACE_CLOSE

#endif
