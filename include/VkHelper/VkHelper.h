#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <defines.h>
#include <string.h>
#include <vector\vector.h>
#include <WindowHelper/WindowHelper.h>

/* A function for checking the available instance extensions */
Vec CheckAvailableInstanceExtensions()
{
	Vec tempVec = vec_create(VkExtensionProperties);
	uint32_t extensions_count = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
	if ((result != VK_SUCCESS) ||
		(extensions_count == 0)) {
		printf("ERROR: Could not get the number of instance extensions.\n");
		return nullptr;
	}

	vec_resize(tempVec, extensions_count, VkExtensionProperties);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, (VkExtensionProperties*)tempVec);
	if ((result != VK_SUCCESS) ||
		(extensions_count == 0)) {
		printf("ERROR: Could not enumerate instance extensions.\n");
		return nullptr;
	}

	return tempVec;
}

/* A function for checking is a given extension is supported */
/* @param Pass in a Vector of available extensions */
/* @param Pass in the name of the extension to be checked */
bool IsExtensionSupported(Vec VkExtensionProperties_available_extensions, const char* extension)
{
	/* Doing the array / vector loop the old fashioned way because we can't use ranged based loop */
	for (int i = 0; i < vec_length(VkExtensionProperties_available_extensions); ++i) {
		VkExtensionProperties* available_extension = (VkExtensionProperties*)vec_get_at(VkExtensionProperties_available_extensions, i);
		if (strstr(available_extension->extensionName, extension)) {
			return true;
		}
	}
	return false;
}

/* A function that creates a Vulkan Instance */
/* @param Pass in a Vector that has the Desired Extensions, pass in null if you don't need extra extensions */
/* @param A string for the application name */
/* @param The VkInstance to be created */
bool CreateVulkanInstance(Vec ConstCharPointer_desired_extensions, const char* applicationName, VkInstance* Inst)
{
	Vec available_extensions = CheckAvailableInstanceExtensions();
	if (available_extensions == nullptr)
		return false;

	/* Don't loop through if we don't need extra extensions */
	if (ConstCharPointer_desired_extensions != nullptr)
	{
		/* Doing the array / vector loop the old fashioned way because we can't use ranged based loop */
		for (int i = 0; i < vec_length(ConstCharPointer_desired_extensions); ++i)
		{
			/* Loop through the available extensions to see if the desired ones are available */
			const char** extension = (const char**)vec_get_at(ConstCharPointer_desired_extensions, i);
			if (!IsExtensionSupported(available_extensions, *extension))
			{
				printf("ERROR: Extension named \"%s\" is not supported but needed!", (char*)*extension);
				return false;
			}
		}
	}

	u32 desiredExtensionsLength = ConstCharPointer_desired_extensions != nullptr ? (u32)(vec_length(ConstCharPointer_desired_extensions)) : 0; /* Again, don't use extensions if we don't want them or we will crash */

	// Create Vulkan instance
	VkApplicationInfo appInfo;
	memset(&appInfo, 0, sizeof(VkApplicationInfo));
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "nullpointer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Nullpointer Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo;
	memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = desiredExtensionsLength;
	createInfo.ppEnabledExtensionNames = (const char *const*)ConstCharPointer_desired_extensions;
	if (vkCreateInstance(&createInfo, nullptr, Inst) != VK_SUCCESS) {
		// Handle instance creation failure
		printf("ERROR: Could not create Vulkan instance");
		vec_destroy(available_extensions);
		return false;
	}

	vec_destroy(available_extensions);

	return true;
}

/* A function to get the available physical devices */
/* @param the VkInstance to get screened */
Vec EnumerateAvailablePhysicalDevices(VkInstance* instance)
{
	u32 devices_count = 0;
	VkResult result = VK_SUCCESS;
	Vec tempDeviceVec = vec_create(VkPhysicalDevice);

	result = vkEnumeratePhysicalDevices(*instance, &devices_count, nullptr);
	if ((result != VK_SUCCESS) || (devices_count == 0))
	{
		printf("ERROR: Could not get the number of available physical devices\n");
		vec_destroy(tempDeviceVec);
		return nullptr;
	}

	vec_resize(tempDeviceVec, devices_count, VkPhysicalDevice);
	result = vkEnumeratePhysicalDevices(*instance, &devices_count, (VkPhysicalDevice*)tempDeviceVec);
	if ((result != VK_SUCCESS) || (devices_count == 0))
	{
		printf("ERROR: Could not enumerate physical devices\n");
		vec_destroy(tempDeviceVec);
		return nullptr;
	}

	return tempDeviceVec;
}

