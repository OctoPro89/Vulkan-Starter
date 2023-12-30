#pragma once
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <defines.h>
#include <string.h>

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

/* A function for loading global level vulkan functions */
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
	for (int i = 0; i < vec_length(available_extensions); ++i)
	{
		/* Loop through the available extensions to see if the desired ones are available */
		VkExtensionProperties* extension = (VkExtensionProperties*)vec_get_at(ConstCharPointer_desired_extensions, i);
		if (!IsExtensionSupported(available_extensions, extension))
		{
			printf("ERROR: Extension named \"%s\" is not supported but needed!", extension->extensionName);
			return false;
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
			static_cast<uint32_t>();
		};
	}
}