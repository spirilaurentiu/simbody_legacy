#ifndef SimTK_SIMBODY_FORCE_SUBSYSTEM_H_
#define SimTK_SIMBODY_FORCE_SUBSYSTEM_H_

/* -------------------------------------------------------------------------- *
 *                               Simbody(tm)                                  *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK biosimulation toolkit originating from           *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org/home/simbody.  *
 *                                                                            *
 * Portions copyright (c) 2006-12 Stanford University and the Authors.        *
 * Authors: Michael Sherman                                                   *
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

namespace SimTK {

/**
 * This is logically an abstract class, more specialized than "Subsystem"
 * but not yet concrete.
 */
class SimTK_SIMBODY_EXPORT ForceSubsystem : public Subsystem {
public:
    /// forward declaration of extendable internals
    class Guts;

    ForceSubsystem() : Subsystem() { }

    SimTK_PIMPL_DOWNCAST(ForceSubsystem, Subsystem);
    Guts& updRep();
    const Guts& getRep() const;

};

typedef ForceSubsystem::Guts ForceSubsystemRep;

} // namespace SimTK

#endif // SimTK_SIMBODY_FORCE_SUBSYSTEM_H_