/* A function to get the available extensions for a given device */
/* @param The Physical Device to be screened */
/* @param A Vector for the available extensions */
Vec CheckAvailableDeviceExtensions(VkPhysicalDevice* physical_device)
{
	Vec tempVecExtensionProperties = vec_create(VkExtensionProperties);
	u32 extensions_count = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateDeviceExtensionProperties(*physical_device, nullptr, &extensions_count, nullptr);
	if ((result != VK_SUCCESS) || (extensions_count == 0))
	{
		printf("ERROR: Could not get the number of device extensions.\n");
		vec_destroy(tempVecExtensionProperties);
		return nullptr;
	}

	vec_resize(tempVecExtensionProperties, extensions_count, VkExtensionProperties);
	result = vkEnumerateDeviceExtensionProperties(*physical_device, nullptr, &extensions_count, (VkExtensionProperties*)vec_get_at(tempVecExtensionProperties, 0));
	if ((result != VK_SUCCESS) || (extensions_count == 0))
	{
		printf("ERROR: Could not enumerate device extensions.\n");
		vec_destroy(tempVecExtensionProperties);
		return nullptr;
	}

	return tempVecExtensionProperties;
}

/* A function to get the features and properties of a physical device */
/* @param The physical device to be screened */
/* @param A pointer to a VkPhysicalDeviceFeatures to be filled in */
/* @param A pointer to a VkPhysicalDeviceProperties to be filled in */
void GetFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice* physicalDevice, VkPhysicalDeviceFeatures* deviceFeatures, VkPhysicalDeviceProperties* deviceProperties)
{
	vkGetPhysicalDeviceFeatures(*physicalDevice, deviceFeatures);
	vkGetPhysicalDeviceProperties(*physicalDevice, deviceProperties);
}

/* A function to check the available Queue Families and their properties */
/* @param The physical device to be screened */
/* @param A Vector of VkQueueFamilyProperties to be filled in with properties */
Vec CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice* physicalDevice)
{
	Vec tempQueueFamilies = vec_create(VkQueueFamilyProperties);
	u32 queueFamiliesCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamiliesCount, nullptr);
	if (queueFamiliesCount == 0)
	{
		printf("ERROR: Could not get the number of queue families.\n");
		vec_destroy(tempQueueFamilies);
		return nullptr;
	}

	vec_resize(tempQueueFamilies, queueFamiliesCount, VkQueueFamilyProperties);
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamiliesCount, (VkQueueFamilyProperties*)tempQueueFamilies);
	if (queueFamiliesCount == 0)
	{
		printf("ERROR: Could not get properties of queue families.");
		vec_destroy(tempQueueFamilies);
		return nullptr;
	}

	return tempQueueFamilies;
}

/* A function to select an index of a queue family with the desired capabilities */
/* @param The physical device to be screened */
/* @param The desired capabilities wanted */
/* @param An unsigned 32 bit integer for the output index */
bool SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice* physicalDevice, VkQueueFlags desiredCapabilities, u32* queueFamilyIndex)
{
	Vec queueFamilies = vec_create(VkQueueFamilyProperties);
	queueFamilies = CheckAvailableQueueFamiliesAndTheirProperties(physicalDevice);
	if (queueFamilies == nullptr)
		return false;

	for (u32 index = 0; index < vec_length(queueFamilies); ++index)
	{
		VkQueueFamilyProperties* indexProperties = (VkQueueFamilyProperties*)vec_get_at(queueFamilies, index);
		if ((indexProperties->queueCount > 0) &&
			(indexProperties->queueFlags & desiredCapabilities))
		{
			*queueFamilyIndex = index;
			vec_destroy(queueFamilies);
			return true;
		}
	}

	vec_destroy(queueFamilies);
	return false;
}

/* A structure of data to hold data about a queue */
typedef struct {
	u32 FamilyIndex;		/* The index of the Queue Family*/
	Vec Float_Priorities;	/* A Vector of floats for the priority of the queue */
} QueueInfo;

/* A function that prints out available extensions from vulkan that are instance level */
/* @param the vector containing the extension data */
void PrintAvailableExtensionsFromVector(Vec extensionsVector)
{
	u32 temp = (u32)vec_length(extensionsVector);
	for (u32 i = 0; i < vec_length(extensionsVector); ++i)
	{
		VkExtensionProperties* extension = vec_get_at(extensionsVector, i);
		printf("AVAILABLE EXTENSION: NAME: %s, SPECIFICATION VERSION: %d\n", extension->extensionName, (int)extension->specVersion);
	}
}


