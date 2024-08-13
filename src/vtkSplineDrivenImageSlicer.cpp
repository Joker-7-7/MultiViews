#include "vtkSplineDrivenImageSlicer.hpp"
#include "vtkFrenetSerretFrame.hpp"

#include <vtkImageReslice.h>
#include <vtkProbeFilter.h>
#include <vtkMatrix4x4.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>

//vtkCxxRevisionMacro(vtkSplineDrivenImageSlicer, "$Revision: 1.31 $");
vtkStandardNewMacro(vtkSplineDrivenImageSlicer);

vtkSplineDrivenImageSlicer::vtkSplineDrivenImageSlicer()
        : OffsetPoint(0),
          ProbeInput(0),
          OffsetLine(0),
          SliceThickness(1),
          _localFrenetFrames(vtkFrenetSerretFrame::New()),
          _reslicer(vtkImageReslice::New()),
        imageStitching{ vtkSmartPointer<vtkImageAppend>::New() }
{
    SliceExtent[0] = 15;
    SliceExtent[1] = 15;
    SliceSpacing[0] = 1;
    SliceSpacing[1] = 1;

    SetNumberOfInputPorts(2);
    SetNumberOfOutputPorts(2);

    /// by default process active point scalars
    SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
}

vtkSplineDrivenImageSlicer::~vtkSplineDrivenImageSlicer() {
    _localFrenetFrames->Delete();
    _reslicer->Delete();
}

void vtkSplineDrivenImageSlicer::SetPathConnection(int id, vtkAlgorithmOutput* algOutput) {
    if (id < 0) {
        vtkErrorMacro("Bad index " << id << " for source.");
        return;
    }

    int numConnections = GetNumberOfInputConnections(1);
    if (id < numConnections) {
        SetNthInputConnection(1, id, algOutput);
    } else {
        if (id == numConnections && algOutput) {
            AddInputConnection(1, algOutput);
        } else {
            if (algOutput) {
                vtkWarningMacro("The source id provided is larger than the maximum "
                                "source id, using " << numConnections << " instead.");
                AddInputConnection(1, algOutput);
            }
        }
    }
}

int vtkSplineDrivenImageSlicer::FillInputPortInformation(int port, vtkInformation* info) {
    if (port == 0) {
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    } else {
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    }
    return 1;
}

int vtkSplineDrivenImageSlicer::FillOutputPortInformation(int port, vtkInformation* info) {
    if (port == 0) {
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
    }
    if (port == 1) {
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
    }
    return 1;
}

int vtkSplineDrivenImageSlicer::RequestInformation(vtkInformation*,
                                                   vtkInformationVector**,
                                                   vtkInformationVector* outputVector) {
    /// get the info objects
    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    int extent[6] = {0, static_cast<int>(SliceExtent[0] - 1),
                     0, static_cast<int>(SliceExtent[1] - 1),
                     0, 1};
    double spacing[3] = {SliceSpacing[0], SliceSpacing[1], SliceThickness};

    outInfo->Set(vtkDataObject::SPACING(), spacing, 3);
    outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);

    return 1;
}

