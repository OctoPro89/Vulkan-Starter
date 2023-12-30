#include <defines.h>
#include <vulkan/vulkan.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector/vector.h>
#include <VkHelper/VkHelper.h>

int main(int argc, char** argv[])
{
	HMODULE vulkan_library = nullptr;

	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;

	if (!ConnectWithVulkanLoaderLibrary(&vulkan_library))
	{
		printf("frick.\n");
		return 1;
	}
	
#if defined _WIN32
	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkan_library, "vkGetInstanceProcAddr");
#elif defined __linux
	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_library, "vkGetInstanceProcAddr");
#endif

	if (vkGetInstanceProcAddr == nullptr)
	{
		printf("Could not get vkGetInstanceProcAddr\n");
		return 1;
	}

	Vec availableExtensions = nullptr;
	if (!CheckAvailableInstanceExtensions(availableExtensions))
	{
		return 1;
	}

	vec_destroy(availableExtensions);
	
	return 0;
}