/* A function to create a logical device */
/* @param The physical device */
/* @param A Vector of Queue Infos */
/* @param A Vector of strings (const char*) of the desired extensions, pass in null if you don't need extra extensions */
/* @param Some VkPhysicalDeviceFeatures for the features of the physical device */
/* @param The logical device to be filled */
bool CreateLogicalDevice(VkPhysicalDevice* physicalDevice, Vec QueueInfo_queue_infos, Vec ConstCharPointer_desired_extensions, VkPhysicalDeviceFeatures* desired_features, VkDevice* logicalDevice)
{
	Vec VkExtensionProperties_available_extensions = vec_create(VkExtensionProperties);
	VkExtensionProperties_available_extensions = CheckAvailableDeviceExtensions(physicalDevice);
	if (VkExtensionProperties_available_extensions == nullptr)
		return false;

	/* Don't go through them if we don't want / need them */
	if (ConstCharPointer_desired_extensions != nullptr)
	{
		/* Old-Fashioned array loop */
		for (int i = 0; i < vec_length(ConstCharPointer_desired_extensions); ++i)
		{
			const char** extension = (const char**)vec_get_at(ConstCharPointer_desired_extensions, i);
			if (!IsExtensionSupported(VkExtensionProperties_available_extensions, *extension))
			{
				printf("ERROR: Extension named: \"%s\" is not supported by a physical device\n", *extension);
				return false;
			}
		}
	}

	Vec VkDeviceQueueCreateInfo_queue_create_info = vec_create(VkDeviceQueueCreateInfo);;
	/* Old-Fashioned array loop */
	for (int i = 0; i < vec_length(QueueInfo_queue_infos); ++i)
	{
		QueueInfo* info = (QueueInfo*)vec_get_at(QueueInfo_queue_infos, i);
		VkDeviceQueueCreateInfo newInfo =
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr,
			0,
			info->FamilyIndex,
			(u32)(vec_length(info->Float_Priorities)),
			(const float*)info
		};
		vec_pushback(VkDeviceQueueCreateInfo_queue_create_info, newInfo, VkDeviceQueueCreateInfo);
	}

	u32 desiredExtensionsLength = ConstCharPointer_desired_extensions != nullptr ? (u32)(vec_length(ConstCharPointer_desired_extensions)) : 0; /* Again, don't use extensions if we don't want them or we will crash */

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = (u32)(vec_length(VkDeviceQueueCreateInfo_queue_create_info));
	deviceCreateInfo.pQueueCreateInfos = vec_length(VkDeviceQueueCreateInfo_queue_create_info) ? vec_get_at(VkDeviceQueueCreateInfo_queue_create_info, 0) : nullptr;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = desiredExtensionsLength;
	deviceCreateInfo.ppEnabledExtensionNames = desiredExtensionsLength > 0 ? vec_get_at(ConstCharPointer_desired_extensions, 0) : nullptr,
	deviceCreateInfo.pEnabledFeatures = desired_features;
	

	VkResult result = vkCreateDevice(*physicalDevice, &deviceCreateInfo, nullptr, logicalDevice);
	if ((result != VK_SUCCESS) || (logicalDevice == VK_NULL_HANDLE))
	{
		printf("ERROR: Could not create logical device.\n");
		vec_destroy(VkExtensionProperties_available_extensions);
		vec_destroy(VkDeviceQueueCreateInfo_queue_create_info);
		return false;
	}

	PrintAvailableExtensionsFromVector(VkExtensionProperties_available_extensions);
	vec_destroy(VkExtensionProperties_available_extensions);
	vec_destroy(VkDeviceQueueCreateInfo_queue_create_info);

	return true;
}

/* A function for obtaining a device queue from a logical device */
/* @param The logical device to get the queue from */
/* @param The index of the queue family */
/* @param The index of the queue */
/* @param A Pointer to a VkQueue to be output */
void GetDeviceQueue(VkDevice logicalDevice, u32 queueFamilyIndex, u32 queueIndex, VkQueue* queue)
{
	vkGetDeviceQueue(logicalDevice, queueFamilyIndex, queueIndex, queue);
}

