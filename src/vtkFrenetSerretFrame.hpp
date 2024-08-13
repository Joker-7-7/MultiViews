// Copyright (c) 2010, Jérôme Velut
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNER ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
// NO EVENT SHALL THE COPYRIGHT OWNER BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/// Refactor by WorHyako

/// \class vtkFrenetSerretFrame
/// \brief Compute tangent and normal vectors to a polyline
///
/// Given a polyline as input, this filter computes the Frenet-Serret frame
/// at each point. The output contains the tangent and normal vectors to the
/// curve. These vectors are appended, so that input array are not overwrited
/// \see vtkImagePathReslice for a use-case.
///
/// \todo Comment this class. (cxx)
/// \todo [ENH] compute the whole chart (B=N^T) and put it in the
/// PointData as a tensor.
///
/// \author Jerome Velut
/// \date 21 jan 2010

#ifndef VTKFRENETSERRETFRAME_HPP
#define VTKFRENETSERRETFRAME_HPP

#include <vtkPoints.h>
#include<vtkPolyDataAlgorithm.h>
#include<vtkPolyData.h>
#include<vtkCellArray.h>

#if defined(NO_DLL) && defined (WIN32)
#undef VTK_EXPORT
#define VTK_EXPORT
#endif

class VTK_EXPORT vtkFrenetSerretFrame final : public vtkPolyDataAlgorithm {
public:
    //   vtkTypeRevisionMacro(vtkFrenetSerretFrame,vtkPolyDataAlgorithm);
    static vtkFrenetSerretFrame* New();

    vtkFrenetSerretFrame(const vtkFrenetSerretFrame&) = delete;

    void operator=(const vtkFrenetSerretFrame&) = delete;

    /// Set ConsistentNormals to 1 if you want your frames to be 'smooth'.
    /// Note that in this case, the normal to the curve will not represent the
    /// acceleration, ie this is no more Frenet-Serret chart.
    vtkBooleanMacro(_consistentNormals, int);

    vtkSetMacro(_consistentNormals, int);

    vtkGetMacro(_consistentNormals, int);

    /// If yes, computes the cross product between Tangent and Normal to get the binormal vector.
    vtkBooleanMacro(_computeBinormal, int);

    vtkSetMacro(_computeBinormal, int);

    vtkGetMacro(_computeBinormal, int);

    /// Define the inclination of the consistent normals. Radian value.
    vtkSetMacro(_viewUp, double);

    vtkGetMacro(_viewUp, double);

    /**
     * Rotate a vector around an axis
     * @param vector    Vector to rotate. In place modification
     * @param axis      Vector defining the axis to turn around
     * @param angle     Rotation angle in radian
     */
    static void RotateVector(double* vector, const double* axis, double angle);

protected:
    vtkFrenetSerretFrame();

    ~vtkFrenetSerretFrame() final = default;

    int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) final;

    int FillInputPortInformation(int port, vtkInformation* info) final;

    /**
     * Computes the derivative between 2 points (Next - Last)
     * @param pointIdNext   give first point Id
     * @param pointIdLast   give second point Id
     * @param tangent       fill a 3-array with the derivative value}
     * @note                If Next is [i+1], Last is [i-1], your are computing the centered tangent at [i]
     */
    void ComputeTangentVectors(vtkIdType pointIdNext, vtkIdType pointIdLast, double* tangent);

    /**
     *
     * @param tgNext give a first derivative
     * @param tgLast give a first derivative
     * @param normal fill a 3-array with the second derivative value
     */
    void ComputeNormalVectors(double* tgNext, double* tgLast, double* normal);

    /**
     * ConsistentNormal depends on the local tangent and the last computed normal. This is a projection of lastNormal
     * on the plan defined by tangent
     * @param tangent       give the tangent
     * @param lastNormal    give the reference normal
     * @param normal        fill a 3-array with the normal vector
     */
    void ComputeConsistentNormalVectors(double* tangent, double* lastNormal, double* normal);

private:
    /**
     * If 1, a Binormal array is added to the output
     */
    int _computeBinormal;

    /**
     * Boolean. If 1, successive normals are computed in smooth manner
     */
    int _consistentNormals;

    /**
     * \see ComputeConsistentNormalVectors
     * Define the inclination of the normal vectors in case of ConsistentNormals is On
     */
    double _viewUp;
};

#endif
