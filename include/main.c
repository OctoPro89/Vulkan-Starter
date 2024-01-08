#include <defines.h>
#include <windows.h>
#include <vector/vector.h>
#include <VkHelper/VkHelper.h>

/* Global variables */
HINSTANCE hInstance;
HWND hWnd;
VkSurfaceKHR PresentationSurface = { 0 };
VkInstance Inst = { 0 };
VkDevice logicalDevice = { 0 };
VkQueue GraphicsQueue = { 0 };
VkQueue ComputeQueue = { 0 };
WindowPerameters window_parameters = { 0 };
VkSwapchainKHR swapchain = { 0 };
VkSemaphore imageAcquiredSemaphore = { 0 };
VkSemaphore readyToPresentSemaphore = { 0 };
Vec swapchainImages = nullptr;
Vec physicalDevices = nullptr;

bool CreateAppInstance()
{
	Vec availableExtensions = nullptr;
	Vec desiredExtensions = vec_create(const char*);
	availableExtensions = CheckAvailableInstanceExtensions();
	if (availableExtensions == nullptr)
		return false;

	PrintAvailableExtensionsFromVector(availableExtensions);

	if (!CreateVulkanInstanceWithWsiExtensionsEnabled(desiredExtensions, "nullpointer", &Inst))
		return false;

	vec_destroy(availableExtensions);
	vec_destroy(desiredExtensions);
	return true;
}

bool CreateAppSwapchain(VkPhysicalDevice* PhysicalDevice)
{
	bool Ready = false;

	VkFormat swapchainImageFormat = { 0 };
	VkExtent2D swapchainImageSize = { 0 };
	Vec swapchainImages = vec_create(VkImage);

	if (!CreateSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(PhysicalDevice, &PresentationSurface, &logicalDevice, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &swapchainImageSize, &swapchainImageFormat, nullptr, &swapchain, swapchainImages))
		return false;

	if (swapchain)
		Ready = true;

	return Ready;
}

bool CreateAppDeviceAndPhysicalDeviceAndSwapchain()
{
	u32 GraphicsQueueFamilyIndex = 0;
	u32 PresentQueueFamilyIndex = 0;
	physicalDevices = EnumerateAvailablePhysicalDevices(&Inst);

	VkSurfaceCapabilitiesKHR capabilities;

	for (u32 i = 0; i < vec_length(physicalDevices); ++i)
	{
		VkPhysicalDevice* physicalDevice = (VkPhysicalDevice*)vec_get_at(physicalDevices, i);
		if (!SelectIndexOfQueueFamilyWithDesiredCapabilities(physicalDevice, VK_QUEUE_GRAPHICS_BIT, &GraphicsQueueFamilyIndex)) {
			return false;
		}

		if (!SelectQueueFamilyThatSupportsPresentationToGivenSurface(physicalDevice, &PresentationSurface, &PresentQueueFamilyIndex)) {
			return false;
		}

		if (!GetCapabilitiesOfPresentationSurface(physicalDevice, &PresentationSurface, &capabilities))
		{
			printf("DEBUG: CreateAppDevice - Presentation Surface After Check (Failed): %p\n", PresentationSurface);
			printf("ERROR: Could not get capabilities of presentation surface!\n");
			return false;
		}

		Vec priorities;
		priorities = vec_create(float);
		vec_pushback(priorities, 1.0f, float);
		QueueInfo infoGraphics = { 0 };
		infoGraphics.FamilyIndex = PresentQueueFamilyIndex;
		infoGraphics.Float_Priorities = priorities;

		QueueInfo infoPresent = { 0 };
		infoGraphics.FamilyIndex = GraphicsQueueFamilyIndex;
		infoGraphics.Float_Priorities = priorities;

		Vec requested_queues = vec_create(QueueInfo);
		vec_pushback(requested_queues, infoGraphics, QueueInfo);
		if (GraphicsQueueFamilyIndex != PresentQueueFamilyIndex) {
			vec_pushback(requested_queues, infoPresent, QueueInfo);
		}
		Vec device_extensions = vec_create(VkExtensionProperties);
		if (!CreateLogicalDeviceWithWsiExtensionsEnabled(physicalDevice, requested_queues, device_extensions, nullptr, &logicalDevice))
			return false;
		else
			return CreateAppSwapchain(physicalDevice);
	}

	return true;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

bool CreateWin()
{
	hInstance = GetModuleHandle(NULL);

	/* Register the window class */
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"nullpointerclass";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return false;
	}

	/* Create the window */
	hWnd = CreateWindowEx(
		0,
		L"nullpointerclass",
		L"nullpointer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return false;
	}

	/* Show the window */
	ShowWindow(hWnd, SW_SHOWNORMAL);

	(HINSTANCE*)window_parameters.HInstance = hInstance;
	(HWND*)window_parameters.HWnd = hWnd;

	return true;
}

void RunWindow()
{
	/* Message loop */
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool CreateSemaphores()
{

}

bool Start()
{
	if (!CreateWin())
		return false;

	if (!CreateAppInstance())
		return false;

	if (!CreatePresentationSurface(&Inst, window_parameters, &PresentationSurface))
		return false;

	if (!CreateAppDeviceAndPhysicalDeviceAndSwapchain())
		return false;

	if (!CreateSemaphores())
		return false;

	RunWindow();

	VulkanSwapchainCleanup(&logicalDevice, &swapchain);
	VulkanDeviceCleanup(&logicalDevice);
	VulkanSurfaceCleanup(&Inst, &PresentationSurface);
	VulkanInstanceCleanup(&Inst);

	return true;
}

bool Draw()
{
	WaitForAllSubmittedCommandsToBeFinished(&logicalDevice);

	u32 imageIndex = 0;
	//if (!AcquireSwapchainImage(&logicalDevice, &swapchain, &imageAcquiredSemaphore, ))
		//return false;

	return true;
}

int main(int argc, char** argv[])
{

	if (!Start())
		exit(1);

	return 0;
}