/* A function to create a logical device with geometry shaders and compute queues */
/* @param The VkInstance to get available devices from and other operations */
/* @param A Pointer to a VkDevice to be used for many operations including checking for geometry shader support */
/* @param A Pointer to a graphics queue */
/* @param A Pointer to a compute queue */
bool CreateLogicalDeviceWithGeometryShaderAndGraphicsAndComputeQueues(VkInstance* instance, VkDevice* logicalDevice, VkQueue* graphicsQueue, VkQueue* computeQueue)
{
	Vec physicalDevices = vec_create(VkPhysicalDevice);

	physicalDevices = EnumerateAvailablePhysicalDevices(instance);

	/* Again, doing the whole loop through vector thing the old-fashioned way because there are no ranged based loops*/
	for (u32 i = 0; i < vec_length(physicalDevices); ++i)
	{
		VkPhysicalDevice* physicalDevice = (VkPhysicalDevice*)vec_get_at(physicalDevices, i);

		VkPhysicalDeviceFeatures deviceFeatures;
		VkPhysicalDeviceProperties deviceProperties;
		GetFeaturesAndPropertiesOfPhysicalDevice(physicalDevice, &deviceFeatures, &deviceProperties);

		if (!deviceFeatures.geometryShader) {
			printf("Device \"%s\", does NOT support geometry shaders!\n", deviceProperties.deviceName);
			continue;
		}
		else {
			memset(&deviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
			deviceFeatures.geometryShader = VK_TRUE;
		}

		u32 graphicsQueueFamilyIndex;
		if (!SelectIndexOfQueueFamilyWithDesiredCapabilities(physicalDevice, VK_QUEUE_GRAPHICS_BIT, &graphicsQueueFamilyIndex))
		{
			printf("Device \"%s\", does NOT support queue graphics!\n", deviceProperties.deviceName);
			continue;
		}

		u32 computeQueueFamilyIndex;
		if (!SelectIndexOfQueueFamilyWithDesiredCapabilities(physicalDevice, VK_QUEUE_COMPUTE_BIT, &computeQueueFamilyIndex))
		{
			printf("Device \"%s\", does NOT support queue compute!\n", deviceProperties.deviceName);
			continue;
		}

		Vec requestedQueues = vec_create(QueueInfo);
		Vec priorities = vec_create(float);
		vec_pushback(priorities, 1.0f, float);
		QueueInfo info;
		info.FamilyIndex = graphicsQueueFamilyIndex;
		info.Float_Priorities = priorities;
		if (graphicsQueueFamilyIndex != computeQueueFamilyIndex)
		{
			QueueInfo info2;
			info2.FamilyIndex = computeQueueFamilyIndex;
			info2.Float_Priorities = priorities;
			vec_pushback(requestedQueues, info2, QueueInfo);
		}

		if (!CreateLogicalDevice(physicalDevice, requestedQueues, nullptr, &deviceFeatures, logicalDevice))
		{
			printf("Failed to create logical device!\n");
			continue;
		}
		else {
			GetDeviceQueue(*logicalDevice, graphicsQueueFamilyIndex, 0, graphicsQueue);
			GetDeviceQueue(*logicalDevice, computeQueueFamilyIndex, 0, computeQueue);
			vec_destroy(requestedQueues);
			vec_destroy(physicalDevices);
			printf("Chosen device: \"%s\"", deviceProperties.deviceName);
			return true;
		}
		vec_destroy(requestedQueues);
	}

	vec_destroy(physicalDevices);
	printf("Could not find a usable device!\n");
	return false;
}

/* A function to create a Vulkan Instance with the required Windows / Linux windowing extensions */
/* @param A Vector of extra extensions besides the windowing ones, please pass in a VALID vector not nullptr if no extra are required */
/* @param The application name */
/* @param The instance to be output to */
bool CreateVulkanInstanceWithWsiExtensionsEnabled(Vec extraExtensions, const char* applicationName, VkInstance* instance) 
{
	const char* platformSurfaceExtensionKHR;
#ifdef VK_USE_PLATFORM_WIN32_KHR
	platformSurfaceExtensionKHR = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined VK_USE_PLATFORM_XCB_KHR
	platformSurfaceExtensionKHR = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#elif defined VK_USE_PLATFORM_XLIB_KHR
	platformSurfaceExtensionKHR = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#endif

	extraExtensions = vec_reserve(const char*, sizeof(VK_KHR_SURFACE_EXTENSION_NAME) + sizeof(platformSurfaceExtensionKHR) + sizeof(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
	vec_pushback(extraExtensions, VK_KHR_SURFACE_EXTENSION_NAME, const char*);
	vec_pushback(extraExtensions, platformSurfaceExtensionKHR, const char*);

	return CreateVulkanInstance(extraExtensions, applicationName, instance);
}

/* A function to create a Vulkan Logical Device with the required Windows / Linux windowing extensions */
/* @param The physical device to create the logical one with */
/* @param A Vector of queue infos */
/* @param A Vector of extra extensions besides the default ones, please pass in a VALID vector not nullptr if no extra are required */
/* @param The desired features name */
/* @param The device to be output to */
bool CreateLogicalDeviceWithWsiExtensionsEnabled(VkPhysicalDevice* physicalDevice, Vec queueInfos, Vec desiredExtensions, VkPhysicalDeviceFeatures* desiredFeatures, VkDevice* logicalDevice) 
{
	desiredExtensions = vec_reserve(const char*, sizeof(desiredExtensions) + sizeof(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
	vec_pushback(desiredExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME, const char*);
	return CreateLogicalDevice(physicalDevice, queueInfos, desiredExtensions, desiredFeatures, logicalDevice);
}

/* A function to create a presentation surface to display on */
/* @param A Pointer to a VkInstance */
/* @param Some window perameters that are platform-specific */
/* @param A Pointer to a VkSurfaceKHR to be output to */
bool CreatePresentationSurface(VkInstance* instance, WindowPerameters params, VkSurfaceKHR* presentationSurface)
{
	VkResult result;

#ifdef VK_USE_PLATFORM_WIN32_KHR
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = params.HInstance;
	surfaceCreateInfo.hwnd = params.HWnd;

	result = vkCreateWin32SurfaceKHR(*instance, &surfaceCreateInfo, nullptr, presentationSurface);
#elif defined VK_USE_PLATFORM_XLIB_KHR

	VkXlibSurfaceCreateInfoKHR surfaceCreateInfo;

	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   
	surfaceCreateInfo.pNext = nullptr,                                          
	surfaceCreateInfo.flags = 0,                                                
	surfaceCreateInfo.dpy = perams.Dpy,                           
	surfaceCreateInfo.window = perams.Window                      

	result = vkCreateXlibSurfaceKHR(*instance, &surface_create_info, nullptr, &presentationSurface);

#elif defined VK_USE_PLATFORM_XCB_KHR

VkXcbSurfaceCreateInfoKHR surfaceCreateInfo
	VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    
	nullptr,                               
	0,                                     
	perams.Connection,                     
	perams.Window                          

result = vkCreateXcbSurfaceKHR(*instance, &surface_create_info, nullptr, &presentationSurface);

#endif

	if ((result != VK_SUCCESS) || (presentationSurface == VK_NULL_HANDLE)) {
		printf("ERROR: Could not create presentation surface.\n");
		return false;
	}

	return true;
}

/* A function to select a queue family that supports presentation to a given surface */
/* @param A Pointer to a physical device such as a GPU to be used for getting properties */
/* @param A Pointer to a surface that is used to get surface support */
/* @param A Pointer to a 32 bit unsigned integer number for the output queueFamilyIndex */
bool SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* presentationSurface, u32* queueFamilyIndex)
{
	Vec queueFamilies = vec_create(VkQueueFamilyProperties);
	queueFamilies = CheckAvailableQueueFamiliesAndTheirProperties(physicalDevice);
	if (queueFamilies == nullptr)
	{
		vec_destroy(queueFamilies);
		return false;
	}

	for (u32 index = 0; index < (u32)vec_length(queueFamilies); ++index)
	{
		VkBool32 presentationSupported = VK_FALSE;
		VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, index, *presentationSurface, &presentationSupported);
		if ((result == VK_SUCCESS) && (presentationSupported == VK_TRUE))
		{
			*queueFamilyIndex = index;
			return true;
		}
	}

	vec_destroy(queueFamilies);
	return false;
}

/* A function that selects a desired presentation mode */
/* @param A Pointer to a physical device */
/* @param A Pointer to a surface */
/* @param A VkPresentModeKHR that represents the desired present mode */
/* @param A Pointer to a present mode to be the output present mode */
bool SelectDesiredPresentationMode(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* presentationSurface, VkPresentModeKHR desiredPresentMode, VkPresentModeKHR* presentMode)
{
	u32 presentModesCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *presentationSurface, &presentModesCount, nullptr);
	if ((result != VK_SUCCESS) || (presentModesCount == 0))
	{
		printf("ERROR: Could not get the number of supported present modes.\n");
		return false;
	}

	Vec presentModes = vec_create(VkPresentModeKHR);
	vec_resize(presentModes, presentModesCount, VkPresentModeKHR);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *presentationSurface, &presentModesCount, (VkPresentModeKHR*)presentModes);
	if ((result != VK_SUCCESS) || (presentModesCount == 0))
	{
		printf("ERROR: Could not enumerate present modes.\n");
		return false;
	}

	/* Old Fashioned array loop */
	for (u32 i = 0; i < vec_length(presentModes); ++i)
	{
		VkPresentModeKHR* crntPresentMode = (VkPresentModeKHR*)vec_get_at(presentModes, i);
		if (*crntPresentMode == desiredPresentMode)
		{
			*presentMode = desiredPresentMode;
			printf("Found usable present mode!");
			return true;
		}
	}

	printf("WARNING: Desired Present Mode not supported, Falling back on FIFO (V-SYNC) Present Mode!\n");
	/* Old Fashioned array loop */
	for (u32 i = 0; i < vec_length(presentModes); ++i)
	{
		VkPresentModeKHR* crntPresentMode = (VkPresentModeKHR*)vec_get_at(presentModes, i);
		if (*crntPresentMode == VK_PRESENT_MODE_FIFO_KHR)
		{
			*presentMode = VK_PRESENT_MODE_FIFO_KHR;
			printf("Using FIFO (V-SYNC) Present Mode!");
			return true;
		}
	}

	printf("ERROR: Could not find available present mode!\n");

	return false;
}

/* A function to get the capabilities of a presentation surface */
/* @param A Pointer to a physical device */
/* @param A Pointer to a VkSurfaceKHR */
/* @param A Pointer to a VkSurfaceCapabilitiesKHR for output */
bool GetCapabilitiesOfPresentationSurface(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* presentationSurface, VkSurfaceCapabilitiesKHR* surfaceCapabilities)
{
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physicalDevice, *presentationSurface, surfaceCapabilities);

	if (result != VK_SUCCESS)
	{
		printf("ERROR: Could not get the capabilities of a presentation surface.\n");
		return false;
	}

	return true;
}

