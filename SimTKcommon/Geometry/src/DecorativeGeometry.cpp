/* -------------------------------------------------------------------------- *
 *                       Simbody(tm): SimTKcommon                             *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK biosimulation toolkit originating from           *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org/home/simbody.  *
 *                                                                            *
 * Portions copyright (c) 2006-12 Stanford University and the Authors.        *
 * Authors: Michael Sherman                                                   *
 * Contributors: Jack Middleton, Peter Eastman                                *
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

#include "SimTKcommon/basics.h"
#include "SimTKcommon/Simmatrix.h"
#include "SimTKcommon/internal/DecorativeGeometry.h"

#include "DecorativeGeometryRep.h"

#include <cmath>

namespace SimTK {

// Some common RGB values; these constants are global external symbols exported
// by the library, defined as externs in DecorativeGeometry.h.

SimTK_SimTKCOMMON_EXPORT const Vec3 Black   = Vec3( 0, 0, 0);
SimTK_SimTKCOMMON_EXPORT const Vec3 Gray    = Vec3(.5,.5,.5);
SimTK_SimTKCOMMON_EXPORT const Vec3 Red     = Vec3( 1, 0, 0);
SimTK_SimTKCOMMON_EXPORT const Vec3 Green   = Vec3( 0, 1, 0);
SimTK_SimTKCOMMON_EXPORT const Vec3 Blue    = Vec3( 0, 0, 1);
SimTK_SimTKCOMMON_EXPORT const Vec3 Yellow  = Vec3( 1, 1, 0);
SimTK_SimTKCOMMON_EXPORT const Vec3 Orange  = Vec3( 1,.5, 0);
SimTK_SimTKCOMMON_EXPORT const Vec3 Magenta = Vec3( 1, 0, 1);
SimTK_SimTKCOMMON_EXPORT const Vec3 Purple  = Vec3(.5, 0,.5);
SimTK_SimTKCOMMON_EXPORT const Vec3 Cyan    = Vec3( 0, 1, 1);
SimTK_SimTKCOMMON_EXPORT const Vec3 White   = Vec3( 1, 1, 1);

    /////////////////////////
    // DECORATIVE GEOMETRY //
    /////////////////////////

// This is an owner handle if there is no rep or if the rep points back
// to this handle.
bool DecorativeGeometry::isOwnerHandle() const {
    return rep==0 || rep->myHandle == this;
}
bool DecorativeGeometry::isEmptyHandle() const {return rep==0;}

DecorativeGeometry::~DecorativeGeometry() {
    if (isOwnerHandle())
        delete rep;
    rep = 0;
}

DecorativeGeometry::DecorativeGeometry(const DecorativeGeometry& src) : rep(0) {
    if (src.rep) {
        rep = src.rep->clone();
        rep->setMyHandle(*this);
    }
}

DecorativeGeometry& DecorativeGeometry::operator=(const DecorativeGeometry& src) {
    if (&src != this) {
        if (isOwnerHandle()) delete rep;
        rep = 0;
        if (src.rep) {
            rep = src.rep->clone();
            rep->setMyHandle(*this);
        }
    }
    return *this;
}

DecorativeGeometry& DecorativeGeometry::setBodyId(int b) {updRep().setBodyId(b);return *this;}
int DecorativeGeometry::getBodyId() const {return getRep().getBodyId();}

DecorativeGeometry& DecorativeGeometry::setTransform(const Transform& X_BD) {updRep().setTransform(X_BD);return *this;}
const Transform& DecorativeGeometry::getTransform() const    {return getRep().getTransform();}

DecorativeGeometry& DecorativeGeometry::setResolution(Real r) {updRep().setResolution(r);return *this;}
Real DecorativeGeometry::getResolution() const {return getRep().getResolution();}

DecorativeGeometry& DecorativeGeometry::setScaleFactors(const Vec3& s) 
{   updRep().setScaleFactors(s); return *this; }
const Vec3& DecorativeGeometry::getScaleFactors() const 
{   return getRep().getScaleFactors(); }

DecorativeGeometry& DecorativeGeometry::setColor(const Vec3& rgb) {updRep().setColor(rgb);return *this;}
const Vec3& DecorativeGeometry::getColor() const   {return getRep().getColor();}

DecorativeGeometry& DecorativeGeometry::setOpacity(Real o)  {updRep().setOpacity(o);return *this;}
Real DecorativeGeometry::getOpacity()  const {return getRep().getOpacity();}

DecorativeGeometry& DecorativeGeometry::setLineThickness(Real t) {updRep().setLineThickness(t);return *this;}
Real DecorativeGeometry::getLineThickness() const {return getRep().getLineThickness();}

DecorativeGeometry& DecorativeGeometry::setRepresentation(const DecorativeGeometry::Representation& r) {
    updRep().setRepresentation(r);return *this;
}

DecorativeGeometry& DecorativeGeometry::setFaceCamera(int shouldFace) 
{   updRep().setFaceCamera(shouldFace);return *this; }
int DecorativeGeometry::getFaceCamera() const 
{   return getRep().getFaceCamera(); }

DecorativeGeometry::Representation
DecorativeGeometry::getRepresentation() const {return getRep().getRepresentation();}

void DecorativeGeometry::implementGeometry(DecorativeGeometryImplementation& geometry) const
{
    getRep().implementGeometry(geometry);
}


    //////////////////////
    // DECORATIVE POINT //
    //////////////////////

/*static*/ bool 
DecorativePoint::isInstanceOf(const DecorativeGeometry& s) {
    return DecorativePointRep::isA(s.getRep());
}
/*static*/ const DecorativePoint&
DecorativePoint::downcast(const DecorativeGeometry& s) {
    assert(isInstanceOf(s));
    return reinterpret_cast<const DecorativePoint&>(s);
}
/*static*/ DecorativePoint&
DecorativePoint::updDowncast(DecorativeGeometry& s) {
    assert(isInstanceOf(s));
    return reinterpret_cast<DecorativePoint&>(s);
}

