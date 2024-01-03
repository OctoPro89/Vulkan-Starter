#include <defines.h>
#include <vector/vector.h>
#include <VkHelper/VkHelper.h>

bool Start()
{
	Vec availableExtensions = nullptr;
	Vec desiredExtensions = vec_create(const char*);
	VkInstance Inst;
	VkDevice logicalDevice;
	VkQueue GraphicsQueue;
	VkQueue ComputeQueue;

	availableExtensions = CheckAvailableInstanceExtensions();
	if (availableExtensions == nullptr)
		return false;

	if (!CreateVulkanInstanceWithWsiExtensionsEnabled(desiredExtensions, "nullpointer", &Inst))
		return false;

	PrintAvailableInstanceExtensionsFromVector(availableExtensions);

	if (!CreateLogicalDeviceWithGeometryShaderAndGraphicsAndComputeQueues(&Inst, &logicalDevice, &GraphicsQueue, &ComputeQueue))
		return false;

	vec_destroy(availableExtensions);
	vec_destroy(desiredExtensions);

	VulkanInstanceCleanup(&Inst);
	VulkanDeviceCleanup(&logicalDevice);

	return true;
}

int main(int argc, char** argv[])
{
	if (!Start())
		exit(1);
	
	return 0;
}