/* A function for selecting the number of swapchain images */
/* @param A Pointer to a VkSurfaceCapabilitiesKHR used for screening */
/* @param A 32 bit unsigned integer for output of the number of swap chain images */
bool SelectNumberOfSwapchainImages(VkSurfaceCapabilitiesKHR* surfaceCapabilities, u32* numberOfImages)
{
	*numberOfImages = surfaceCapabilities->minImageCount + 1;
	if ((surfaceCapabilities->maxImageCount > 0) && (*numberOfImages > surfaceCapabilities->maxImageCount))
	{
		*numberOfImages = surfaceCapabilities->maxImageCount;
	}
	return true;
}

/* A function to choose the appropriate size of images for a swapchain based on surface capabilities */
/* @param The VkSurfaceCapabilitiesKHR to be screened */
/* @param The output VkExtent2D */
bool ChooseSizeofSwapchainImages(VkSurfaceCapabilitiesKHR* surfaceCapabilities, VkExtent2D* sizeOfImages)
{
	if (surfaceCapabilities->currentExtent.width = 0xFFFFFFFF) 
	{
		sizeOfImages->width = 640;
		sizeOfImages->height = 480;

		if (sizeOfImages->width < surfaceCapabilities->minImageExtent.width) {
			sizeOfImages->width = surfaceCapabilities->minImageExtent.width;
		}
		else if (sizeOfImages->width > surfaceCapabilities->maxImageExtent.width) {
			sizeOfImages->width = surfaceCapabilities->maxImageExtent.width;
		}

		if (sizeOfImages->height < surfaceCapabilities->minImageExtent.height) {
			sizeOfImages->height = surfaceCapabilities->minImageExtent.height;
		}
		else if (sizeOfImages->height > surfaceCapabilities->maxImageExtent.height) {
			sizeOfImages->height = surfaceCapabilities->maxImageExtent.height;
		}
	}
	else {
		*sizeOfImages = surfaceCapabilities->currentExtent;
	}
	return true;
}

