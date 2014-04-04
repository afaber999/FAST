#ifndef IMAGE2D_HPP
#define IMAGE2D_HPP

#include "SmartPointers.hpp"
#include "StaticImage.hpp"
#include "OpenCLManager.hpp"
#include "ExecutionDevice.hpp"
#include "OpenCLImageAccess2D.hpp"
#include "ImageAccess2D.hpp"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
namespace fast {

class Image2D: public StaticImage {
    FAST_OBJECT(Image2D)
    public:
        void createImage(unsigned int width, unsigned int height, DataType type, unsigned int nrOfComponents, ExecutionDevice::pointer device);
        void createImage(unsigned int width, unsigned int height, DataType type, unsigned int nrOfComponents, ExecutionDevice::pointer device, const void * data);
        OpenCLImageAccess2D getOpenCLImageAccess(accessType type, OpenCLDevice::pointer);
        ImageAccess2D getImageAccess(accessType type);
        ~Image2D() { freeAll(); };
    private:
        Image2D();
        boost::unordered_map<OpenCLDevice::pointer, cl::Image2D*> mCLImages;
        boost::unordered_map<OpenCLDevice::pointer, bool> mCLImagesIsUpToDate;
        boost::unordered_map<OpenCLDevice::pointer, bool> mCLImagesAccess;
        void * mHostData;
        bool mHostHasData;
        bool mHostDataIsUpToDate;
        bool mHostDataIsBeingAccessed;
        bool isDataModified();
        void updateOpenCLImageData(OpenCLDevice::pointer device);
        void updateHostData();
        void setAllDataToOutOfDate();
        bool isAnyDataBeingAccessed();
        void transferCLImageFromHost(OpenCLDevice::pointer device);
        void transferCLImageToHost(OpenCLDevice::pointer device);
        bool isInitialized();
        void free(ExecutionDevice::pointer device);
        void freeAll();
};

} // end namespace fast

#endif