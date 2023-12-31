#pragma once
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <defines.h>
#include <string.h>
#include <vector\vector.h>

/* A function for checking the available instance extensions */
/* @param Pass in a nullptr vector which will be modified to reserve a size suffecient enough for the extensions */
bool CheckAvailableInstanceExtensions(Vec VkExtensionProperties_available_extensions)
{
	u32 extensions_count = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
	if ((result != VK_SUCCESS) || (extensions_count == 0))
	{
		printf("Could not get the number of Instance Extensions!\n");
		return false;
	}

	/* reserve memory to fit the needs of the vkEnumerateInstanceExtensionProperties */
	VkExtensionProperties_available_extensions = vec_reserve(VkExtensionProperties, extensions_count);

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, (VkExtensionProperties*)VkExtensionProperties_available_extensions);
	if ((result != VK_SUCCESS) || (extensions_count == 0))
	{
		printf("Could not enumerate Instance extensions!\n");
		return false;
	}

	return true;
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
/* @param Pass in a Vector that has the Desired Extensions */
/* @param A string for the application name */
/* @param The VkInstance to be created */
bool CreateVulkanInstance(Vec ConstCharPointer_desired_extensions, const char* applicationName, VkInstance* instance)
{
	Vec available_extensions = vec_create(VkExtensionProperties);
	if (!CheckAvailableInstanceExtensions(available_extensions))
		return false;
	
	/* Doing the array / vector loop the old fashioned way because we can't use ranged based loop */
	for (int i = 0; i < vec_length(ConstCharPointer_desired_extensions); ++i)
	{
		/* Loop through the available extensions to see if the desired ones are available */
		VkExtensionProperties* extension = (VkExtensionProperties*)vec_get_at(ConstCharPointer_desired_extensions, i);
		if (!IsExtensionSupported(available_extensions, extension->extensionName))
		{
			printf("ERROR: Extension named \"%s\" is not supported but needed!", extension->extensionName);
			return false;
		}
	}

	VkApplicationInfo applicationInfo =
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		applicationName,
		VK_MAKE_VERSION(1, 0, 0),
		"nullpointer",
		VK_MAKE_VERSION(1, 0, 0),
		VK_MAKE_VERSION(1, 0, 0)
	};

	VkInstanceCreateInfo instanceCreateInfo =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&applicationInfo,
		0,
		nullptr,
		vec_length(ConstCharPointer_desired_extensions),
		vec_length(ConstCharPointer_desired_extensions) > 0 ? vec_get_at(ConstCharPointer_desired_extensions, 0) : nullptr
	};

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, instance);
	if ((result != VK_SUCCESS) || (instance == VK_NULL_HANDLE))
	{
		printf("o crap\n");
		return false;
	}
	
	vec_destroy(available_extensions);

	return true;
}

/* A function to get the available physical devices */
/* @param the VkInstance to get screened */
/* @param A Vector to be filled with VkPhysicalDevices for the available devices */
bool EnumerateAvailablePhysicalDevices(VkInstance instance, Vec VkPhysicalDevice_available_devices)
{
	u32 devices_count = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
	if ((result != VK_SUCCESS) || (devices_count = 0))
	{
		printf("ERROR: Could not get the number of available physical devices\n");
		return false;
	}

	VkPhysicalDevice_available_devices = vec_reserve(VkPhysicalDevice, devices_count);
	result = vkEnumeratePhysicalDevices(instance, &devices_count, vec_get_at(VkPhysicalDevice_available_devices, 0));
	if ((result != VK_SUCCESS) || (devices_count == 0))
	{
		printf("ERROR: Could not enumerate physical devices\n");
		return false;
	}

	return true;
}

/* A function to get the available extensions for a given device */
/* @param The Physical Device to be screened */
/* @param A Vector for the available extensions */
bool CheckAvailableDeviceExtensions(VkPhysicalDevice physical_device, Vec VkExtensionProperties_available_extensions)
{
	u32 extensions_count = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr);
	if ((result != VK_SUCCESS) || (extensions_count == 0))
	{
		printf("ERROR: Could not get the number of device extensions.\n");
		return false;
	}

	VkExtensionProperties_available_extensions = vec_reserve(VkExtensionProperties, extensions_count);
	result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, vec_get_at(VkExtensionProperties_available_extensions, 0));
	if ((result != VK_SUCCESS) || (extensions_count == 0))
	{
		printf("ERROR: Could not enumerate device extensions.\n");
		return false;
	}

	return true;
}

/* A function to get the features and properties of a physical device */
/* @param The physical device to be screened */
/* @param A pointer to a VkPhysicalDeviceFeatures to be filled in */
/* @param A pointer to a VkPhysicalDeviceProperties to be filled in */
void GetFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* deviceFeatures, VkPhysicalDeviceProperties* deviceProperties)
{
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
}

/* A function to check the available Queue Families and their properties */
/* @param The physical device to be screened */
/* @param A Vector of VkQueueFamilyProperties to be filled in with properties */
bool CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physicalDevice, Vec VkQueueFamilyProperties_queue_families)
{
	u32 queueFamiliesCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
	if (queueFamiliesCount == 0)
	{
		printf("ERROR: Could not get the number of queue families.\n");
		return false;
	}

	VkQueueFamilyProperties_queue_families = vec_reserve(VkQueueFamilyProperties, queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, (VkQueueFamilyProperties*)VkQueueFamilyProperties_queue_families);
	if (queueFamiliesCount == 0)
	{
		printf("ERROR: Could not get properties of queue families.");
		return false;
	}

	return true;
}

/* A function to select an index of a queue family with the desired capabilities */
/* @param The physical device to be screened */
/* @param The desired capabilities wanted */
/* @param An unsigned 32 bit integer for the output index */
bool SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physicalDevice, VkQueueFlags desiredCapabilities, u32* queueFamilyIndex)
{
	Vec queueFamilies = nullptr;
	if(!CheckAvailableQueueFamiliesAndTheirProperties(physicalDevice, queueFamilies))
		return false;
	for (u32 index = 0; index < (u32)(vec_length(queueFamilies)); ++index)
	{
		VkQueueFamilyProperties* indexProperties = (VkQueueFamilyProperties*)vec_get_at(queueFamilies, index);
		if ((indexProperties->queueCount > 0) &&
			(indexProperties->queueFlags & desiredCapabilities))
		{
			queueFamilyIndex = index;
			return true;
		}
	}
	return false;
}

/* A structure of data to hold data about a queue */
struct QueueInfo {
	u32 FamilyIndex;
	Vec Float_Priorities;
};