/* A function for selecting desired usage of swapchain produced images */
/* @param A Pointer to a VkSurfaceCapabilitiesKHR */
/* @param A VkImageUsageFlags for the desired usage */
/* @param A Pointer to a VkImageUsageflags for output */
bool SelectDesiredUsageScenariosOfSwapchainImages(VkSurfaceCapabilitiesKHR* surfaceCapabilities, VkImageUsageFlags desiredUsages, VkImageUsageFlags* imageUsages)
{
	*imageUsages = desiredUsages & surfaceCapabilities->supportedUsageFlags;
	return desiredUsages == *imageUsages;
}

/* A function for selection the transformation of swapchain images (useful for mobile / vertical mode) */
/* A VkSurfaceTransformFlagBitsKHR for the desired transform */
/* A VkSurfaceTransformFlagBitsKHR for the output transform */
bool SelectTransformationOfSwapchainImages(VkSurfaceCapabilitiesKHR* surfaceCapabilities, VkSurfaceTransformFlagBitsKHR desiredTransform, VkSurfaceTransformFlagBitsKHR* surfaceTransform)
{
	if (surfaceCapabilities->supportedTransforms & desiredTransform)
	{
		*surfaceTransform = desiredTransform;
	}
	else {
		*surfaceTransform = surfaceCapabilities->currentTransform;
	}
	return true;
}