int vtkSplineDrivenImageSlicer::RequestData(vtkInformation*,
                                            vtkInformationVector** inputVector,
                                            vtkInformationVector* outputVector) {
    /// get the info objects
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation* pathInfo = inputVector[1]->GetInformationObject(0);
    vtkInformation* outImageInfo = outputVector->GetInformationObject(0);
    vtkInformation* outPlaneInfo = outputVector->GetInformationObject(1);

    /// get the input and ouptut
    vtkImageData* input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData* inputCopy = vtkImageData::New();
    inputCopy->ShallowCopy(input);
    vtkPolyData* inputPath = vtkPolyData::SafeDownCast(pathInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkImageData* outputImage = vtkImageData::SafeDownCast(outImageInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData* outputPlane = vtkPolyData::SafeDownCast(outPlaneInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkSmartPointer<vtkPolyData> pathCopy = vtkSmartPointer<vtkPolyData>::New();
    pathCopy->ShallowCopy(inputPath);

    /// Compute the local normal and tangent to the path
    _localFrenetFrames->SetInputData(pathCopy);
    _localFrenetFrames->Set_viewUp(Incidence);
    _localFrenetFrames->_computeBinormalOn();
    _localFrenetFrames->Update();

    /// path will contain PointData array "Tangents" and "Vectors"
    auto path = static_cast<vtkPolyData*>(_localFrenetFrames->GetOutputDataObject(0));
    /// Count how many points are used in the cells
    /// In case of loop, points may be used several times
    /// (note: not using NumberOfPoints because we want only LINES points...)
    vtkCellArray* lines = path->GetLines();
    lines->InitTraversal();
    vtkIdType nbCellPoints;
    vtkIdType const* points;

    vtkIdType cellId = -1;
    do {
        lines->GetNextCell(nbCellPoints, points);
        cellId++;
    } while (cellId != OffsetLine);

    int ptId = OffsetPoint;
    if (ptId >= nbCellPoints) {
        ptId = nbCellPoints - 1;
    }

    /// Build a new reslicer with image input as input too.
    _reslicer->SetInputData(inputCopy);

    /// Get the Frenet-Serret chart at point ptId:
    /// - position (center)
    /// - tangent T
    /// - normal N
    double center[3];
    path->GetPoints()->GetPoint(ptId, center);
    auto pathTangents = static_cast<vtkDoubleArray*>(path->GetPointData()->GetArray("FSTangents"));
    double tangent[3];
    pathTangents->GetTuple(ptId, tangent);

    auto pathNormals = static_cast<vtkDoubleArray*>(path->GetPointData()->GetArray("FSNormals"));
    double normal[3];
    pathNormals->GetTuple(ptId, normal);

    auto pathBinormals = static_cast<vtkDoubleArray*>
    (path->GetPointData()->GetArray("FSBinormals"));
    double binormal[3];
    pathBinormals->GetTuple(ptId, binormal);

    /// Build the plane output that will represent the slice location in 3D view
    vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();

    double planeOrigin[3];
    double planePoint1[3];
    double planePoint2[3];
    for (int comp = 0; comp < 3; comp++) {
        planeOrigin[comp] = center[comp] - normal[comp] * static_cast<double>(SliceExtent[1]) * SliceSpacing[1] / 2.0
                            - binormal[comp] * static_cast<double>(SliceExtent[0]) * SliceSpacing[0] / 2.0;
        planePoint1[comp] = planeOrigin[comp] + binormal[comp] * static_cast<double>(SliceExtent[0]) * SliceSpacing[0];
        planePoint2[comp] = planeOrigin[comp] + normal[comp] * static_cast<double>(SliceExtent[1]) * SliceSpacing[1];
    }
    plane->SetOrigin(planeOrigin);
    plane->SetPoint1(planePoint1);
    plane->SetPoint2(planePoint2);
    plane->SetResolution(static_cast<int>(SliceExtent[0]), static_cast<int>(SliceExtent[1]));
    plane->Update();

    if (ProbeInput == 1) {
        vtkSmartPointer<vtkProbeFilter> probe = vtkSmartPointer<vtkProbeFilter>::New();
        probe->SetInputConnection(plane->GetOutputPort());
        probe->SetSourceData(inputCopy);
        probe->Update();
        outputPlane->DeepCopy(probe->GetOutputDataObject(0));
    } else {
        outputPlane->DeepCopy(plane->GetOutputDataObject(0));
    }

    /// Build the transformation matrix (inspired from vtkImagePlaneWidget)
    vtkMatrix4x4* resliceAxes = vtkMatrix4x4::New();
    resliceAxes->Identity();
    double origin[4];
    /// According to vtkImageReslice API:
    /// - 1st column contains the resliced image x-axis
    /// - 2nd column contains the resliced image y-axis
    /// - 3rd column contains the normal of the resliced image plane
    /// -> 1st column is normal to the path
    /// -> 3nd column is tangent to the path
    /// -> 2nd column is B = T^N
    for (int comp = 0; comp < 3; comp++) {
        resliceAxes->SetElement(0, comp, binormal[comp]);
        resliceAxes->SetElement(1, comp, normal[comp]);
        resliceAxes->SetElement(2, comp, tangent[comp]);

        origin[comp] = center[comp] - normal[comp] * static_cast<double>(SliceExtent[1]) * SliceSpacing[1] / 2.0
                       - binormal[comp] * static_cast<double>(SliceExtent[0]) * SliceSpacing[0] / 2.0;
    }

    /// Transform the origin in the homogeneous coordinate space.
    /// \todo See why !
    origin[3] = 1.0;
    double originXYZW[4];
    resliceAxes->MultiplyPoint(origin, originXYZW);

    /// Get the new origin from the transposed matrix.
    /// \todo See why !
    resliceAxes->Transpose();
    double neworiginXYZW[4];
    resliceAxes->MultiplyPoint(originXYZW, neworiginXYZW);

    resliceAxes->SetElement(0, 3, neworiginXYZW[0]);
    resliceAxes->SetElement(1, 3, neworiginXYZW[1]);
    resliceAxes->SetElement(2, 3, neworiginXYZW[2]);

    _reslicer->SetResliceAxes(resliceAxes);
    _reslicer->SetInformationInput(input);
    _reslicer->SetInterpolationModeToCubic();

    _reslicer->SetOutputDimensionality(2);
    _reslicer->SetOutputOrigin(0, 0, 0);
    _reslicer->SetOutputExtent(0, SliceExtent[0] - 1,
                               0, SliceExtent[1] - 1,
                               0, 1);
    _reslicer->SetOutputSpacing(SliceSpacing[0], SliceSpacing[1], SliceThickness);
    _reslicer->Update();

    resliceAxes->Delete();
    vtkNew<vtkImageData> tempSlice;
    tempSlice->DeepCopy(outputImage);
    imageStitching->AddInputData(tempSlice);
    outputImage->DeepCopy(_reslicer->GetOutputDataObject(0));
    outputImage->GetPointData()->GetScalars()->SetName("ReslicedImage");
    return 1;
}

void vtkSplineDrivenImageSlicer::SetPathConnection(vtkAlgorithmOutput* algOutput) {
    SetPathConnection(0, algOutput);
}

vtkAlgorithmOutput* vtkSplineDrivenImageSlicer::GetPathConnection() {
    return (GetInputConnection(1, 0));
}
