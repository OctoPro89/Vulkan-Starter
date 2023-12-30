#pragma once
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <defines.h>

#if defined _WIN32
#include <Windows.h>
#define LIBRARY_TYPE HMODULE
#define LoadFunction GetProcAddress
#elif defined __linux
#define LoadFunction dlsym
#endif

/* A function for connecting vulkan with the dynamic link library */
/* @param A pointer to the type of library (HMODULE on Window) */
bool ConnectWithVulkanLoaderLibrary(LIBRARY_TYPE* vulkan_library)
{
#if defined _WIN32
	*vulkan_library = LoadLibrary(L"vulkan-1.dll");
#elif defined __linux
	*vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
#endif

	if (*vulkan_library == nullptr) {
		printf("Could not connect with a Vulkan Runtime library.\n");
		return false;
	}

	return true;
}

/* A function for loading exported vulkan loader library functions */
/* @param A const pointer to the library type (HMODULE on Windows) */
bool LoadFunctionExportedFromVulkanLoaderLibrary(const LIBRARY_TYPE* vulkan_library) {
	#define EXPORTED_VULKAN_FUNCTION( name )								 \
    name = (PFN_##name)LoadFunction( *vulkan_library, #name );				 \
    if( name == nullptr ) {													 \
		printf("Could not load exported Vulkan function named: %s\n", name); \
        return false;														 \
    }

	#include <VulkanFunctions.h>

	return true;
}
bool LoadGlobalLevelFunctions() {
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name )										\
    name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name );						\
    if( name == nullptr ) {															\
		printf("Could not load global level Vulkan function named: %s\n", name);	\
		return false;																\
    }

	#include <VulkanDefines.h>

	return true;
}

/* A function for checking the available instance extensions */
/* @param Pass in a nullptr vector which will be modified to reserve a size suffecient enough for the extensions */
bool CheckAvailableInstanceExtensions(Vec VkExtensionProperties_available_extensions)
{
	uint32_t extensions_count = 0;
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

/* A function that creates a Vulkan Instance */
/* @param Pass in a Vector that has the Desired Extensions */
/* @param A string for the application name */
/* @param The VkInstance to be created */
bool CreateVulkanInstance(Vec ConstCharPointer_desired_extensions, const char* applicationName, VkInstance* instance)
{
	Vec available_extensions = vec_create(VkExtensionProperties);
	if (!CheckAvailableInstanceExtensions(available_extensions))
		return false;
}