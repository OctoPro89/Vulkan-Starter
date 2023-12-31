#include <defines.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector/vector.h>
#include <VkHelper/VkHelper.h>

bool Initialize(Vec availableExtensions)
{
	if (!CheckAvailableInstanceExtensions(availableExtensions))
		return false;

	//if (!CreateVulkanInstance(nullptr, "nullpointer", *Instance))

	return true;
}

int main(int argc, char** argv[])
{
	Vec availableExtensions = nullptr;
	Vec desiredExtensions = vec_reserve(VkExtensionProperties, 1);
	if (!Initialize(availableExtensions))
		exit(1);

	vec_destroy(availableExtensions);
	vec_destroy(desiredExtensions);
	
	return 0;
}