const DecorativePointRep& 
DecorativePoint::getRep() const {
    return dynamic_cast<const DecorativePointRep&>(*rep);
}
DecorativePointRep&       
DecorativePoint::updRep() {
    return dynamic_cast<DecorativePointRep&>(*rep);
}

DecorativePoint::DecorativePoint(const Vec3& p) {
    rep = new DecorativePointRep(p);
    rep->setMyHandle(*this);
}
DecorativePoint& DecorativePoint::setPoint(const Vec3& p) {
    DecorativePointRep::downcast(*rep).setPoint(p); return *this;
}
const Vec3& DecorativePoint::getPoint() const {
    return DecorativePointRep::downcast(*rep).getPoint();
}


    /////////////////////
    // DECORATIVE LINE //
    /////////////////////

/*static*/ bool 
DecorativeLine::isInstanceOf(const DecorativeGeometry& s) {
    return DecorativeLineRep::isA(s.getRep());
}
/*static*/ const DecorativeLine&
DecorativeLine::downcast(const DecorativeGeometry& s) {
    assert(isInstanceOf(s));
    return reinterpret_cast<const DecorativeLine&>(s);
}
/*static*/ DecorativeLine&
DecorativeLine::updDowncast(DecorativeGeometry& s) {
    assert(isInstanceOf(s));
    return reinterpret_cast<DecorativeLine&>(s);
}

const DecorativeLineRep& 
DecorativeLine::getRep() const {
    return dynamic_cast<const DecorativeLineRep&>(*rep);
}
DecorativeLineRep&       
DecorativeLine::updRep() {
    return dynamic_cast<DecorativeLineRep&>(*rep);
}

DecorativeLine::DecorativeLine(const Vec3& p1, const Vec3& p2) {
    rep = new DecorativeLineRep(p1,p2);
    rep->setMyHandle(*this);
}
DecorativeLine& DecorativeLine::setPoint1(const Vec3& p1) {
    DecorativeLineRep::downcast(*rep).setPoint1(p1); return *this;
}
DecorativeLine& DecorativeLine::setPoint2(const Vec3& p2) {
    DecorativeLineRep::downcast(*rep).setPoint2(p2); return *this;
}
DecorativeLine& DecorativeLine::setEndpoints(const Vec3& p1, const Vec3& p2) {
    DecorativeLineRep::downcast(*rep).setEndpoints(p1,p2); return *this;
}
const Vec3& DecorativeLine::getPoint1() const {
    return DecorativeLineRep::downcast(*rep).getPoint1();
}
const Vec3& DecorativeLine::getPoint2() const {
    return DecorativeLineRep::downcast(*rep).getPoint2();
}


    ///////////////////////
    // DECORATIVE CIRCLE //
    ///////////////////////

DecorativeCircle::DecorativeCircle(Real radius) {
    rep = new DecorativeCircleRep(radius);
    rep->setMyHandle(*this);
}
void DecorativeCircle::setRadius(Real r) {
    DecorativeCircleRep::downcast(*rep).setRadius(r);
}
Real DecorativeCircle::getRadius() const {
    return DecorativeCircleRep::downcast(*rep).getRadius();
}


    ///////////////////////
    // DECORATIVE SPHERE //
    ///////////////////////

DecorativeSphere::DecorativeSphere(Real radius) {
    rep = new DecorativeSphereRep(radius);
    rep->setMyHandle(*this);
}

void DecorativeSphere::setRadius(Real r) {
    DecorativeSphereRep::downcast(*rep).setRadius(r);
}
Real DecorativeSphere::getRadius() const {
    return DecorativeSphereRep::downcast(*rep).getRadius();
}


    //////////////////////////
    // DECORATIVE ELLIPSOID //
    //////////////////////////

