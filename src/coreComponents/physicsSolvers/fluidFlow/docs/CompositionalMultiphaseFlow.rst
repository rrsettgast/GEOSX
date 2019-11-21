.. _CompositionalMultiphaseFlow:

#######################################
Compositional Multiphase Flow Solver
#######################################

Here, we review the compositional solver in three steps:

1. :ref:`theory`

    a. :ref:`equations`

    b. :ref:`primary_variables`

    c. :ref:`discretization`

    d. :ref:`solution_strategy`

2. :ref:`usage`

3. :ref:`input_example`

.. _theory:

Theory
=========================

In this section, we review the global variable formulation implemented in GEOSX.
We review the set of :ref:`equations` first, followed by a discussion of the
choice of :ref:`primary_variables`. Then we give an overview of the :ref:`discretization`
and we conclude with a brief description of the nonlinear :ref:`solution_strategy`.

.. _equations:

Governing equations
-------------------

Mass conservation equations
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Mass conservation for component :math:`c` is expressed as:

.. math::
   \phi \frac{ \partial  }{\partial t} \bigg( \sum_\ell \rho_{\ell} \, y_{c \ell} \, S_{\ell} \bigg)
   + \nabla \cdot \bigg( \sum_\ell \rho_{\ell} \, y_{c \ell} \, \boldsymbol{u}_{\ell} \bigg)
   - \sum_\ell \rho_{\ell} \, y_{c \ell} \, q_{\ell} = 0,


where :math:`\phi` is the porosity of the medium,
:math:`S_{\ell}` is the saturation of phase :math:`\ell`, :math:`y_{c \ell}`
is the mass fraction of component :math:`c` in phase :math:`\ell`,
:math:`\rho_{\ell}` is the phase density, and :math:`t` is time. We note that the
formulation currently implemented in GEOSX is isothermal.

Darcy's law
~~~~~~~~~~~

Using the multiphase extension of Darcy's law, the phase velocity :math:`\boldsymbol{u}_{\ell}`
is written as a function of the phase potential gradient :math:`\nabla \Phi_{\ell}`:

.. math::
  \boldsymbol{u}_{\ell} := -\boldsymbol{k} \lambda_{\ell} \nabla \Phi_{\ell}
  = - \boldsymbol{k} \lambda_{\ell} \big( \nabla (p - P_{c,\ell}) - \rho_{\ell} g \nabla z \big).

In this equation, :math:`\boldsymbol{k}` is the rock permeability,
:math:`\lambda_{\ell} = k_{r \ell} / \mu_{\ell}` is the phase mobility,
defined as the phase relative permeability divided by the phase viscosity,
:math:`p` is the reference pressure, :math:`P_{c,\ell}` is the the capillary
pressure,  :math:`g` is the gravitational acceleration, and :math:`z` is depth.
The evaluation of the relative permeabilities, capillary pressures, and
viscosities is reviewed in the section about :doc:`/coreComponents/constitutive/docs/Constitutive`.

Combining the mass conservation equations with Darcy's law yields a set of :math:`n_c`
equations written as:

.. math::
   \phi \frac{ \partial  }{\partial t} \bigg( \sum_\ell \rho_{\ell} \, y_{c \ell} \, S_{\ell} \bigg)
   - \nabla \cdot \boldsymbol{k} \bigg( \sum_\ell \rho_{\ell} \, y_{c \ell} \, \lambda_{\ell} \nabla \Phi_{\ell}   \bigg)
   - \sum_\ell \rho_{\ell} \, y_{c \ell} \, q_{\ell} = 0.

Constraints and thermodynamic equilibrium
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The volume constraint equation states that the pore space is always completely filled by
the phases. The constraint can be expressed as:

.. math::
  \sum_{\ell} S_{\ell} = 1.

The system is closed by the following thermodynamic equilibrium constraints:

.. math::
  f_{c \ell} - f_{c m} = 0.

where :math:`f_{c \ell}` is the fugacity of component :math:`c` in phase :math:`\ell`.
The flash calculations performed to enforce the thermodynamical equilibrium are reviewed
in the section about :doc:`/coreComponents/constitutive/docs/Constitutive`.

To summarize, the compositional multiphase flow solver assembles a set of :math:`n_c+1`
equations, i.e., :math:`n_c` mass conservation equations and one volume constraint equation.
A separate module discussed in the :doc:`/coreComponents/constitutive/docs/Constitutive`
is responsible for the enforcement of the thermodynamic equilibrium at each nonlinear iteration.

