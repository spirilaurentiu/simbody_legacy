#ifndef SimTK_SIMBODY_HUNT_CROSSLEY_FORCE_H_
#define SimTK_SIMBODY_HUNT_CROSSLEY_FORCE_H_

/* -------------------------------------------------------------------------- *
 *                               Simbody(tm)                                  *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK biosimulation toolkit originating from           *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org/home/simbody.  *
 *                                                                            *
 * Portions copyright (c) 2008-12 Stanford University and the Authors.        *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#include "SimTKcommon.h"

#include "simbody/internal/common.h"
#include "simbody/internal/Force.h"

namespace SimTK {

class GeneralContactSubsystem;
class HuntCrossleyForceImpl;

/**
 * This class models the forces generated by simple point contacts, such as between
 * two spheres, or a sphere and a half space.  This includes components for the normal
 * restoring force, dissipation in the material, and surface friction.  This force
 * is only applied to point contacts.  Other contacts, such as those involving triangle
 * meshes, are ignored.
 *
 * This class relies on a GeneralContactSubsystem to identify contacts, then applies
 * forces to all contacts in a single contact set.  To use it, do the following:
 *
 * <ol>
 * <li>Add a GeneralContactSubsystem and GeneralForceSubsystem to a MultibodySystem.</li>
 * <li>Create a contact set within the GeneralContactSubsystem, and add ContactGeometry::Sphere
 * and ContactGeometry::HalfSpace objects to it.</li>
 * <li>Add a HuntCrossleyForce to the GeneralForceSubsystem, and call setBodyParameters() on it
 * once for each body in the contact set.</li>
 * </ol>
 *
 * <h1>Normal Force Components</h1>
 *
 * The force in the normal direction is based on a model due to Hunt & Crossley: K. H. Hunt
 * and F. R. E. Crossley, "Coefficient of Restitution Interpreted as Damping in Vibroimpact,"
 * ASME Journal of Applied Mechanics, pp. 440-445, June 1975. This is
 * a continuous model based on Hertz elastic contact theory,
 * which correctly reproduces the empirically observed dependence
 * on velocity of coefficient of restitution, where e=(1-cv) for 
 * (small) impact velocity v and a material property c with units 1/v. Note that
 * c can be measured right off the coefficient of restitution-vs.-velocity
 * curves: it is the absolute value of the slope at low velocities.
 *
 * Given a collision between two spheres, or a sphere and a plane,
 * we can generate a contact force from this equation
 *      f = kx^n(1 + 3/2 cv)
 * where k is a stiffness constant incorporating material properties
 * and geometry (to be defined below), x is penetration depth and 
 * v = dx/dt is penetration rate (positive during penetration and
 * negative during rebound). Exponent n depends on the surface
 * geometry. For Hertz contact where the geometry can be approximated
 * by sphere (or sphere-plane) interactions, which is all we are
 * currently handling here, n=3/2.
 *
 * Stiffness k is defined in terms of the relative radius of curvature R and
 * effective plane-strain modulus E, each of which is a combination of
 * the description of the two individual contacting elements:
 *
 * <pre>
 *          R1*R2                                         E2^(2/3)
 *     R = -------,  E = (s1 * E1^(2/3))^(3/2),  s1= ------------------- 
 *         R1 + R2                                   E1^(2/3) + E2^(2/3)
 * </pre>
 *     
 *     c = c1*s1 + c2*(1-s1)
 *     k = (4/3) sqrt(R) E
 *     f = k x^(3/2) (1 + 3/2 c xdot)
 *     pe = 2/5 k x^(5/2)
 * Also, we can calculate the contact patch radius a as
 *     a = sqrt(R*x)
 *
 * In the above, E1 and E2 are the *plane strain* moduli. If you have instead
 * Young's modulus Y1 and Poisson's ratio p1, then E1=Y1/(1-p1^2). The interface
 * to this subsystem asks for E1 (pressure/%strain) and c1 (1/velocity), and
 * E2,c2 only.
 * 
 * <h1>Friction Force</h1>
 *
 * The friction force is based on a model by Michael Hollars:
 *
 * f = fn*[min(vs/vt,1)*(ud+2(us-ud)/(1+(vs/vt)^2))+uv*vs]
 *
 * where fn is the normal force at the contact point, vs is the slip (tangential)
 * velocity of the two bodies at the contact point, vt is a transition velocity
 * (see below), and us, ud, and uv are the coefficients of static, dynamic, and
 * viscous friction respectively.  Each of the three friction coefficients is calculated
 * based on the friction coefficients of the two bodies in contact:
 *
 * u = 2*u1*u2/(u1+u2)
 *
 * Because the friction force is a continuous function of the slip velocity, this
 * model cannot represent stiction; as long as a tangential force is applied, the
 * two bodies will move relative to each other.  There will always be a nonzero
 * drift, no matter how small the force is.  The transition velocity vt acts as an
 * upper limit on the drift velocity.  By setting vt to a sufficiently small value,
 * the drift velocity can be made arbitrarily small, at the cost of making the
 * equations of motion very stiff.  The default value of vt is 0.01.
 */
class SimTK_SIMBODY_EXPORT HuntCrossleyForce : public Force {
public:
    /**
     * Create a Hunt-Crossley contact model.
     * 
     * @param forces         the subsystem which will own this HuntCrossleyForce element
     * @param contacts       the subsystem to which this contact model should be applied
     * @param contactSet     the index of the contact set to which this contact model will be applied
     */
    HuntCrossleyForce(GeneralForceSubsystem& forces, 
                      GeneralContactSubsystem& contacts, 
                      ContactSetIndex contactSet);
    /**
     * Set the material parameters for a surface in the contact set.
     *
     * @param surfIndex       the index of the surface within the contact set
     * @param stiffness       the stiffness constant (k) for the body
     * @param dissipation     the dissipation coefficient (c) for the body
     * @param staticFriction  the coefficient of static friction (us) for the body
     * @param dynamicFriction the coefficient of dynamic friction (ud) for the body
     * @param viscousFriction the coefficient of viscous friction (uv) for the body
     */
    void setBodyParameters
       (ContactSurfaceIndex surfIndex, Real stiffness, Real dissipation, 
        Real staticFriction, Real dynamicFriction, Real viscousFriction);
    /**
     * Get the transition velocity (vt) of the friction model.
     */
    Real getTransitionVelocity() const;
    /**
     * Set the transition velocity (vt) of the friction model.
     */
    void setTransitionVelocity(Real v);
    /**
     * Retrieve the ContactSetIndex that was associated with this 
     * %HuntCrossleyForce on construction. 
     */
    ContactSetIndex getContactSetIndex() const;

    SimTK_INSERT_DERIVED_HANDLE_DECLARATIONS(HuntCrossleyForce, HuntCrossleyForceImpl, Force);
};

} // namespace SimTK

#endif // SimTK_SIMBODY_HUNT_CROSSLEY_FORCE_H_