/* A function for selecting the format of swapchain images */
/* @param A Pointer to a physical device */
/* @param A Pointer to a surface for the presentation surface */
/* @param A Pointer to a surface format khr for the desired format */
/* @param A Pointer to a format to be output to */
/* @param A Pointer to a color space khr */
bool SelectFormatOfSwapchainImages(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* presentationSurface, VkSurfaceFormatKHR* desiredSurfaceFormat, VkFormat* imageFormat, VkColorSpaceKHR* imageColorSpace)
{
	/* Enumerate supported format */
	u32 formatsCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *presentationSurface, &formatsCount, nullptr);
	if ((VK_SUCCESS != result) || (formatsCount == 0))
	{
		printf("Could not get the number of supported surfac formats.\n");
		return false;
	}

	Vec surfaceFormats = vec_create(VkSurfaceFormatKHR);
	vec_resize(surfaceFormats, formatsCount, VkSurfaceFormatKHR);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *presentationSurface, &formatsCount, surfaceFormats);
	if ((VK_SUCCESS != result) || (formatsCount == 0))
	{
		printf("Could not enumerate supported surface formats.\n");
		vec_destroy(surfaceFormats);
		return false;
	}

	VkSurfaceFormatKHR* first = vec_get_at(surfaceFormats, 0);

	if ((vec_length(surfaceFormats) == 1) && (first->format == VK_FORMAT_UNDEFINED))
	{
		*imageFormat = desiredSurfaceFormat->format;
		*imageColorSpace = desiredSurfaceFormat->colorSpace;
		vec_destroy(surfaceFormats);
		return true;
	}

	/* Old fashioned vector / array loop */
	for (u32 i = 0; i < vec_length(surfaceFormats); ++i)
	{
		VkSurfaceFormatKHR* surfaceFormat = (VkSurfaceFormatKHR*)vec_get_at(surfaceFormats, i);
		if ((surfaceFormat->format == desiredSurfaceFormat->format) && (surfaceFormat->colorSpace == desiredSurfaceFormat->colorSpace))
		{
			*imageFormat = desiredSurfaceFormat->format;
			*imageColorSpace = desiredSurfaceFormat->colorSpace;
			vec_destroy(surfaceFormats);
			return true;
		} 
		else if (surfaceFormat->format == desiredSurfaceFormat->format)
		{
			*imageFormat = desiredSurfaceFormat->format;
			*imageColorSpace = surfaceFormat->colorSpace;
			printf("WARNING: Desired combination of format and color space not supported! Falling back on other colorspace.\n");
			vec_destroy(surfaceFormats);
			return true;
		}
	}

	*imageFormat = first->format;
	*imageColorSpace = first->colorSpace;
	printf("WARNING: Desired format is not supported. Selecting available format and colorspace combination!\n");
	vec_destroy(surfaceFormats);
	return true;
}

/* A function to clean up created vulkan resources */
/* @param A pointer to the resource to cleanup */
void VulkanInstanceCleanup(VkInstance* instance)
{
	if (instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(*instance, nullptr);
		instance = VK_NULL_HANDLE;
	}
}

/* A function to clean up created vulkan resources */
/* @param A pointer to the resource to cleanup */
void VulkanDeviceCleanup(VkDevice* logicalDevice)
{
	if (logicalDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(*logicalDevice, nullptr);
		logicalDevice = VK_NULL_HANDLE;
	}
}

/* A function to clean up created vulkan resources */
/* @param A pointer to the resource to cleanup */
void VulkanSurfaceCleanup(VkInstance* instance, VkSurfaceKHR* surface)
{
	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(*instance, *surface, nullptr);
		surface = VK_NULL_HANDLE;
	}
}

/* A function to clean up created vulkan resources */
/* @param A pointer to the resource to cleanup */
void VulkanSwapchainCleanup()
{

}

/* A function to create a swapchain */
/* @param A Pointer to a logical device */
/* @param A Pointer to a presentation surface */
/* @param A Pointer to a 32 bit unsigned integer for output */
/* @param A Pointer to a surface format */
/* @param A Pointer to the image size */
/* @param A Pointer to the image usage flags */
/* @param A Pointer to the surface transform */
/* @param A Pointer to a present mode */
/* @param A Pointer to the old swapchain (null if none) */
/* @param A Pointer to a new swapchain for output */
bool CreateSwapchain(VkDevice* logicalDevice, VkSurfaceKHR* presentationSurface, u32* imageCount, VkSurfaceFormatKHR* surfaceFormat, VkExtent2D* imageSize,
	VkImageUsageFlags* imageUsage, VkSurfaceTransformFlagBitsKHR* surfaceTransform, VkPresentModeKHR* presentMode, VkSwapchainKHR* oldSwapchain, VkSwapchainKHR* swapchain)
{
	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = *presentationSurface;
	swapchainCreateInfo.minImageCount = *imageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat->format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat->colorSpace;
	swapchainCreateInfo.imageExtent = *imageSize;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = *imageUsage;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.preTransform = *surfaceTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = *presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = *oldSwapchain;
	

	VkResult result;
	result = vkCreateSwapchainKHR(*logicalDevice, &swapchainCreateInfo, nullptr, swapchain);
	if ((result != VK_SUCCESS) || (swapchain == VK_NULL_HANDLE))
	{
		printf("ERROR: Could not create swapchain!\n");
		return false;
	}

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(*logicalDevice, *oldSwapchain, nullptr);
	}

	return true;
}

