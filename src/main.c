#include <defines.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector/vector.h>
#include <VkHelper/VkHelper.h>

bool Start()
{
	Vec availableExtensions = nullptr;
	Vec desiredExtensions = vec_create(VkExtensionProperties);
	VkInstance Inst;
	VkDevice logicalDevice;

	availableExtensions = CheckAvailableInstanceExtensions();
	if (availableExtensions == nullptr)
		return false;

	VkExtensionProperties* lol = vec_get_at(availableExtensions, 0);

	vec_pushback(desiredExtensions, *lol, VkExtensionProperties);

	if (!CreateVulkanInstance(desiredExtensions, "nullpointer", &Inst))
		return false;

	PrintAvailableInstanceExtensionsFromVector(availableExtensions);

	vec_destroy(availableExtensions);
	vec_destroy(desiredExtensions);

	return true;
}

int main(int argc, char** argv[])
{
	if (!Start())
		exit(1);

	while (true);
	
	return 0;
}