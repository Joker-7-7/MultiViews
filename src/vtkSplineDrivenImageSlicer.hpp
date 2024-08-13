/// \class vtkSplineDrivenImageSlicer
/// \brief Reslicing of a volume along a path
///
/// Straightened Curved Planar Reformation (Stretched-CPR) builds a 2D image
/// from an input path and an input volume. Each point of the path is
/// considered as the center of a 2D vtkImageReslicer. Reslicers axes are set
/// orthogonal to the path. Reslicers output are appended on the z axis. Thus
/// the output of this filter is a volume with central XZ- and YZ-slices
/// corresponding to the Straightened-CPR.
///
/// Input: vtkImageData (InputConnection) and vtkPolyData (PathConnection)
/// one polyline representing the path. Typically, the output of vtkSpline can
/// be used as path input.
///
/// \see Kanitsar et al. "CPR - Curved Planar Reformation", Proc. IEEE  Visualization, 2002, 37-44
/// \author Jerome Velut
/// \date 6 february 2011

/// Refactor by WorHyako

#ifndef VTKSPLINEDRIVENIMAGESLICER_HPP
#define VTKSPLINEDRIVENIMAGESLICER_HPP

#include "vtkFrenetSerretFrame.hpp"

#include "vtkImageAlgorithm.h"
#include "vtkImageAppend.h"

class vtkImageReslice;

class VTK_EXPORT vtkSplineDrivenImageSlicer final : public vtkImageAlgorithm {
public:
    vtkSplineDrivenImageSlicer(const vtkSplineDrivenImageSlicer&) = delete;

    void operator=(const vtkSplineDrivenImageSlicer&) = delete;

    //vtkTypeRevisionMacro(vtkSplineDrivenImageSlicer,vtkImageAlgorithm);
    static vtkSplineDrivenImageSlicer* New();

    vtkSmartPointer<vtkImageAppend> imageStitching;

    /**
     * Specify the path represented by a vtkPolyData which contains PolyLines
     */
    void SetPathConnection(int id, vtkAlgorithmOutput* algOutput);

    void SetPathConnection(vtkAlgorithmOutput* algOutput);

    vtkAlgorithmOutput* GetPathConnection();

    vtkSetVector2Macro(SliceExtent, vtkIdType);

    vtkGetVector2Macro(SliceExtent, vtkIdType);

    vtkSetVector2Macro(SliceSpacing, double);

    vtkGetVector2Macro(SliceSpacing, double);

    vtkSetMacro(SliceThickness, double);

    vtkGetMacro(SliceThickness, double);

    vtkSetMacro(OffsetPoint, vtkIdType);

    vtkGetMacro(OffsetPoint, vtkIdType);

    vtkSetMacro(OffsetLine, vtkIdType);

    vtkGetMacro(OffsetLine, vtkIdType);

    vtkSetMacro(ProbeInput, vtkIdType);

    vtkGetMacro(ProbeInput, vtkIdType);

    vtkBooleanMacro(ProbeInput, vtkIdType);

    vtkSetMacro(Incidence, double);

    vtkGetMacro(Incidence, double);

protected:
    vtkSplineDrivenImageSlicer();

    ~vtkSplineDrivenImageSlicer() final;

    /**
     *  RequestData is called by the pipeline process
     */
    int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) final;

    int FillInputPortInformation(int port, vtkInformation* info) final;

    int FillOutputPortInformation(int, vtkInformation*) final;

    int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) final;

private:
    /**
     *  Computes local tangent along path input
     */
    vtkFrenetSerretFrame* _localFrenetFrames;

    /**
     *  Reslicers array
     */
    vtkImageReslice* _reslicer;

    /**
     * Number of pixels nx, ny in the slice space around the center points
     */
    vtkIdType SliceExtent[2];

    /**
     * Pixel size sx, sy of the output slice
     */
    double SliceSpacing[2];

    /**
     *  Slice thickness (useful for volumic reconstruction)
     */
    double SliceThickness;

    /**
     *  Rotation of the initial normal vector.
     */
    double Incidence;

    /**
     *  Id of the point where the reslicer proceed
     */
    vtkIdType OffsetPoint;

    /**
     *  Id of the line cell where to get the reslice center
     */
    vtkIdType OffsetLine;

    /**
     *  If true, the output plane (2nd output probes the input image)
     */
    vtkIdType ProbeInput;
};

#endif