DecorativeEllipsoid::DecorativeEllipsoid(const Vec3& radii) {
    rep = new DecorativeEllipsoidRep(radii);
    rep->setMyHandle(*this);
}

void DecorativeEllipsoid::setRadii(const Vec3& r) {
    DecorativeEllipsoidRep::downcast(*rep).setRadii(r);
}
const Vec3& DecorativeEllipsoid::getRadii() const {
    return DecorativeEllipsoidRep::downcast(*rep).getRadii();
}
    //////////////////////
    // DECORATIVE BRICK //
    //////////////////////

DecorativeBrick::DecorativeBrick(const Vec3& xyzHalfLengths) {
    rep = new DecorativeBrickRep(xyzHalfLengths);
    rep->setMyHandle(*this);
}

void DecorativeBrick::setHalfLengths(const Vec3& xyzHalfLengths) {
    DecorativeBrickRep::downcast(*rep).setHalfLengths(xyzHalfLengths);
}
const Vec3& DecorativeBrick::getHalfLengths() const {
    return DecorativeBrickRep::downcast(*rep).getHalfLengths();
}

    /////////////////////////
    // DECORATIVE CYLINDER //
    /////////////////////////

DecorativeCylinder::DecorativeCylinder(Real radius, Real halfHeight) {
    rep = new DecorativeCylinderRep(radius,halfHeight);
    rep->setMyHandle(*this);
}

void DecorativeCylinder::setRadius(Real r) {
    DecorativeCylinderRep::downcast(*rep).setRadius(r);
}
void DecorativeCylinder::setHalfHeight(Real r) {
    DecorativeCylinderRep::downcast(*rep).setHalfHeight(r);
}

Real DecorativeCylinder::getRadius() const {
    return DecorativeCylinderRep::downcast(*rep).getRadius();
}
Real DecorativeCylinder::getHalfHeight() const {
    return DecorativeCylinderRep::downcast(*rep).getHalfHeight();
}

    //////////////////////
    // DECORATIVE FRAME //
    //////////////////////

DecorativeFrame::DecorativeFrame(Real axisLength) {
    rep = new DecorativeFrameRep(axisLength);
    rep->setMyHandle(*this);
}

void DecorativeFrame::setAxisLength(Real l) {
    DecorativeFrameRep::downcast(*rep).setAxisLength(l);
}
Real DecorativeFrame::getAxisLength() const {
    return DecorativeFrameRep::downcast(*rep).getAxisLength();
}


    /////////////////////
    // DECORATIVE TEXT //
    /////////////////////

DecorativeText::DecorativeText(const std::string& label) {
    rep = new DecorativeTextRep(label);
    rep->setMyHandle(*this);
}

void DecorativeText::setText(const std::string& label) {
    DecorativeTextRep::downcast(*rep).setText(label);
}
const std::string& DecorativeText::getText() const {
return DecorativeTextRep::downcast(*rep).getText();
}

    /////////////////////
    // DECORATIVE MESH //
    /////////////////////

DecorativeMesh::DecorativeMesh(const PolygonalMesh& mesh) {
    rep = new DecorativeMeshRep(mesh);
    rep->setMyHandle(*this);
}
const PolygonalMesh& DecorativeMesh::getMesh() const {
    return DecorativeMeshRep::downcast(*rep).getMesh();
}


    /////////////////
    // DECORATIONS //
    /////////////////

/*static*/ bool 
Decorations::isInstanceOf(const DecorativeGeometry& s) {
    return DecorationsRep::isA(s.getRep());
}
/*static*/ const Decorations&
Decorations::downcast(const DecorativeGeometry& s) {
    assert(isInstanceOf(s));
    return reinterpret_cast<const Decorations&>(s);
}
/*static*/ Decorations&
Decorations::updDowncast(DecorativeGeometry& s) {
    assert(isInstanceOf(s));
    return reinterpret_cast<Decorations&>(s);
}

const DecorationsRep& 
Decorations::getRep() const {
    return dynamic_cast<const DecorationsRep&>(*rep);
}
DecorationsRep&       
Decorations::updRep() {
    return dynamic_cast<DecorationsRep&>(*rep);
}

Decorations::Decorations() {
    rep = new DecorationsRep();
    rep->setMyHandle(*this);
}
Decorations::Decorations(const DecorativeGeometry& decoration) {
    rep = new DecorationsRep();
    rep->setMyHandle(*this);
    updRep().addDecoration(decoration);
}
Decorations& Decorations::
addDecoration(const DecorativeGeometry& decoration) {
    updRep().addDecoration(decoration);
    return *this;
}
Decorations& Decorations::
addDecoration(const Transform& placement,
              const DecorativeGeometry& decoration) {
    updRep().addDecoration(placement, decoration);
    return *this;
}
int Decorations::getNumDecorations() const
{   return getRep().getNumDecorations(); }
const DecorativeGeometry& Decorations::getDecoration(int i) const
{   return getRep().getDecoration(i); }

} // namespace SimTK