==================== ===========================
Number of equations  Equation type
==================== ===========================
:math:`n_c`          Mass conservation equations
1                    Volume constraint
==================== ===========================

.. _primary_variables:

Primary variables
------------------

The variable formulation implemented in GEOSX is a global variable formulation based on
:math:`n_c+1` primary variables, namely, one pressure, :math:`p`, and
:math:`n_c` (by default, molar) component densities, :math:`\rho_c`.
By default, we use molar component densities. A flag discussed in the section
:ref:`usage` can be used to use mass component densities instead of molar component densities.

=========================== ===========================
Number of primary variables Variable type
=========================== ===========================
1                           Pressure
:math:`n_c`                 Component densities
=========================== ===========================

Assembling the residual equations and calling the
:doc:`/coreComponents/constitutive/docs/Constitutive` requires computing the molar component
fractions and saturations. This is done with the relationship:

.. math::
  z_c := \frac{\rho_c}{\rho_T},

where

.. math::
  \rho_T := \sum_c \rho_c.

These secondary variables are used as input to the flash calculations.
After the flash calculations, the saturations are computed as:

.. math::
  S_{\ell} := \nu_{\ell} \frac{ \rho_T }{ \rho_{\ell}},

where :math:`\nu_{\ell}` is the global mole fraction of phase :math:`\ell`
and :math:`\rho_{\ell}` is the molar density of phase :math:`\ell`.
These steps also involve computing the derivatives of the component
fractions and saturations with respect to the pressure and component densities.

.. _discretization:

Discretization
--------------

Spatial discretization
~~~~~~~~~~~~~~~~~~~~~~

The governing equations are discretized using standard cell-centered finite-volume
discretization.

In the approximation of the flux term at the interface between two control volumes,
the calculation of the pressure stencil is general and will ultimately support a
Multi-Point Flux Approximation (MPFA) approach. The current implementation of the
transmissibility calculation is reviewed in the section about
:doc:`/coreComponents/finiteVolume/docs/FiniteVolume`.

The approximation of the dynamic transport coefficients multiplying the discrete
potential difference (e.g., the phase mobilities) is performed with a first-order
phase-per-phase single-point upwinding based on the sign of the phase potential difference
at the interface.

Temporal discretization
~~~~~~~~~~~~~~~~~~~~~~~

The compositional multiphase solver uses a fully implicit (backward Euler) temporal discretization.

.. _solution_strategy:

Solution strategy
-----------------

The nonlinear solution strategy is based on Newton's method.
at each Newton iteration, the solver assembles a residual vector, :math:`R`,
collecting the :math:`n_c` discrete mass conservation equations and the volume
constraint for all the control volumes.
The solver also assembles the Jacobian matrix :math:`J` containing the analytical
derivatives of :math:`R` with respect to the primary variables, namely, pressure
and component densities.
The Newton update, :math:`\delta X`, is then computed as:

.. math::
  \delta X := - J^{-1} R,

The linear system is solved with one of the solvers described in :doc:`/coreComponents/linearAlgebra/docs/LinearSolverParameters`.
The Newton update is then applied to the primary variables:

..  math::
  X := X + \delta X.

This procedure is repeated until convergence.

.. _usage:

Usage and model parameters
==========================

The following attributes are supported:

.. include:: /coreComponents/fileIO/schema/docs/CompositionalMultiphaseFlow.rst

.. _input_example:

Input example
=========================

.. code-block:: xml

  <Solvers
    gravityVector="0.0,0.0,-9.81">

    <CompositionalMultiphaseFlow name="compflow"
                                 verboseLevel="1"
                                 gravityFlag="1"
                                 discretization="fluidTPFA"
                                 fluidName="fluid1"
                                 solidName="rock"
                                 relPermName="relperm"
                                 temperature="297.15"
                                 useMass="0"
                                 targetRegions="Region2">
      <SystemSolverParameters name="SystemSolverParameters"
                              krylovTol="1.0e-10"
                              newtonTol="1.0e-6"
                              maxIterNewton="15"
                              useDirectSolver="1"
                              solverType="Klu"
                              ilut_fill="0"
                              ilut_drop="0"/>
    </CompositionalMultiphaseFlow>
  </Solvers>