/* A function for getting a vector of swapchain images */
/* @param A Pointer to a logical device */
/* @param A Pointer to the swapchain */
Vec GetSwapchainImageHandles(VkDevice* logicalDevice, VkSwapchainKHR* swapchain)
{
	Vec tempVec = vec_create(VkImage);

	u32 imageCount = 0;
	VkResult result = VK_SUCCESS;
	
	result = vkGetSwapchainImagesKHR(*logicalDevice, *swapchain, &imageCount, nullptr);
	if ((result != VK_SUCCESS) || (imageCount == 0))
	{
		printf("ERROR: Could not get the number of swapchain images!\n");
		return nullptr;
	}

	vec_resize(tempVec, imageCount, VkImage);
	result = vkGetSwapchainImagesKHR(*logicalDevice, *swapchain, &imageCount, (VkImage*)tempVec);
	if ((result != VK_SUCCESS) || (imageCount == 0))
	{
		printf("ERROR: Could not enumerate swapchain images!\n");
		return nullptr;
	}

	return tempVec;
}

/* A function for creating a swapchain with R8G8B8A8 format and a mailbox present mode */
/* @param A Pointer to a physical device */
/* @param A Pointer to a presentation surface */
/* @param A Pointer to a logical device */
/* @param A Pointer to the usage flags */
/* @param A Pointer to the image size */
/* @param A Pointer to the image format */
/* @param A Pointer to the old swapchain (null if none) */
/* @param A Pointer to a new swapchain for output */
bool CreateSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* presentationSurface, VkDevice* logicalDevice, VkImageUsageFlags* swapchainImageUsage,
	VkExtent2D* imageSize, VkFormat* imageFormat, VkSwapchainKHR* oldSwapchain, VkSwapchainKHR* swapchain, Vec swapchainImages)
{
	VkPresentModeKHR* desiredPresentMode = nullptr;
	if (!SelectDesiredPresentationMode(physicalDevice, presentationSurface, VK_PRESENT_MODE_MAILBOX_KHR, desiredPresentMode))
	{
		printf("ERROR: Could not use mailbox presentation mode!\n");
		return false;
	}

	VkSurfaceCapabilitiesKHR* surfaceCapabilities = nullptr;
	if (!GetCapabilitiesOfPresentationSurface(physicalDevice, presentationSurface, surfaceCapabilities))
	{
		printf("ERROR: Could not get capabilities of presentation surface!\n");
		return false;
	}

	u32* numberOfImages = 0;
	if (!SelectNumberOfSwapchainImages(surfaceCapabilities, numberOfImages))
	{
		printf("ERROR: Could not select number of swapchain images!\n");
		return false;
	}

	if (!ChooseSizeofSwapchainImages(surfaceCapabilities, imageSize))
	{
		printf("ERROR: Could not choose size of swapchain images!\n");
		return false;
	}

	VkImageUsageFlags* imageUsage = nullptr;
	if (!SelectDesiredUsageScenariosOfSwapchainImages(surfaceCapabilities, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, imageUsage))
	{
		printf("ERROR: Could not select desired usage scenarios of swapchain images!\n");
		return false;
	}

	VkSurfaceTransformFlagBitsKHR* surfaceTransform = nullptr;
	SelectTransformationOfSwapchainImages(surfaceCapabilities, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, surfaceTransform);
	
	VkColorSpaceKHR* imageColorSpace = nullptr;
	VkSurfaceFormatKHR desiredFormat[] = {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	if (!SelectFormatOfSwapchainImages(physicalDevice, presentationSurface, &desiredFormat[0], imageFormat, imageColorSpace))
	{
		printf("ERROR: Could not select format of swapchain images!\n");
		return false;
	}

	VkSurfaceFormatKHR* formatToUse = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR));

	if (!CreateSwapchain(logicalDevice, presentationSurface, numberOfImages, formatToUse, imageSize, imageUsage, surfaceTransform, desiredPresentMode, oldSwapchain, swapchain))
	{
		printf("ERROR: Could not create swapchain!\n");
		free(formatToUse);
		return false;
	}

	swapchainImages = GetSwapchainImageHandles(logicalDevice, swapchain);

	if (swapchainImages == nullptr)
	{
		printf("ERROR: Could not get swapchain image handles!\n");
		free(formatToUse);
		return false;
	}

	printf("INFO: Created Swapchain successfully!\n");
	free(formatToUse);
	return true;
}
