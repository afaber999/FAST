#include "DeviceManager.hpp"
#include "OpenCLManager.hpp"

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl_gl.h>
#include <OpenGL/OpenGL.h>
#else
#if _WIN32
#include <CL/cl_gl.h>
#else
#include <GL/glx.h>
#include <CL/cl_gl.h>
#endif
#endif

using namespace fast;

DeviceManager& DeviceManager::getInstance() {
    static DeviceManager instance;
    return instance;
}

std::vector<OpenCLDevice::pointer> getDevices(oul::DeviceCriteria criteria, bool enableVisualization) {
    unsigned long * glContext = NULL;
    if(enableVisualization) {
        // Create GL context
#if defined(__APPLE__) || defined(__MACOSX)
        CGLPixelFormatAttribute attribs[] = {};
        CGLPixelFormatObj pix;
        CGLChoosePixelFormat(attribs, &pix, NULL);
        CGLContextObj appleGLContext;
        CGLCreateContext(pix, NULL, &appleGLContext);
        glContext = (unsigned long *)appleGLContext;
#else
#if _WIN32
        // TODO implement windows OpenGL stuff
#else
        int sngBuf[] = { GLX_RGBA,
                         GLX_DOUBLEBUFFER,
                         GLX_RED_SIZE, 1,
                         GLX_GREEN_SIZE, 1,
                         GLX_BLUE_SIZE, 1,
                         GLX_DEPTH_SIZE, 12,
                         None
        };
        Display * display = XOpenDisplay(0);
        XVisualInfo* vi = glXChooseVisual(display, DefaultScreen(display), sngBuf);
        glContext = (unsigned long *)glXCreateContext(display, vi, 0, GL_TRUE);
#endif
#endif
        criteria.setCapabilityCriteria(oul::DEVICE_CAPABILITY_OPENGL_INTEROP);
    }
    oul::OpenCLManager * manager = oul::OpenCLManager::getInstance();
    std::vector<oul::PlatformDevices> platformDevices = manager->getDevices(criteria);

    std::vector<OpenCLDevice::pointer> executionDevices;
    for(unsigned int i = 0; i < platformDevices.size(); i++) {
    for(unsigned int j = 0; j < platformDevices[i].second.size(); j++) {
        std::vector<cl::Device> deviceVector;
        deviceVector.push_back(platformDevices[i].second[j]);
        OpenCLDevice * device = new OpenCLDevice(deviceVector, glContext);
        executionDevices.push_back(OpenCLDevice::pointer(device));
    }}

    return executionDevices;
}

std::vector<OpenCLDevice::pointer> DeviceManager::getAllDevices(
        bool enableVisualization) {

    oul::DeviceCriteria criteria;
    return getDevices(criteria,enableVisualization);
}

std::vector<OpenCLDevice::pointer> DeviceManager::getAllGPUDevices(
        bool enableVisualization) {
    oul::DeviceCriteria criteria;
    criteria.setTypeCriteria(oul::DEVICE_TYPE_GPU);
    return getDevices(criteria,enableVisualization);
}

std::vector<OpenCLDevice::pointer> DeviceManager::getAllCPUDevices(
        bool enableVisualization) {
    oul::DeviceCriteria criteria;
    criteria.setTypeCriteria(oul::DEVICE_TYPE_CPU);
    return getDevices(criteria,enableVisualization);
}

OpenCLDevice::pointer DeviceManager::getOneGPUDevice(
        bool enableVisualization) {

    oul::DeviceCriteria criteria;
    criteria.setTypeCriteria(oul::DEVICE_TYPE_GPU);
    criteria.setDeviceCountCriteria(1);
    std::vector<OpenCLDevice::pointer> devices = getDevices(criteria,enableVisualization);
    return devices[0];
}

OpenCLDevice::pointer DeviceManager::getOneOpenCLDevice(
        bool enableVisualization) {

    oul::DeviceCriteria criteria;

    // Check if a GPU is available first, if not choose any
    criteria.setTypeCriteria(oul::DEVICE_TYPE_GPU);
    criteria.setDeviceCountCriteria(1);
    std::vector<OpenCLDevice::pointer> devices = getDevices(criteria,enableVisualization);
    if(devices.size() > 0) {
        return devices[0];
    } else {
        criteria.setTypeCriteria(oul::DEVICE_TYPE_ANY);
        std::vector<OpenCLDevice::pointer> devices = getDevices(criteria,enableVisualization);
        return devices[0];
    }
}

OpenCLDevice::pointer DeviceManager::getOneCPUDevice(
        bool enableVisualization) {
    oul::DeviceCriteria criteria;
    criteria.setTypeCriteria(oul::DEVICE_TYPE_CPU);
    criteria.setDeviceCountCriteria(1);
    std::vector<OpenCLDevice::pointer> devices = getDevices(criteria,enableVisualization);
    return devices[0];
}

Host::pointer DeviceManager::getHostDevice() {
    return Host::New();
}

void DeviceManager::setDefaultDevice(ExecutionDevice::pointer device) {
    mDefaultComputationDevice = device;
    mDefaultVisualizationDevice = device;
}

void DeviceManager::setDefaultComputationDevice(
        ExecutionDevice::pointer device) {
    mDefaultComputationDevice = device;
}

void DeviceManager::setDefaultVisualizationDevice(
        ExecutionDevice::pointer device) {
    mDefaultVisualizationDevice = device;
}

ExecutionDevice::pointer DeviceManager::getDefaultComputationDevice() {
    return mDefaultComputationDevice;
}

ExecutionDevice::pointer DeviceManager::getDefaultVisualizationDevice() {
    return mDefaultVisualizationDevice;
}

DeviceManager::DeviceManager() {
    // Set one random device as default device
    setDefaultDevice(getOneOpenCLDevice());
}